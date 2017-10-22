/*
 * alarm.c
 *
 *  Created on: Jun 25, 2017
 *      Author: Shirzad
 */

#include <stdint.h>
#include "bcomdef.h"
#include "comdef.h"
#include "alarm.h"

#include "console.h"
#include "SABLEXR2_DEV_BOARD.h"
#include "LIS3DH_Driver.h"
#include "PINCC26XX.h"

// Alarm Setting var
static uint8_t alarm_current_setting = ALARM_OFF;
static uint16_t static_xyzValue[3][SENSOR_MOVE_COUNT];

/*
 // Initialize accelerometer
 */
void InitMovementSensor(void)
{
    ////////////////////////////////// LIS3DH /////////////////////////////////////////////////////

    if (!LIS3DH_Initialize())
    {
        RPrintf(" LIS3DH failed to init!!!\r\n");
    }

    LIS3DH_SetDeviceMode(LIS3DH_MODE_LOW_POWER, LIS3DH_ODR_25_HZ, LIS3DH_FULL_SCALE_SELECTION_4G);
    RPrintf(" LIS3DH is Good!\r\n");
}

/*
 // Configure the accelerometer to interrupt when moved based on threshold and duration as inputs
 */
void SensorConfiguration (uint8_t threshold, uint8_t duration)
{
    uint8_t bVal;
    LIS3DH_Filter filter_Parms;

    filter_Parms.highPassFilterIntEnable = LIS3DH_HF_FILTER_INT_AI1; //LIS3DH_HF_FILTER_INT_NONE; //LIS3DH_HF_FILTER_INT_AI1;
    filter_Parms.highPassFilterDataSel = LIS3DH_HF_FILTER_DATA_SEL_OUT;
    filter_Parms.highPassFilterMode = LIS3DH_HF_FILTER_MODE_NORMAL_RESET;
    filter_Parms.highPassFilterCutOffFreq = LIS3DH_HF_FILTER_CUTOFF_FREQ_3;

  /////////////////////// THIS IS TO SETUP THE HIGH PASS FILTER INTERRUPT /////////////////////////////////////////////////////////////////////////////////
    LIS3DH_SetFilter(filter_Parms);
    LIS3DH_InterruptCtrl();
    LIS3DH_Interrupt1Threshold(threshold); //0x05 // Best way to adjust sensitivity
    LIS3DH_Interrupt1Duration(duration); // 2 // you can add duration here too
    LIS3DH_ReadRefrence(&bVal); // Dummy read to force the HP filter to current acceleration value  (i.e. set reference acceleration/tilt value)
    LIS3DH_Interrupt1Config(0x2A); // Configure desired wake-up event (AOI 6D ZHIE ZLIE YHIE YLIE XHIE XLIE)
}

/*
 // Disable accelerometer's interrupt
 */
void DisableAccelerometerIntterupt (void)
{
    LIS3DH_Filter filter_Parms;
    uint8_t bVal;

    // DISABLE INTERRUPT /////////////////////////
    filter_Parms.highPassFilterIntEnable = LIS3DH_HF_FILTER_INT_NONE; //LIS3DH_HF_FILTER_INT_NONE; //LIS3DH_HF_FILTER_INT_AI1;
    filter_Parms.highPassFilterDataSel = LIS3DH_HF_FILTER_DATA_SEL_BYPASS;
    filter_Parms.highPassFilterMode = LIS3DH_HF_FILTER_MODE_NORMAL_RESET;
    filter_Parms.highPassFilterCutOffFreq = LIS3DH_HF_FILTER_CUTOFF_FREQ_3;
    LIS3DH_SetFilter(filter_Parms);
    LIS3DH_Interrupt1Config(0x00); // Disable Interrupt // Configure desired wake-up event (AOI 6D ZHIE ZLIE YHIE YLIE XHIE XLIE)
    //////////////////////////////////////////////

    LIS3DH_ReadINT1Source(&bVal); // Return the event that has triggered the interrupt and clear interrupt
}

