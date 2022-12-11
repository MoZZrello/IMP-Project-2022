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
		0b11000000, 0b00000000, 0b00000000,
		0b11110000, 0b11000000, 0b00000000,
		0b11111111, 0b11110000, 0b00110000,
		0b11111111, 0b11111111, 0b11110000,
		0b11111111, 0b11111111, 0b11110000,
		0b11111111, 0b11110000, 0b00110000,
		0b11110000, 0b11000000, 0b00000000,
		0b11000000, 0b00000000, 0b00000000
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

	int joystick_middle_x = 0, joystick_middle_y = 0;
	//welcome message
	ssd1306_clear_screen(&dev, false);
	ssd1306_contrast(&dev, 0xff);
	ssd1306_display_text(&dev, center, "    WELCOME", 11, false);
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

	/*ssd1306_clear_screen(&dev, false);
	ssd1306_contrast(&dev, 0xff);
	int bitmapWidth = 4*8;
	int width = ssd1306_get_width(&dev);
	int xpos = width / 2; // center of width
	xpos = xpos - bitmapWidth/2; 
	int ypos = 16;
	ESP_LOGD(tag, "width=%d xpos=%d", width, xpos);
	ssd1306_bitmaps(&dev, xpos, ypos, spaceship, 24, 8, false);
	vTaskDelay(3000 / portTICK_PERIOD_MS);
	while(1){
		for(int i=0;i<8;i++) {
			ssd1306_wrap_arround(&dev, SCROLL_RIGHT, 2, 3, 0);
		}

		for(int i=0;i<8; i++) {
			ssd1306_wrap_arround(&dev, SCROLL_UP, 52, 77, 0);
		}

		for(int i=0;i<8;i++) {
			ssd1306_wrap_arround(&dev, SCROLL_LEFT, 1, 2, 0);
		}

		for(int i=0;i<8;i++) {
			ssd1306_wrap_arround(&dev, SCROLL_DOWN, 52-8, 77-8, 0);
		}
	}	
	vTaskDelay(3000 / portTICK_PERIOD_MS);*/

	//print
	restart:
	ssd1306_clear_screen(&dev, false);
	ssd1306_contrast(&dev, 0xff);
	int bitmapWidth = 3*8;
	int width = ssd1306_get_width(&dev);
	int xpos = 10; 
	int ypos = (64/2)-(bitmapWidth/2);
	int page = 2;
	ssd1306_bitmaps(&dev, xpos, ypos, spaceship, 24, 8, false);

	int x_deviation = joystick_middle_x*0.05;
	int y_deviation = joystick_middle_y*0.05;
	int max_val = 4000;
	int min_val = 100;
	//controling
	while(1) {
        ssd1306_clear_line(&dev, 0, false);
        adc2_get_raw( ADC2_CHANNEL_7, ADC_WIDTH_12Bit, &val_x);
        adc2_get_raw( ADC2_CHANNEL_6, ADC_WIDTH_12Bit, &val_y);
		printf("Vals: %d|%d\n", val_x, val_y);

		adc2_get_raw( ADC2_CHANNEL_5, ADC_WIDTH_12Bit, &val_button);
		if(val_button == 0){
			goto restart;
		}
        
		if(val_x > max_val){
			for(int i=0;i<8;i++) {
				ssd1306_wrap_arround(&dev, SCROLL_RIGHT, page, page+1, 0);
			}
			xpos += 8;
		} else if(val_x > (joystick_middle_x + x_deviation)){
			for(int i=0;i<2;i++) {
				ssd1306_wrap_arround(&dev, SCROLL_RIGHT, page, page+1, 0);
			}
			xpos += 2;
		}
		
		if(val_x < min_val){
			for(int i=0;i<8;i++) {
				ssd1306_wrap_arround(&dev, SCROLL_LEFT, page, page+1, 0);
			}
			xpos -= 8;
		} else if(val_x < (joystick_middle_x - x_deviation)){
			for(int i=0;i<2;i++) {
				ssd1306_wrap_arround(&dev, SCROLL_LEFT, page, page+1, 0);
			}
			xpos -= 2;
		}

		if(val_y > (joystick_middle_y+y_deviation)){
			for(int i=0;i<8; i++) {
				ssd1306_wrap_arround(&dev, SCROLL_UP, xpos, xpos+bitmapWidth, 0);
			}
			page -= 1;
		}
		if(page == -1){
			page = 6;
		}
		
		if(val_y < (joystick_middle_y-y_deviation)){
			for(int i=0;i<8;i++) {
				ssd1306_wrap_arround(&dev, SCROLL_DOWN, xpos, xpos+bitmapWidth, 0);
			}
			page += 1;
		}
		if(page == 8){
			page = 0;
		}

		vTaskDelay(200 / portTICK_PERIOD_MS);
	}
	vTaskDelay(3000 / portTICK_PERIOD_MS);
    free(lineChar);
}