#include <stdio.h>
#include "dht_driver.h"

#define DATA_LENGTH 40

int wait_for_value(int gpio_pin, int value);

inline int wait_for_value(int gpio_pin, int value) {
	int count = 0;
	while(gpio_read(gpio_pin) != value) {
		if(++count > 20000) // just a big number to avoid looping forever... on a RPi 2 50us are between 350 and 450 iterations
			return -1;
	}
	return count;
}

int dht_read(int gpio_pin, float *humidity, float *temperature) {
	if(humidity == NULL || temperature == NULL) {
		return DHT_ERROR;
	}
	
	if(gpio_init() == MY_GPIO_FAILURE) {
		return DHT_ERROR;
	}
	
	*humidity = 0.0f;
	*temperature = 0.0f;
	int dataBits[DATA_LENGTH] = {0};
	int waitTime[DATA_LENGTH] = {0};
	
	//since all the code below is time critical it is better to have the highest possible priority
	set_process_priority(MAX_PRIORITY);
	
	//starting handshake with sensor
	gpio_set_mode(gpio_pin, WRITE);
	gpio_write(gpio_pin, LOW);
	//according to datasheet I have to keep the pin to low at least for 10ms, to be sure I'll keep it low for 30
	sleep_ms(30);
	//now I have to pull the pin up for at leaset 20~40us
	gpio_write(gpio_pin, HIGH);
	gpio_set_mode(gpio_pin, READ);
	sleep_us(20);
	//now to see if sensor detected our request wait for it to pull the pin down
	if(wait_for_value(gpio_pin, LOW) < 0) {
		set_process_priority(DEFAULT_PRIORITY);
		return DHT_ERROR;
	}
	
	//once down the pin should stay in such state for 80us, let's wait for the pin to go up again
	if(wait_for_value(gpio_pin, HIGH) < 0) {
		set_process_priority(DEFAULT_PRIORITY);
		return DHT_ERROR;
	}
	//now th data transmission will begin soon... as the pin goes low again (after another 80us) the first bit will begin
	if(wait_for_value(gpio_pin, LOW) < 0) {
		set_process_priority(DEFAULT_PRIORITY);
		return DHT_ERROR;
	}
	//handshake ended
	//data transmission start
	
	for(int i = 0; i < DATA_LENGTH; ++i) {
		//every bit begins with a low value that lasts for 50us
		int currentWaitTime = wait_for_value(gpio_pin, HIGH);
		if(currentWaitTime < 0) {
			set_process_priority(DEFAULT_PRIORITY);
			return DHT_ERROR;
		}
		waitTime[i] = currentWaitTime;
		//once the value is HIGH i need to check for how long it stays in such state
		//0 = 26~28us
		//1 = 70us
		int currentBitTime = wait_for_value(gpio_pin, LOW);
		if(currentBitTime < 0) {
			set_process_priority(DEFAULT_PRIORITY);
			return DHT_ERROR;
		}
		dataBits[i] = currentBitTime;
	}
	//data transmission ended	
	set_process_priority(DEFAULT_PRIORITY);
	
	//now to have a time reference for the iterations let's evaluate the average number of iterations for the wait
	//cilces that is supposed to be 50us.
	//I'm calculating this now because previous operations where time critical 
	//so anything not strictly necessary was supposed to be avoided
	int avgWait = 0;
	for(int i = 0; i < DATA_LENGTH; ++i) {
		avgWait += waitTime[i];
	}
	avgWait /= DATA_LENGTH; //this should be the reference for 50us
	
	uint8_t dataBytes[5] = {0};
	for(int i = 0; i < DATA_LENGTH; ++i) {
		uint8_t currentBit = 0;
		if(dataBits[i] > avgWait) {
			currentBit = 1;
		}
		dataBytes[i / 8] |= currentBit << (7 - (i % 8));
	}
	
	//check checksum
	uint8_t checksum = (dataBytes[0] + dataBytes[1] + dataBytes[2] + dataBytes[3]) & 0xFF;
	if(checksum != dataBytes[4]) {
		return DHT_ERROR;
	}
	
	*humidity = ((dataBytes[0] << 8) | dataBytes[1]) / 10.0f;
	if(dataBytes[2] >> 7) { // first bit of temperature is 1 hence temperature is below zero
		*temperature = -1.0 * (((dataBytes[2] & 127) << 8) | dataBytes[3]) / 10.0f;
	} else
		*temperature = ((dataBytes[2] << 8) | dataBytes[3]) / 10.0f;
	
	
	return DHT_OK;
}