/*
/////// Read XYZ and see if the detected movement from HPF is real (not noise) //////////////
 */
void ReadSensorValue(uint8_t counter)
{
    uint16_t xValue;
    uint16_t yValue;
    uint16_t zValue;

    LIS3DH_ReadDeviceValue(&xValue, &yValue, &zValue); // clear the interrupt

    // math below will change the xyz values from 2s complement to 0x00 (-2g) to 0xFF (2g)
    xValue = xValue & 0xFF; // in 8-bit mode sometimes i have seen 0x100 which is not correct, mask to remove the bug
    if (xValue > 0x7F) xValue = xValue & 0x7F;
    else xValue = xValue | 0x80;

    yValue = yValue & 0xFF; // in 8-bit mode sometimes i have seen 0x100 which is not correct, mask to remove the bug
    if (yValue > 0x7F) yValue = yValue & 0x7F;
    else yValue = yValue | 0x80;

    zValue = zValue & 0xFF; // in 8-bit mode sometimes i have seen 0x100 which is not correct, mask to remove the bug
    if (zValue > 0x7F) zValue = zValue & 0x7F;
    else zValue = zValue | 0x80;
//  Log_print3(Diags_USER1, "XYZ_1 = %x, %d, %d", xValue, yValue, zValue);

    static_xyzValue[0][counter] = xValue;
    static_xyzValue[1][counter] = yValue;
    static_xyzValue[2][counter] = zValue;
}

/*
/////// Analyze the array of X,Y and Z and determine of movement happened (or it was just noise)  //////////////
 */
uint8_t CheckForMovement(void)
{
    uint8_t i, j;
    uint8_t bVal;
    //LIS3DH_Filter filter_Parms;
    uint8_t lastxyzValue[3] = {static_xyzValue[0][0], static_xyzValue[1][0], static_xyzValue[2][0]};
    uint8_t maxDiff = 0;

    for (i=1; i<SENSOR_MOVE_COUNT; i++)
    {
        for (j=0; j<3; j++)
        {
            if(static_xyzValue[j][i] > lastxyzValue[j]) bVal = static_xyzValue[j][i] - lastxyzValue[j];
            else bVal = lastxyzValue[j] - static_xyzValue[j][i];
            if (bVal > maxDiff)
            {
                maxDiff = bVal;
                lastxyzValue[j] = static_xyzValue[j][i];
//                Log_print1(Diags_USER1, "maxDiff = %d", maxDiff);
            }
        }
//        Log_print4(Diags_USER1, "s_XYZ_%d = %d, %d, %d", i, static_xyzValue[0][i], static_xyzValue[1][i], static_xyzValue[2][i]);
    }

    if (maxDiff > SENSOR_MOVEMENT_THRESHOLD)
    {
        //bVal = PINCC26XX_getOutputValue(Board_RLED);
        //PINCC26XX_setOutputValue(Board_RLED, !bVal);
        //RPrintf("ALARM_MOVEMENT_THRESHOLD\r\n");
        bVal = 1;
    }
    else
    {
        bVal = 0;
    }

    SensorConfiguration (0x05, 2); // Enable sensor's interrupt again

    return bVal;
}

/*********************************************************************
 */
bStatus_t Alarm_SetSetting( uint8_t alarm_setting )
{
    RPrintf("Alarm State = %d\r\n", alarm_setting);
  if (alarm_setting <= ALARM_ON_SEN_5)
  {
	  alarm_current_setting = alarm_setting;
	  return SUCCESS;
  }
  else
  {
	  alarm_current_setting = ALARM_ERROR;
	  return INVALIDPARAMETER;
  }
}

/*********************************************************************
 */
uint8_t  Alarm_GetSetting (void)
{
  return alarm_current_setting;
}

void Start_Alarm (void)
{

}
