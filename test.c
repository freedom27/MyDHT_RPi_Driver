#include<stdio.h>
#include <unistd.h>
#include "dht_driver.h"

int main(void)
{
    printf("Sensor test:\n");
	struct dht_sensor_data data;
	
	while(1) {
		dht_read(17, &data);
		printf("Humidity %f and Temperature %f \n", data.humidity, data.temperature);
		sleep(3);
	}

}