/*
 * led.c
 *
 *  Created on: Jun 24, 2017
 *      Author: Shirzad
 */

#include "PINCC26XX.h"
#include "./sablexr2_module/sablexr2_module_board.h"
#include "led.h"
#include "util.h"

// Clock instances for internal periodic events.
//static Clock_Struct ledBlinkClock;



void Led_init(void)
{
	  PINCC26XX_setOutputEnable(Board_GLED, 1);
	  PINCC26XX_setOutputValue(Board_GLED, LED_OFF);
	  PINCC26XX_setOutputEnable(Board_BLED, 1);
	  PINCC26XX_setOutputValue(Board_BLED, LED_OFF);
	  PINCC26XX_setOutputEnable(Board_RLED, 1);
	  PINCC26XX_setOutputValue(Board_RLED, LED_OFF);

	  PINCC26XX_setOutputEnable(Buzzer_SHDN_N, 1);
	  PINCC26XX_setOutputValue(Buzzer_SHDN_N, 1); // Power On
	  PINCC26XX_setOutputEnable(Buzzer_PWM, 1);
	  PINCC26XX_setOutputValue(Buzzer_PWM, 1);	  // 1 = Disable, 0 = Enable
}

void Toggle_led(void)
{
	uint8_t bVal = PINCC26XX_getOutputValue(Board_BLED);
	PINCC26XX_setOutputValue(Board_BLED, !bVal);
}

void BuzzerOnOff(bool state)
{
    if(state == false) PINCC26XX_setOutputValue(Buzzer_PWM, 1); // 1 = Disable, 0 = Enable
    else PINCC26XX_setOutputValue(Buzzer_PWM, 0); // 1 = Disable, 0 = Enable
}

/*
void Led_Flash(LED_State flash_type)
{

}
*/
