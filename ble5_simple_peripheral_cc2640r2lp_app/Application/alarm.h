/*
 * alarm.h
 *
 *  Created on: Jun 25, 2017
 *      Author: Shirzad
 */

#ifndef APPLICATION_ALARM_H_
#define APPLICATION_ALARM_H_

#define SENSOR_MOVE_COUNT                       10
#define SENSOR_MOVEMENT_THRESHOLD                10

enum Alarm_Cmd {
    ALARM_OFF 	    	= 0x00,
	ALARM_ON_SEN_1 		= 0x01,
	ALARM_ON_SEN_2   	= 0x02,
	ALARM_ON_SEN_3   	= 0x03,
	ALARM_ON_SEN_4   	= 0x04,
	ALARM_ON_SEN_5     	= 0x05,
	ALARM_ERROR			= 0xFF
};

enum ALARM_SENSITIVITY {
    ALARM_SENS_LOWEST       = 0x00,
    ALARM_SENS_LOW          = 0x01,
    ALARM_SENS_MID          = 0x02,
    ALARM_SENS_HIGH         = 0x03,
    ALARM_SENS_HIGHEST      = 0x04,
    ALARM_SENS_ERROR        = 0xFF
} ;

enum ALARM_STATE {
    ALARM_STATE_OFF         = 0x00,
    ALARM_STATE_BUZ         = 0x01,
    ALARM_STATE_LED         = 0x02,
    ALARM_STATE_MSG         = 0x03,
    ALARM_STATE_BUZ_LED     = 0x04,
    ALARM_STATE_BUZ_MSG     = 0x05,
    ALARM_STATE_LED_MSG     = 0x06,
    ALARM_STATE_BUZ_LED_MSG = 0x07,
    ALARM_STATE_ERROR       = 0xFF
} ;

enum MOVEMENT_MSG {
    MOVEMENT_MSG_NONE       = 0x00,
    MOVEMENT_MSG_LOW        = 0x01,
    MOVEMENT_MSG_HIGH       = 0x02,
    MOVEMENT_MSG_ERROR      = 0xFF
} ;


void InitMovementSensor(void);
void ReadSensorValue(uint8_t counter);
uint8_t CheckForMovement(void);
void DisableAccelerometerIntterupt (void);

void EnableAccelerometerIntterupt (uint8_t threshold, uint8_t duration);

bStatus_t Alarm_SetSetting( uint8_t alarm_setting );
uint8_t  Alarm_GetSetting (void);
void Start_Alarm (void);



#endif /* APPLICATION_ALARM_H_ */
