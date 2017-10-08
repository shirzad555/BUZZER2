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

enum LED_STATE {
    LED_STATE_OFF 	    = 0x00,
	LED_STATE_ON 		= 0x01,
	LED_STATE_FLASH_1   = 0x02,
	LED_STATE_FLASH_2   = 0x04,
	LED_STATE_FLASH_3   = 0x08,
	LED_STATE_ERROR     = 0xFF
} ;

enum ALARM_SENSITIVITY {
    ALARM_SENS_LOWEST       = 0x00,
    ALARM_SENS_LOW          = 0x01,
    ALARM_SENS_MID          = 0x02,
    ALARM_SENS_HIGH         = 0x04,
    ALARM_SENS_HIGHEST      = 0x08,
    ALARM_SENS_ERROR        = 0xFF
} ;

enum ALARM_STATE {
    ALARM_STATE_OFF         = 0x00,
    ALARM_STATE_BUZ         = 0x01,
    ALARM_STATE_LED         = 0x02,
    ALARM_STATE_MSG         = 0x04,
    ALARM_STATE_BUZ_LED     = 0x08,
    ALARM_STATE_BUZ_MSG     = 0x09,
    ALARM_STATE_LED_MSG     = 0x09,
    ALARM_STATE_BUZ_LED_MSG = 0x0A,
    ALARM_STATE_ERROR       = 0xFF
} ;

enum MOVEMENT_MSG {
    MOVEMENT_MSG_NONE       = 0x00,
    MOVEMENT_MSG_LOW        = 0x01,
    MOVEMENT_MSG_HIGH       = 0x02,
    MOVEMENT_MSG_ERROR      = 0xFF
} ;

void Led_init(void);
void Toggle_led(void);
//void Led_Flash(LED_State flash_type);
void BuzzerOnOff(bool state);


#endif /* APPLICATION_LED_H_ */
