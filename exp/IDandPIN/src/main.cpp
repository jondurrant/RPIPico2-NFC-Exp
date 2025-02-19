/**
 * Jon Durrant.
 *
 *  NFC ID Card Example
 *  NFC reader on I2C0 GP0 & GP1
 *  SSD1306 OLED on  I2C1 GP2 & GP3
 */

#include "pico/stdlib.h"
#include <stdio.h>

#include "hardware/i2c.h"

#include <cstdlib>
#include <cstring>

extern "C"{
#include <piconfc.h>
#include "ssd1306.h"
}

#include "Switch4x4.h"
const char lookup[17] = "123A456B789C*0#D";

#define DELAY 200 // in microseconds

enum StateType {
	TAG_WAIT,
	PIN_WAIT,
	ID_CHECK,
	ID_DISPLAY,
	ERROR_DISPLAY
};

typedef struct {
	uint32_t id;
	char pin[5];
	char name[8];
} AccessRecord;

#define ACCESS_LEN 2
const AccessRecord access[ACCESS_LEN] = {
		0xA758777A, "1234", "Jon",
		0x865F9425, "1472", "Bob"
};

int main() {
	bool on;
    const uint LED_PIN =  25;
    uint8_t uid[80];
    uint8_t uidLen;
    uint32_t id;
    uint8_t buffer[256]={0};
    gpio_init(LED_PIN);
    ssd1306_t oled;
    char pin[5];
    const char *name;
    StateType state = TAG_WAIT;
    Switch4x4 sw(
    		9, 8, 6, 7, //ROWS
			10,11,12,13 //COLS
    		);

    //Initialise LED
    gpio_set_dir(LED_PIN, GPIO_OUT);

    stdio_init_all();


    //NFC Setup
    PicoNFCConfig nfcConfig;
    if (!piconfc_init(
    		 &nfcConfig,
			  i2c0,
			  0,
			  1)){
    	printf("Failed to initialise NFC \n");
    }

    //Initialise SSD
    i2c_init(i2c1, 100 * 1000);
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
	ssd1306_clear(&oled);
    ssd1306_draw_string(
			&oled,
			1,
			1,
			2,
			"START UP"
			);
    ssd1306_show(&oled);
    sleep_ms(3000);
    printf("GO\n");



    for (;;){
        gpio_put(LED_PIN, on);
        on = !on;

        //WAIT FOR A TAG
        if (state == TAG_WAIT){
        	ssd1306_clear(&oled);
			ssd1306_draw_string(
					&oled,
					1,
					1,
					3,
					"TAG"
					);
			ssd1306_show(&oled);
			uidLen = 80;
			if ( piconfc_PN532_readPassiveTargetID(
					 &nfcConfig,
					 0x00,
					 uid,
					 &uidLen,  DELAY)){

				if (uidLen == 4){
					printf("Tag read\n UID(%d):", uidLen);
					id = 0x00;
					for (uint8_t i=0; i < uidLen; i++){
						printf("%02X ", uid[i]);
						id = id << 8 | uid[i];
					}
					printf("\n 0x%X\n", id);

					state = PIN_WAIT;
					sw.clear();
					strcpy(pin, "");
				} else {
					printf("Tag too long an id (%d) \n", uidLen);
					state = ERROR_DISPLAY;
				}
			}
        }

        //WAIT FOR A PIN
        if (state == PIN_WAIT){
        	if (sw.isAvailable()){
        		int8_t swNum = sw.readInt();
        		int i = strlen(pin);
        		pin[i] = lookup[swNum];
        		pin[i+1] = '\0';
        	}

        	ssd1306_clear(&oled);
			ssd1306_draw_string(
				&oled,
				1,
				1,
				3,
				"PIN"
				);
			ssd1306_draw_string(
				&oled,
				1,
				25,
				3,
				pin
				);
			ssd1306_show(&oled);

			if (strlen(pin) >= 4){
				state=ID_CHECK;
			}
        }

        //CHECK if PIN matched ID
        if (state == ID_CHECK){
        	for (int i=0; i < ACCESS_LEN; i++){
        		if (id == access[i].id){
        			printf("Found record for 0x%X\n", id);
        			if (strcmp(access[i].pin, pin) == 0){
        				state = ID_DISPLAY;
        				name = access[i].name;
        				printf("PIN Match for record for 0x%X and %s\n", id, name);
        			} //Pin check
        		} // ID match
        	} // for

        	if (	state != ID_DISPLAY){
        		state = ERROR_DISPLAY;
        	}
        }

        //DISPLAY Welcome for Access
        if (state == ID_DISPLAY){
        	ssd1306_clear(&oled);
			ssd1306_draw_string(
				&oled,
				1,
				1,
				3,
				"Hello"
				);
			ssd1306_draw_string(
				&oled,
				1,
				25,
				3,
				name
				);
			ssd1306_show(&oled);
			sleep_ms(3000);
			state = TAG_WAIT;
        }

        //DISPLAY UNKNOWN
        if (state == 	ERROR_DISPLAY){
        	ssd1306_clear(&oled);
			ssd1306_draw_string(
				&oled,
				1,
				1,
				3,
				"UNKNOWN"
				);
			ssd1306_show(&oled);
			sleep_ms(3000);
			state = TAG_WAIT;
        }

        ssd1306_show(&oled);
        sleep_ms(DELAY);


    }

}
