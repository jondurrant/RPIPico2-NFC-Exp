/**
 * Jon Durrant.
 *
 *  LED on GPIO 15 will follow Switch on GPIO 14
 */

#include "pico/stdlib.h"
#include <stdio.h>

#include "hardware/i2c.h"

#include <cstdlib>

extern "C"{
#include "ssd1306.h"
}

#define DELAY 200 // in microseconds

int main() {
	bool on;
    const uint LED_PIN =  15;
    uint8_t uid[80];
    uint8_t uidLen;
    uint8_t buffer[256]={0};
   // gpio_init(LED_PIN);
    ssd1306_t oled;

    //Initialise LED
   // gpio_set_dir(LED_PIN, GPIO_OUT);

    stdio_init_all();
    sleep_ms(2000);
    printf("GO\n");


    //Initialise SSD
    i2c_init(i2c1, 400 * 1000);
    gpio_set_function(2, GPIO_FUNC_I2C);
    gpio_set_function(3, GPIO_FUNC_I2C);
    gpio_pull_up(2);
    gpio_pull_up(3);
    ssd1306_init(
			&oled,
			128,
			64,
			0x3c,
			i2c1);
    //ssd1306_poweron(&oled);
    //ssd1306_contrast(&oled, 0xFF);
	ssd1306_clear(&oled);
	ssd1306_draw_square(
			&oled,
			10, 10,
			90, 50
			);
    ssd1306_draw_string(
			&oled,
			1,
			1,
			2,
			"Hi"
			);
    ssd1306_show(&oled);

    for (;;){
        //gpio_put(LED_PIN, on);
        on = !on;

        ssd1306_clear(&oled);
        if (on){
            ssd1306_draw_string(
        			&oled,
        			1,
        			1,
        			2,
        			"Hello"
        			);
            printf("Hello ");
        } else {
        	ssd1306_draw_string(
					&oled,
					1,
					1,
					2,
					"World"
					);
        	printf("World\n");
        }
        ssd1306_show(&oled);

        sleep_ms(DELAY);


    }

}
