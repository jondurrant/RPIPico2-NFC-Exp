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
#include <piconfc.h>
}

#define DELAY 500 // in microseconds

int main() {
	bool on;
    const uint LED_PIN =  15;
    uint8_t uid[80];
    uint8_t uidLen;
    gpio_init(LED_PIN);

    gpio_set_dir(LED_PIN, GPIO_OUT);


    stdio_init_all();
    sleep_ms(2000);
    printf("GO\n");


    PicoNFCConfig nfcConfig;
    if (!piconfc_init(
    		 &nfcConfig,
			  i2c0,
			  0,
			  1)){
    	printf("Failed to initialise NFC \n");
    }

    for (;;){
        gpio_put(LED_PIN, on);
        on = !on;

        uidLen = 80;
        if ( piconfc_PN532_readPassiveTargetID(
        		 &nfcConfig,
				 0x00,
				 uid,
				 &uidLen,  DELAY)){

        	printf("Tag read\n UID(%d):", uidLen);
        	for (uint8_t i=0; i < uidLen; i++){
        		printf("%02X ", uid[i]);
        	}

        } else {
        	//printf("No Tag\n");
        }


    }

}
