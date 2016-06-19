#ifndef DHT_DRIVER_H
#define DHT_DRIVER_H

#define DHT_HANDSHAKE_ERROR     -1
#define DHT_TIMEOUT_ERROR       -2
#define DHT_CHECKSUM_ERROR      -3
#define DHT_INPUT_ERROR         -4
#define DHT_INIT_ERROR          -5
#define DHT_OK                   0

#define DHT11                    0
#define DHT22                    1

#include "MyGPIO/my_gpio.h"
#include "MyGPIO/my_utils.h"
#include "MyGPIO/my_time_utils.h"

struct dht_sensor_data {
    float humidity;
    float temperature;
};

int dht_read(int type, int gpio_pin, struct dht_sensor_data *sensor_data);

#endif