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

void app_main(void)
{
	SSD1306_t dev;
	int center = 3, top = 0, bottom = 7;
	char *lineChar = malloc(sizeof(char)*10);

#if CONFIG_I2C_INTERFACE
	ESP_LOGI(tag, "INTERFACE is i2c");
	ESP_LOGI(tag, "CONFIG_SDA_GPIO=%d",CONFIG_SDA_GPIO);
	ESP_LOGI(tag, "CONFIG_SCL_GPIO=%d",CONFIG_SCL_GPIO);
	ESP_LOGI(tag, "CONFIG_RESET_GPIO=%d",CONFIG_RESET_GPIO);
	i2c_master_init(&dev, CONFIG_SDA_GPIO, CONFIG_SCL_GPIO, CONFIG_RESET_GPIO);
#endif // CONFIG_I2C_INTERFACE

#if CONFIG_SSD1306_128x64
	ESP_LOGI(tag, "Panel is 128x64");
	ssd1306_init(&dev, 128, 64);
#endif // CONFIG_SSD1306_128x64

	//ADC config
    int val_y = 0, val_x = 0, val_button = 0;
    adc2_config_channel_atten( ADC2_CHANNEL_6, ADC_ATTEN_MAX );
    adc2_config_channel_atten( ADC2_CHANNEL_7, ADC_ATTEN_MAX );
    adc2_config_channel_atten( ADC2_CHANNEL_5, ADC_ATTEN_DB_0 );

	//display start
	ssd1306_clear_screen(&dev, false);
	ssd1306_contrast(&dev, 0xff);
	ssd1306_display_text(&dev, center, "  Press stick", 13, false);
	ssd1306_display_text(&dev, center+2, "    to start", 12, false);
	while(1){
        adc2_get_raw( ADC2_CHANNEL_5, ADC_WIDTH_12Bit, &val_button);
		if(val_button == 0){
			break;
		}
		vTaskDelay(100 / portTICK_PERIOD_MS);
	}

	
	//welcome message
	restart:
	ssd1306_clear_screen(&dev, false);
	ssd1306_contrast(&dev, 0xff);
	ssd1306_display_text(&dev, center, "    WELCOME", 11, false);
	int joystick_middle_x = 0, joystick_middle_y = 0;
	for(int i=0; i<10; i++){
        adc2_get_raw( ADC2_CHANNEL_7, ADC_WIDTH_12Bit, &val_x);
        adc2_get_raw( ADC2_CHANNEL_6, ADC_WIDTH_12Bit, &val_y);
		joystick_middle_x += val_x;
		joystick_middle_y += val_y;
		vTaskDelay(200 / portTICK_PERIOD_MS);
	}
	joystick_middle_x = joystick_middle_x/10;
	joystick_middle_y = joystick_middle_y/10;
	printf("Middle: %d|%d\n",joystick_middle_x, joystick_middle_y);

	//print
	ssd1306_clear_screen(&dev, false);
	ssd1306_contrast(&dev, 0xff);
	int bitmapWidth = 3*8;
	int bitmapHeight = 10;
	int xpos = 10; 
	int ypos = 64/2;
	ssd1306_bitmaps(&dev, xpos, ypos, spaceship, bitmapWidth, bitmapHeight, false);

	int x_deviation = joystick_middle_x*0.1;
	int y_deviation = joystick_middle_y*0.1;
	int max_val = 4000;
	int min_val = 100;
	//controling
	while(1) {
        ssd1306_clear_line(&dev, 0, false);
        adc2_get_raw( ADC2_CHANNEL_7, ADC_WIDTH_12Bit, &val_x);
        adc2_get_raw( ADC2_CHANNEL_6, ADC_WIDTH_12Bit, &val_y);
		//printf("Vals: %d|%d\n", val_x, val_y);
		//printf("Pos: %d|%d\n", xpos, ypos);

		adc2_get_raw( ADC2_CHANNEL_5, ADC_WIDTH_9Bit, &val_button);
		if(val_button == 0){
			printf("RESTARTED: %d\n", val_button);
			goto restart;
		}

		if(val_x < min_val && val_y < min_val){
			for(int i=0; i<4; i++){
				if(xpos > 10){
					xpos -= 1;
				}
				if(ypos > 10){
					ypos -= 1;
				}
				ssd1306_bitmaps(&dev, xpos, ypos, spaceship, bitmapWidth, bitmapHeight, false);
			}
		} else if(val_x > max_val && val_y > max_val){
			for(int i=0; i<4; i++){
				if(xpos < 117){
					xpos += 1;
				}
				if(ypos < 54){
					ypos += 1;
				}
				ssd1306_bitmaps(&dev, xpos, ypos, spaceship, bitmapWidth, bitmapHeight, false);
			}
		} else if(val_x > max_val && val_y < min_val){
			for(int i=0; i<4; i++){
				if(xpos < 117){
					xpos += 1;
				}
				if(ypos > 10){
					ypos -= 1;
				}
				ssd1306_bitmaps(&dev, xpos, ypos, spaceship, bitmapWidth, bitmapHeight, false);
			}
		} else if(val_x < min_val && val_y > max_val){
			for(int i=0; i<4; i++){
				if(xpos > 10){
					xpos -= 1;
				}
				if(ypos < 54){
					ypos += 1;
				}
				ssd1306_bitmaps(&dev, xpos, ypos, spaceship, bitmapWidth, bitmapHeight, false);
			}
		} else {
        
			if(val_x > max_val){
				for(int i=0; i<4; i++){
					if(xpos < 117){
						xpos += 1;
						ssd1306_bitmaps(&dev, xpos, ypos, spaceship, bitmapWidth, bitmapHeight, false);
					}
				}
			} else if(val_x > (joystick_middle_x + x_deviation)){
				for(int i=0; i<2; i++){
					if(xpos < 117){
						xpos += 1;
						ssd1306_bitmaps(&dev, xpos, ypos, spaceship, bitmapWidth, bitmapHeight, false);
					}
				}
			}
		
			if(val_x < min_val){
				for(int i=0; i<4; i++){
					if(xpos > 10){
						xpos -= 1;
						ssd1306_bitmaps(&dev, xpos, ypos, spaceship, bitmapWidth, bitmapHeight, false);
					}
				}
			} else if(val_x < (joystick_middle_x - x_deviation)){
				for(int i=0; i<2; i++){
					if(xpos > 10){
						xpos -= 1;
						ssd1306_bitmaps(&dev, xpos, ypos, spaceship, bitmapWidth, bitmapHeight, false);
					}
				}
			}

			if(val_y > max_val){
				for(int i=0; i<4; i++){
					if(ypos < 54){
						ypos += 1;
						ssd1306_bitmaps(&dev, xpos, ypos, spaceship, bitmapWidth, bitmapHeight, false);
					}
				}
			} else if(val_y > (joystick_middle_y+y_deviation)){
				for(int i=0; i<2; i++){
					if(ypos < 54){
						ypos += 1;
						ssd1306_bitmaps(&dev, xpos, ypos, spaceship, bitmapWidth, bitmapHeight, false);
					}
				}
			}
		
			if(val_y < min_val){
				for(int i=0; i<4; i++){
					if(ypos > 10){
						ypos -= 1;
						ssd1306_bitmaps(&dev, xpos, ypos, spaceship, bitmapWidth, bitmapHeight, false);
					}
				}
			} else if(val_y < (joystick_middle_y-y_deviation)){
				for(int i=0; i<42; i++){
					if(ypos > 10){
						ypos -= 1;
						ssd1306_bitmaps(&dev, xpos, ypos, spaceship, bitmapWidth, bitmapHeight, false);
					}
				}
			}
		}

		vTaskDelay(200 / portTICK_PERIOD_MS);
	}
	vTaskDelay(3000 / portTICK_PERIOD_MS);
    free(lineChar);
}