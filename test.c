#include<stdio.h>
#include <unistd.h>
#include "dht_driver.h"

int main(void)
{
    printf("Sensor test:\n");
	float t = 0.0f;
	float h = 0.0f;
	
	while(1) {
		dht_read(17, &h, &t);
		printf("Humidity %f and Temperature %f \n", h, t);
		sleep(3);
	}

}