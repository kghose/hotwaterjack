#ifndef FLASHLED_H
#define FLASHLED_H

#define LED GPIO_NUM_2

void configure_led(void);
void flash_led(size_t t);

#endif