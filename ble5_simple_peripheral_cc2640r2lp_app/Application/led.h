/*
 * led.h
 *
 *  Created on: Jun 24, 2017
 *      Author: Shirzad
 */

#ifndef APPLICATION_LED_H_
#define APPLICATION_LED_H_


#define LED_ON          	Board_LED_ON
#define LED_OFF         	Board_LED_OFF

#define LED_BLINK_COUNT_1 	10

enum LED_State {
    LED_STATE_OFF 	    = 0x00,
	LED_STATE_ON 		= 0x01,
	LED_STATE_FLASH_1   = 0x02,
	LED_STATE_FLASH_2   = 0x04,
	LED_STATE_FLASH_3   = 0x08,
	LED_STATE_ERROR     = 0xFF
} ;


void Led_init(void);
void Toggle_led(void);
//void Led_Flash(LED_State flash_type);


#endif /* APPLICATION_LED_H_ */
