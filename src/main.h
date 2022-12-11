#ifndef HEADER_FILE
#define HEADER_FILE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "driver/adc.h"

#include "../components/ssd1306/ssd1306.h"
#include "../components/ssd1306/font8x8_basic.h"

#define tag "SSD1306"

typedef struct meteor_t{ 
    unsigned int xpos;
    unsigned int ypos;
} meteor_t;

typedef struct fires_t{ 
    unsigned int xpos;
    unsigned int ypos;
} fires_t;

uint8_t spaceship[] = {
		0b00000000, 0b00000000, 0b00000000,
		0b01100000, 0b00000000, 0b00000000,
		0b01111000, 0b11000000, 0b00000000,
		0b01111111, 0b11111000, 0b00100000,
		0b01111111, 0b11111111, 0b11110000,
		0b01111111, 0b11111111, 0b11110000,
		0b01111111, 0b11111000, 0b00100000,
		0b01111000, 0b11000000, 0b00000000,
		0b01100000, 0b00000000, 0b00000000,
		0b00000000, 0b00000000, 0b00000000,
};

uint8_t clear_spaceship[] = {
		0b00000000, 0b00000000, 0b00000000,
		0b00000000, 0b00000000, 0b00000000,
		0b00000000, 0b00000000, 0b00000000,
		0b00000000, 0b00000000, 0b00000000,
		0b00000000, 0b00000000, 0b00000000,
		0b00000000, 0b00000000, 0b00000000,
		0b00000000, 0b00000000, 0b00000000,
		0b00000000, 0b00000000, 0b00000000
};

uint8_t fire[] = {
	0b10000000
};

uint8_t clear_fire[] = {
	0b00000000
};

uint8_t meteor[] = {
	0b00111100,
	0b01100110,
	0b00111100
};

uint8_t clear_meteor[] = {
	0b00000000,
	0b00000000,
	0b00000000
};

#endif