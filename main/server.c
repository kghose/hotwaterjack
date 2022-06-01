#include <string.h>
#include <sys/param.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "esp_netif.h"

#include "lwip/err.h"
#include "lwip/sockets.h"
#include "lwip/sys.h"
#include <lwip/netdb.h>

#include "server.h"

#define PORT 3333
#define KEEPALIVE_IDLE 5     // Keep-alive idle time. In idle time without receiving any data from peer, will send keep-alive probe packet
#define KEEPALIVE_INTERVAL 5 // Keep-alive probe packet interval time.
#define KEEPALIVE_COUNT 3    // Keep-alive probe packet retry count.

static const char *TAG = "Server";

static void tx_response(const int sock, char *tx_buffer, size_t start, size_t end)
{
    // send() can return less bytes than supplied length.
    // Walk-around for robust implementation.
    int to_write = end - start;
    while (to_write > 0)
    {
        int written = send(sock, tx_buffer + (end - to_write), to_write, 0);
        if (written < 0)
        {
            ESP_LOGE(TAG, "Error occurred during sending: errno %d", errno);
        }
        to_write -= written;
    }
}

static void print_boiler_info(BoilerData const *boiler_data, const int sock)
{
    char tx_buffer[1024];
    int tx_len = boiler_info(boiler_data, tx_buffer);
    tx_response(sock, tx_buffer, 0, tx_len);
}

static void latest_sample_human_readable(BoilerData const *boiler_data, const int sock)
{
    char tx_buffer[128];
    int tx_len = human_readable(latest_sample(boiler_data), tx_buffer);
    tx_response(sock, tx_buffer, 0, tx_len);
}

static void N_recent_samples_binary(BoilerData const *boiler_data, uint16_t samples, const int sock)
{
    DataChunks data_chunks = get_data_chunks(boiler_data, samples);
    tx_response(sock, (char*)boiler_data->data, data_chunks.start[0], data_chunks.end[0]);
    if (data_chunks.has_two_chunks)
    {
        tx_response(sock, (char*)boiler_data->data, data_chunks.start[1], data_chunks.end[1]);
    }
}

static void help(const int sock)
{
    char tx_buffer[40];
    int tx_len = sprintf(tx_buffer, "Valid commands: n, i, 1, 2, 3 ...\n");
    tx_response(sock, tx_buffer, 0, tx_len);
}

static void serve_data(const int sock, BoilerData const *boiler_data)
{
    int len;
    char rx_buffer[128];

    do
    {
        len = recv(sock, rx_buffer, sizeof(rx_buffer) - 1, 0);
        if (len < 0)
        {
            ESP_LOGE(TAG, "Error occurred during receiving: errno %d", errno);
        }
        else if (len == 0)
        {
            ESP_LOGW(TAG, "Connection closed");
        }
        else
        {
            rx_buffer[len] = 0; // Null-terminate whatever is received and treat it like a string
            ESP_LOGI(TAG, "Received %d bytes: %s", len, rx_buffer);

            if (rx_buffer[0] == 'i') // "info": Stats about stuff
            {
                print_boiler_info(boiler_data, sock);
            }
            else if (rx_buffer[0] == 'n') // "now": Human readable latest reading
            {
                latest_sample_human_readable(boiler_data, sock);
            }
            else
            {
                int samples = atoi(rx_buffer);
                if (samples == 0)
                {
                    help(sock);
                }
                else
                {
                    N_recent_samples_binary(boiler_data, samples, sock);
                }
            }
        }
    } while (len > 0);
}

static void tcp_server_task(void *pvParameters)
{
    char addr_str[128];
    BoilerData *boiler_data = (BoilerData *)pvParameters;
    int ip_protocol = 0;
    int keepAlive = 1;
    int keepIdle = KEEPALIVE_IDLE;
    int keepInterval = KEEPALIVE_INTERVAL;
    int keepCount = KEEPALIVE_COUNT;
    struct sockaddr_storage dest_addr;

    struct sockaddr_in6 *dest_addr_ip6 = (struct sockaddr_in6 *)&dest_addr;
    bzero(&dest_addr_ip6->sin6_addr.un, sizeof(dest_addr_ip6->sin6_addr.un));
    dest_addr_ip6->sin6_family = AF_INET6;
    dest_addr_ip6->sin6_port = htons(PORT);
    ip_protocol = IPPROTO_IPV6;

    int listen_sock = socket(AF_INET6, SOCK_STREAM, ip_protocol);
    if (listen_sock < 0)
    {
        ESP_LOGE(TAG, "Unable to create socket: errno %d", errno);
        vTaskDelete(NULL);
        return;
    }
    int opt = 1;
    setsockopt(listen_sock, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    ESP_LOGI(TAG, "Socket created");

    int err = bind(listen_sock, (struct sockaddr *)&dest_addr, sizeof(dest_addr));
    if (err != 0)
    {
        ESP_LOGE(TAG, "Socket unable to bind: errno %d", errno);
        goto CLEAN_UP;
    }
    ESP_LOGI(TAG, "Socket bound, port %d", PORT);

    err = listen(listen_sock, 1);
    if (err != 0)
    {
        ESP_LOGE(TAG, "Error occurred during listen: errno %d", errno);
        goto CLEAN_UP;
    }

    while (1)
    {

        ESP_LOGI(TAG, "Socket listening");

        struct sockaddr_storage source_addr; // Large enough for both IPv4 or IPv6
        socklen_t addr_len = sizeof(source_addr);
        int sock = accept(listen_sock, (struct sockaddr *)&source_addr, &addr_len);
        if (sock < 0)
        {
            ESP_LOGE(TAG, "Unable to accept connection: errno %d", errno);
            break;
        }

        // Set tcp keepalive option
        setsockopt(sock, SOL_SOCKET, SO_KEEPALIVE, &keepAlive, sizeof(int));
        setsockopt(sock, IPPROTO_TCP, TCP_KEEPIDLE, &keepIdle, sizeof(int));
        setsockopt(sock, IPPROTO_TCP, TCP_KEEPINTVL, &keepInterval, sizeof(int));
        setsockopt(sock, IPPROTO_TCP, TCP_KEEPCNT, &keepCount, sizeof(int));
        // Convert ip address to string
        if (source_addr.ss_family == PF_INET)
        {
            inet_ntoa_r(((struct sockaddr_in *)&source_addr)->sin_addr, addr_str, sizeof(addr_str) - 1);
        }
        else if (source_addr.ss_family == PF_INET6)
        {
            inet6_ntoa_r(((struct sockaddr_in6 *)&source_addr)->sin6_addr, addr_str, sizeof(addr_str) - 1);
        }
        ESP_LOGI(TAG, "Socket accepted ip address: %s", addr_str);

        serve_data(sock, boiler_data);

        shutdown(sock, 0);
        close(sock);
    }

CLEAN_UP:
    close(listen_sock);
    vTaskDelete(NULL);
}

void start_data_server(BoilerData *boiler_data)
{
    xTaskCreate(tcp_server_task, "Data Server", 4096, (void *)boiler_data, 5, NULL);
}