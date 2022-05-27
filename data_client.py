import socket
import sys

HOST, PORT = "192.168.0.196", 3333
samples = 10080
vars = 4

# Create a socket (SOCK_STREAM means a TCP socket)
with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as sock:
    # Connect to server and send data
    sock.connect((HOST, PORT))
    sock.sendall(bytes(f"{samples}", "utf-8"))

    # Receive data from the server and shut down
    # received = str(sock.recv(100), "utf-8")  # Only for text data

    data = list(sock.recv(samples * vars))

for n in range(samples):
    print(data[n * vars:(n+1)* vars])
