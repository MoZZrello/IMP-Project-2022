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

void app_main(void)
{
	SSD1306_t dev;
	int center = 3, top = 2, bottom = 8;
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

	ssd1306_clear_screen(&dev, false);
	ssd1306_contrast(&dev, 0xff);
    ssd1306_display_text_x3(&dev, 0, "Hello", 5, false);
	vTaskDelay(3000 / portTICK_PERIOD_MS);

    int val_y, val_x;
    adc2_config_channel_atten( ADC2_CHANNEL_6, ADC_ATTEN_MAX );
    adc2_config_channel_atten( ADC2_CHANNEL_7, ADC_ATTEN_MAX );
	
	// Scroll Down
	ssd1306_clear_screen(&dev, false);
	while(1) {
        ssd1306_clear_line(&dev, 0, false);
        adc2_get_raw( ADC2_CHANNEL_7, ADC_WIDTH_12Bit, &val_x);
        adc2_get_raw( ADC2_CHANNEL_6, ADC_WIDTH_12Bit, &val_y);
        //int val_button = adc1_get_raw(ADC1_CHANNEL_0);
		sprintf(lineChar, "%d|%d", val_x, val_y);   
        lineChar[strlen(lineChar)] = '\0';
        ssd1306_display_text(&dev, 0, lineChar, strlen(lineChar), false);
		vTaskDelay(250 / portTICK_PERIOD_MS);
	}
	vTaskDelay(3000 / portTICK_PERIOD_MS);
    free(lineChar);
}