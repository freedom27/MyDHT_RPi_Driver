#ifndef DHT_DRIVER_H
#define DHT_DRIVER_H

#define DHT_ERROR 	-1
#define DHT_OK 		0

#include "MyGPIO/my_gpio.h"
#include "MyGPIO/my_utils.h"
#include "MyGPIO/my_time_utils.h"

int dht_read(int gpio_pin, float *humidity, float *temperature);

#endif