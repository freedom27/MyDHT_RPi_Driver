#include<stdio.h>
#include <unistd.h>
#include "dht_driver.h"

int main(void)
{
    printf("Sensor test:\n");
	struct dht_sensor_data data;
	
	while(1) {
		int ret_val = dht_read(17, &data);
		if(ret_val < 0) {
			switch(ret_val) {
				case -1:
					printf("Error: Hanshake!\n");
					break;
				case -2:
					printf("Error: Timeout!\n");
					break;
				case -3:
					printf("Error: Checksum check!\n");
					break;
				case -5:
					printf("Error: Init!\n");
					break;
			}
			
		} else {
			printf("Humidity %f and Temperature %f \n", data.humidity, data.temperature);
		}
		
		sleep(3);
	}

}