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
    uint8_t buffer[256]={0};
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
        	printf("\n");

        	enum NTAG21X tagType = piconfc_NTAG_getModel( &nfcConfig);
        	printf("Tag Type: 0x%X\n", tagType);

        	if (piconfc_NTAG_read1Page(
        			&nfcConfig,
        			0X01,
					buffer)){
        		printf("Read buffer:");
        		for (uint8_t i=0; i < 16; i++){
					printf("%02X ", buffer[i]);
				}
        		printf("\n");
        	} else {
        		printf("Read page failed\n");
        	}

        	uint8_t block_number = 1;
        	uint8_t rlen;
#define MIFARE_CMD_READ                     (0x30)
        	uint8_t cmdbuf[] = {MIFARE_CMD_READ, block_number & 0xFF};
        	bool success = piconfc_PN532_initiatorDataExchange(
        			&nfcConfig,
					cmdbuf,
					sizeof(cmdbuf),
					buffer,
					&rlen,
					16);
        	if (success){
        		printf("MIFARE READ(%d): ", rlen);
        		for (int i=0; i < rlen; i++){
        			printf("%02X ", buffer[i]);
        		}
        		printf("\n");
        	} else {
        		printf("MIFARE Failes\n");
        	}

        } else {
        	//printf("No Tag\n");
        }


    }

}
