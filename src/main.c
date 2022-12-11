#include "main.h"

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
	ssd1306_display_text(&dev, center, " STARTING GAME...", 17, false);
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

	//print
	ssd1306_clear_screen(&dev, false);
	ssd1306_contrast(&dev, 0xff);
	unsigned int bitmapWidth = 3*8;
	unsigned int bitmapHeight = 10;
	unsigned int xpos = 0; 
	unsigned int ypos = 64/2;
	ssd1306_bitmaps(&dev, xpos, ypos, spaceship, bitmapWidth, bitmapHeight, false);

	unsigned int x_deviation = joystick_middle_x*0.1;
	unsigned int y_deviation = joystick_middle_y*0.1;
	unsigned int max_val = 4000;
	unsigned int min_val = 100;
	//controling

	fires_t *all_shots = malloc(sizeof(fires_t)*13);
	unsigned int number_of_shots = 1;

	meteor_t meteor_1;
	meteor_t meteor_2;
	meteor_t meteor_3;
	meteor_t meteor_4;
	meteor_t meteor_5;

	bool meteor_1_active = true;
	bool meteor_2_active = true;
	bool meteor_3_active = true;
	bool meteor_4_active = true;
	bool meteor_5_active = true;

	meteor_1.xpos = 103;
	meteor_1.ypos = 15;
	ssd1306_bitmaps(&dev, meteor_1.xpos, meteor_1.ypos, meteor, 8, 3, false);

	meteor_2.xpos = 111;
	meteor_2.ypos = 25;
	ssd1306_bitmaps(&dev, meteor_2.xpos, meteor_2.ypos, meteor, 8, 3, false);

	meteor_3.xpos = 103;
	meteor_3.ypos = 35;
	ssd1306_bitmaps(&dev, meteor_3.xpos, meteor_3.ypos, meteor, 8, 3, false);

	meteor_4.xpos = 111;
	meteor_4.ypos = 45;
	ssd1306_bitmaps(&dev, meteor_4.xpos, meteor_4.ypos, meteor, 8, 3, false);

	meteor_5.xpos = 103;
	meteor_5.ypos = 55;
	ssd1306_bitmaps(&dev, meteor_5.xpos, meteor_5.ypos, meteor, 8, 3, false);

	while(1) {
        ssd1306_clear_line(&dev, 0, false);
        adc2_get_raw( ADC2_CHANNEL_7, ADC_WIDTH_12Bit, &val_x);
        adc2_get_raw( ADC2_CHANNEL_6, ADC_WIDTH_12Bit, &val_y);
		adc2_get_raw( ADC2_CHANNEL_5, ADC_WIDTH_9Bit, &val_button);

		if(val_button == 0){
			goto restart;
		}

		if(val_x < min_val && val_y < min_val){
			for(int i=0; i<4; i++){
				if(xpos > 0){
					xpos -= 1;
				}
				if(ypos > 10){
					ypos -= 1;
				}
				ssd1306_bitmaps(&dev, xpos, ypos, spaceship, bitmapWidth, bitmapHeight, false);
			}
		} else if(val_x > max_val && val_y > max_val){
			for(int i=0; i<4; i++){
				if(xpos < 103){
					xpos += 1;
				}
				if(ypos < 54){
					ypos += 1;
				}
				ssd1306_bitmaps(&dev, xpos, ypos, spaceship, bitmapWidth, bitmapHeight, false);
			}
		} else if(val_x > max_val && val_y < min_val){
			for(int i=0; i<4; i++){
				if(xpos < 103){
					xpos += 1;
				}
				if(ypos > 10){
					ypos -= 1;
				}
				ssd1306_bitmaps(&dev, xpos, ypos, spaceship, bitmapWidth, bitmapHeight, false);
			}
		} else if(val_x < min_val && val_y > max_val){
			for(int i=0; i<4; i++){
				if(xpos > 0){
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
					if(xpos < 103){
						xpos += 1;
						ssd1306_bitmaps(&dev, xpos, ypos, spaceship, bitmapWidth, bitmapHeight, false);
					}
				}
			} else if(val_x > (joystick_middle_x + x_deviation)){
				for(int i=0; i<2; i++){
					if(xpos < 103){
						xpos += 1;
						ssd1306_bitmaps(&dev, xpos, ypos, spaceship, bitmapWidth, bitmapHeight, false);
					}
				}
			}
		
			if(val_x < min_val){
				for(int i=0; i<4; i++){
					if(xpos > 0){
						xpos -= 1;
						ssd1306_bitmaps(&dev, xpos, ypos, spaceship, bitmapWidth, bitmapHeight, false);
					}
				}
			} else if(val_x < (joystick_middle_x - x_deviation)){
				for(int i=0; i<2; i++){
					if(xpos > 0){
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
				for(int i=0; i<2; i++){
					if(ypos > 10){
						ypos -= 1;
						ssd1306_bitmaps(&dev, xpos, ypos, spaceship, bitmapWidth, bitmapHeight, false);
					}
				}
			}
		}
		
		if(number_of_shots == 10){
			number_of_shots = 1;
		}
		fires_t new_shot;
		new_shot.xpos = xpos + bitmapWidth + 2;
		new_shot.ypos = ypos + (bitmapHeight/2);
		ssd1306_bitmaps(&dev, all_shots[number_of_shots-1].xpos, all_shots[number_of_shots-1].ypos, clear_fire, 8, 1, false);
		all_shots[number_of_shots-1] = new_shot;
		number_of_shots++;
		for(int i=0; i<10; i++){
			ssd1306_bitmaps(&dev, all_shots[i].xpos, all_shots[i].ypos, clear_fire, 8, 1, false);
			if(all_shots[i].xpos < 111){
				all_shots[i].xpos += 8;
				ssd1306_bitmaps(&dev, all_shots[i].xpos, all_shots[i].ypos, fire, 8, 1, false);
			}
			if(all_shots[i].xpos >= 95 && all_shots[i].xpos < 127){

				if(meteor_1_active && all_shots[i].ypos > 12 && all_shots[i].ypos < 18){
					meteor_1_active = false;
					ssd1306_bitmaps(&dev, meteor_1.xpos, meteor_1.ypos, clear_meteor, 8, 3, false);
				} else if(meteor_2_active && all_shots[i].ypos > 22 && all_shots[i].ypos < 28){
					meteor_2_active = false;
					ssd1306_bitmaps(&dev, meteor_2.xpos, meteor_2.ypos, clear_meteor, 8, 3, false);
				} else if(meteor_3_active && all_shots[i].ypos > 32 && all_shots[i].ypos < 38){
					meteor_3_active = false;
					ssd1306_bitmaps(&dev, meteor_3.xpos, meteor_3.ypos, clear_meteor, 8, 3, false);
				} else if(meteor_4_active && all_shots[i].ypos > 42 && all_shots[i].ypos < 48){
					meteor_4_active = false;
					ssd1306_bitmaps(&dev, meteor_4.xpos, meteor_4.ypos, clear_meteor, 8, 3, false);
				} else if(meteor_5_active && all_shots[i].ypos > 52 && all_shots[i].ypos < 58){
					meteor_5_active = false;
					ssd1306_bitmaps(&dev, meteor_5.xpos, meteor_5.ypos, clear_meteor, 8, 3, false);
				}
			}
		}
		if(!meteor_1_active && !meteor_2_active && !meteor_3_active && !meteor_4_active && !meteor_5_active){
			break;
		}
	}
	
	ssd1306_clear_screen(&dev, false);
	ssd1306_contrast(&dev, 0xff);
	ssd1306_display_text(&dev, center, "    YOU WON!", 12, false);

	vTaskDelay(5000 / portTICK_PERIOD_MS);
	esp_restart();

	free(all_shots);
    free(lineChar);
}