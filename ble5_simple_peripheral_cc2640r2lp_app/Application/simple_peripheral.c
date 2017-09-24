/******************************************************************************

 @file       simple_peripheral.c

 @brief This file contains the Simple Peripheral sample application for use
        with the CC2650 Bluetooth Low Energy Protocol Stack.

 Group: CMCU, SCS
 Target Device: CC2640R2

 ******************************************************************************
 
 Copyright (c) 2013-2017, Texas Instruments Incorporated
 All rights reserved.

 Redistribution and use in source and binary forms, with or without
 modification, are permitted provided that the following conditions
 are met:

 *  Redistributions of source code must retain the above copyright
    notice, this list of conditions and the following disclaimer.

 *  Redistributions in binary form must reproduce the above copyright
    notice, this list of conditions and the following disclaimer in the
    documentation and/or other materials provided with the distribution.

 *  Neither the name of Texas Instruments Incorporated nor the names of
    its contributors may be used to endorse or promote products derived
    from this software without specific prior written permission.

 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

 ******************************************************************************
 Release Name: simplelink_cc2640r2_sdk_1_40_00_45
 Release Date: 2017-07-20 17:16:59
 *****************************************************************************/

/*********************************************************************
 * INCLUDES
 */
#include <string.h>

#include <ti/sysbios/knl/Task.h>
#include <ti/sysbios/knl/Clock.h>
#include <ti/sysbios/knl/Event.h>
#include <ti/sysbios/knl/Queue.h>
#include <ti/display/Display.h>
#include <ti/sysbios/knl/Swi.h>
#include <ti/sysbios/family/arm/m3/Hwi.h>

#if defined( USE_FPGA ) || defined( DEBUG_SW_TRACE )
#include <driverlib/ioc.h>
#endif // USE_FPGA | DEBUG_SW_TRACE

#include <icall.h>
#include "util.h"
/* This Header file contains all BLE API and icall structure definition */
#include "icall_ble_api.h"

#include "devinfoservice.h"
#include "simple_gatt_profile.h"

#if defined(FEATURE_OAD) || defined(IMAGE_INVALIDATE)
#include "oad_target.h"
#include "oad.h"
#endif //FEATURE_OAD || IMAGE_INVALIDATE

#include "peripheral.h"

#ifdef USE_RCOSC
#include "rcosc_calibration.h"
#endif //USE_RCOSC


#include "board.h"
#include "./sablexr2_module/sablexr2_module_board.h"

#if !defined(Display_DISABLE_ALL)
#include "board_key.h"
#include <menu/two_btn_menu.h>

#include "simple_peripheral_menu.h"
#endif  // !Display_DISABLE_ALL

#include "simple_peripheral.h"

#include "LIS3DH_Driver.h"
#include "led.h"
#include "PINCC26XX.h"
#include "alarm.h"

#include <ti/drivers/UART.h>
#include <ti/drivers/uart/UARTCC26XX.h>

#if defined (NPI_USE_UART) || defined (NPI_USE_SPI)
#include "tl.h"
#endif //TL

#include "console.h"
/*********************************************************************
 * CONSTANTS
 */

// Advertising interval when device is discoverable (units of 625us, 160=100ms)
#define DEFAULT_ADVERTISING_INTERVAL          160

// General discoverable mode: advertise indefinitely
#define DEFAULT_DISCOVERABLE_MODE             GAP_ADTYPE_FLAGS_GENERAL

#ifndef FEATURE_OAD
// Minimum connection interval (units of 1.25ms, 80=100ms) for automatic
// parameter update request
#define DEFAULT_DESIRED_MIN_CONN_INTERVAL     80

// Maximum connection interval (units of 1.25ms, 800=1000ms) for automatic
// parameter update request
#define DEFAULT_DESIRED_MAX_CONN_INTERVAL     800

#else // FEATURE_OAD
// Increase the the connection interval to allow for higher throughput for OAD

// Minimum connection interval (units of 1.25ms, 8=10ms) for automatic
// parameter update request
#define DEFAULT_DESIRED_MIN_CONN_INTERVAL     8

// Maximum connection interval (units of 1.25ms, 8=10ms) for automatic
// parameter update request
#define DEFAULT_DESIRED_MAX_CONN_INTERVAL     8
#endif // FEATURE_OAD

// Slave latency to use for automatic parameter update request
#define DEFAULT_DESIRED_SLAVE_LATENCY         0

// Supervision timeout value (units of 10ms, 1000=10s) for automatic parameter
// update request
#define DEFAULT_DESIRED_CONN_TIMEOUT          1000

// After the connection is formed, the peripheral waits until the central
// device asks for its preferred connection parameters
#define DEFAULT_ENABLE_UPDATE_REQUEST         GAPROLE_LINK_PARAM_UPDATE_WAIT_REMOTE_PARAMS

// Connection Pause Peripheral time value (in seconds)
#define DEFAULT_CONN_PAUSE_PERIPHERAL         6

// How often to perform periodic event (in msec)
#define MDP_PERIODIC_EVT_PERIOD               500  // 5000 Original
#define MDP_LED_BLINK_EVT_PERIOD              250
#define MDP_SENSOR_MOVE_EVT_PERIOD            40

// Type of Display to open
#if !defined(Display_DISABLE_ALL)
  #if defined(BOARD_DISPLAY_USE_LCD) && (BOARD_DISPLAY_USE_LCD!=0)
    #define SBP_DISPLAY_TYPE Display_Type_LCD
  #elif defined (BOARD_DISPLAY_USE_UART) && (BOARD_DISPLAY_USE_UART!=0)
    #define SBP_DISPLAY_TYPE Display_Type_UART
  #else // !BOARD_DISPLAY_USE_LCD && !BOARD_DISPLAY_USE_UART
    #define SBP_DISPLAY_TYPE 0 // Option not supported
  #endif // BOARD_DISPLAY_USE_LCD && BOARD_DISPLAY_USE_UART
#else // BOARD_DISPLAY_USE_LCD && BOARD_DISPLAY_USE_UART
  #define SBP_DISPLAY_TYPE 0 // No Display
#endif // !Display_DISABLE_ALL

#ifdef FEATURE_OAD
// The size of an OAD packet.
#define OAD_PACKET_SIZE                       ((OAD_BLOCK_SIZE) + 2)
#endif // FEATURE_OAD

// Task configuration
#define SBP_TASK_PRIORITY                     1

#ifndef SBP_TASK_STACK_SIZE
#define SBP_TASK_STACK_SIZE                   644
#endif

// Internal Events for RTOS application
#define MDP_STATE_CHANGE_EVT                  Event_Id_00
#define MDP_CHAR_CHANGE_EVT                   Event_Id_01
#define MDP_PERIODIC_EVT                      Event_Id_02
#define MDP_CONN_EVT_END_EVT                  Event_Id_03

#define MDP_KEY_CHANGE_EVT                    Event_Id_04
#define MDP_LED_BLINK_EVT                     Event_Id_05
#define MDP_SENSOR_MOVE_EVT                   Event_Id_06

// Application specific event ID for HCI Connection Event End Events
#define SBP_HCI_CONN_EVT_END_EVT              Event_Id_07

// Internal Events for RTOS application
#define SBP_ICALL_EVT                         ICALL_MSG_EVENT_ID // Event_Id_31
#define SBP_QUEUE_EVT                         UTIL_QUEUE_EVENT_ID // Event_Id_30
//#define SBP_PERIODIC_EVT                      Event_Id_00

#if defined (NPI_USE_UART) || defined (NPI_USE_SPI)
//events that TL will use to control the driver
#define TRANSPORT_RX_EVENT                    Event_Id_09
#define MRDY_EVENT                            Event_Id_08
#define TRANSPORT_TX_DONE_EVENT               Event_Id_10
#endif //TL


#ifdef FEATURE_OAD
// Additional Application Events for OAD
#define SBP_QUEUE_PING_EVT                    Event_Id_08

// Bitwise OR of all events to pend on with OAD
#define SBP_ALL_EVENTS                        (SBP_ICALL_EVT        | \
                                               SBP_QUEUE_EVT        | \
                                               MDP_PERIODIC_EVT     | \
                                               SBP_QUEUE_PING_EVT)
#else
// Bitwise OR of all events to pend on
// no need to include the queue events which are captured in "UTIL_QUEUE_EVENT_ID", when you queue any of the 3 guys (char change, key change and stack event change) you create a "UTIL_QUEUE_EVENT_ID", when in endless loop we see this event we un-queue the message and then we know which of the 3 guys happened
#define SBP_ALL_EVENTS                        (SBP_ICALL_EVT        | \
                                               SBP_QUEUE_EVT        | \
                                               MDP_LED_BLINK_EVT    | \
                                               MDP_SENSOR_MOVE_EVT  | \
                                               MDP_PERIODIC_EVT)

                                               /*MDP_LED_BLINK_EVT    | \
                                               MDP_KEY_CHANGE_EVT   | \ */
#endif /* FEATURE_OAD */

// Row numbers for two-button menu
#define SBP_ROW_RESULT        TBM_ROW_APP
#define SBP_ROW_STATUS_1      (TBM_ROW_APP + 1)
#define SBP_ROW_STATUS_2      (TBM_ROW_APP + 2)
#define SBP_ROW_ROLESTATE     (TBM_ROW_APP + 3)
#define SBP_ROW_BDADDR        (TBM_ROW_APP + 4)

#define SENSOR_MOVE_COUNT                       10
#define ALARM_MOVEMENT_THRESHOLD                10


#if defined (NPI_USE_UART) || defined (NPI_USE_SPI)
#define APP_TL_BUFF_SIZE                      150
#endif //TL

/*********************************************************************
 * TYPEDEFS
 */
uint8_t valueForTest = 0;

// App event passed from profiles.
typedef struct
{
  appEvtHdr_t hdr;  // event header.
} sbpEvt_t;

/*********************************************************************
 * GLOBAL VARIABLES
 */

// Display Interface
Display_Handle dispHandle = NULL;

//static UART_Handle uartHandle;


//! \brief Pointer to NPI TL RX Buffer
//static Char* TransportTxBuf;

//extern UARTCC26XX_Object uartCC26XXObjects[];

//ICall_Semaphore sem; // Semaphore used for ICall

/*********************************************************************
 * LOCAL VARIABLES
 */
uint8_t tempMem = 0;
// Entity ID globally used to check for source and/or destination of messages
static ICall_EntityID selfEntity;

// Event globally used to post local events and pend on system and
// local events.
static ICall_SyncHandle syncEvent; // Synchronization object data type

//static ICall_SyncHandle syncEvent2; // Synchronization object data type

// Clock instances for internal periodic events.
static Clock_Struct periodicClock;
static Clock_Struct ledBlinkClock;
static Clock_Struct sensorMovementClock;

// Queue object used for app messages
static Queue_Struct appMsg;
static Queue_Handle appMsgQueue;

#if defined(FEATURE_OAD)
// Event data from OAD profile.
static Queue_Struct oadQ;
static Queue_Handle hOadQ;
#endif //FEATURE_OAD

// Task configuration
Task_Struct sbpTask;
Char sbpTaskStack[SBP_TASK_STACK_SIZE];

// Scan response data (max size = 31 bytes)
static uint8_t scanRspData[] =
{
  // complete name
  17,   // length of this data
  GAP_ADTYPE_LOCAL_NAME_COMPLETE,
  'C',
  'C',
  '2',
  '6',
  '5',
  '0',
  ' ',
  'S',
  'e',
  'n',
  's',
  'o',
  'r',
  'T',
  'a',
  'g',

  // connection interval range
  0x05,   // length of this data
  GAP_ADTYPE_SLAVE_CONN_INTERVAL_RANGE,
  LO_UINT16(DEFAULT_DESIRED_MIN_CONN_INTERVAL),   // 100ms
  HI_UINT16(DEFAULT_DESIRED_MIN_CONN_INTERVAL),
  LO_UINT16(DEFAULT_DESIRED_MAX_CONN_INTERVAL),   // 1s
  HI_UINT16(DEFAULT_DESIRED_MAX_CONN_INTERVAL),

  // Tx power level
  0x02,   // length of this data
  GAP_ADTYPE_POWER_LEVEL,
  0       // 0dBm
};

// Advertisement data (max size = 31 bytes, though this is
// best kept short to conserve power while advertising)
static uint8_t advertData[] =
{
  // Flags: this field sets the device to use general discoverable
  // mode (advertises indefinitely) instead of general
  // discoverable mode (advertise for 30 seconds at a time)
  0x02,   // length of this data
  GAP_ADTYPE_FLAGS,
  DEFAULT_DISCOVERABLE_MODE | GAP_ADTYPE_FLAGS_BREDR_NOT_SUPPORTED,

  // service UUID, to notify central devices what services are included
  // in this peripheral
#if !defined(FEATURE_OAD) || defined(FEATURE_OAD_ONCHIP)
  0x11,   // length of this data
#else //OAD for external flash
  0x05,  // length of this data
#endif //FEATURE_OAD
  GAP_ADTYPE_16BIT_MORE,      // some of the UUID's, but not all
#ifdef FEATURE_OAD
  LO_UINT16(OAD_SERVICE_UUID),
  HI_UINT16(OAD_SERVICE_UUID),
#endif //FEATURE_OAD
#ifndef FEATURE_OAD_ONCHIP
  //LO_UINT16(SIMPLEPROFILE_SERV_UUID),
  //HI_UINT16(SIMPLEPROFILE_SERV_UUID)
  GAP_ADTYPE_128BIT_MORE, // Shirzad Original: LO_UINT16(SIMPLEPROFILE_SERV_UUID),
  TI_BASE_UUID_128(MOVEDETECTOR_SERV_UUID), //(SIMPLEPROFILE_SERV_UUID), // Shirzad Original: HI_UINT16(SIMPLEPROFILE_SERV_UUID),
#endif //FEATURE_OAD_ONCHIP
};

// GAP GATT Attributes
static uint8_t attDeviceName[GAP_DEVICE_NAME_LEN] = "Project Angela"; // "Project Angela"; // "Simple Peripheral";

// Globals used for ATT Response retransmission
static gattMsgEvent_t *pAttRsp = NULL;
static uint8_t rspTxRetry = 0;

uint8_t ledBlinkCount = 0;
uint8_t sensorCheckCount = 0;

static uint16_t static_xyzValue[3][SENSOR_MOVE_COUNT];
//static uint16_t static_yValue[SENSOR_MOVE_COUNT];
//static uint16_t static_zValue[SENSOR_MOVE_COUNT];

#if defined (NPI_USE_UART) || defined (NPI_USE_SPI)
//used to store data read from transport layer
static uint8_t appRxBuf[APP_TL_BUFF_SIZE];
#endif //TL

/*********************************************************************
 * LOCAL FUNCTIONS
 */

static void Movedetector_init( void );
static void Movedetector_taskFxn(UArg a0, UArg a1);

static uint8_t Movedetector_processStackMsg(ICall_Hdr *pMsg);
static uint8_t Movedetector_processGATTMsg(gattMsgEvent_t *pMsg);
static void Movedetector_processAppMsg(sbpEvt_t *pMsg);
static void Movedetector_processStateChangeEvt(gaprole_States_t newState);
static void Movedetector_processCharValueChangeEvt(uint8_t paramID);
static void Movedetector_performPeriodicTask(void);
//static void Movedetector_clockHandler(UArg arg);
static void Movedetector_clockHandler(UArg arg);

static void Movedetector_sendAttRsp(void);
static void Movedetector_freeAttRsp(uint8_t status);

static void Movedetector_stateChangeCB(gaprole_States_t newState);
#ifndef FEATURE_OAD_ONCHIP
static void Movedetector_charValueChangeCB(uint8_t paramID);
#endif //!FEATURE_OAD_ONCHIP
static void Movedetector_enqueueMsg(uint8_t event, uint8_t state);
//static void MovedetectorSensor_enqueueMsg(uint8_t event, uint8_t state);

#ifdef FEATURE_OAD
void Movedetector_processOadWriteCB(uint8_t event, uint16_t connHandle,
                                           uint8_t *pData);
#endif //FEATURE_OAD

void MovedetectorSensor_keyChangeHandler(uint8 keysPressed);
//static void Movedetector_handleKeys(uint8_t keys);
static void MovedetectorSensor_handleKeys(uint8_t shift, uint8_t keys);
static void ReadSensorValue(void);
static void CheckForAlarm(void);

//uint8_t SimpleUART_initialize(Char *tRxBuf, Char *tTxBuf);
//void Write_Hello(void);
//static void NPITLUART_writeCallBack(UART_Handle handle, void *ptr, size_t size);
//static void NPITLUART_readCallBack(UART_Handle handle, void *ptr, size_t size);

#if defined (NPI_USE_UART) || defined (NPI_USE_SPI)
//TL packet parser
static void SimpleBLEPeripheral_TLpacketParser(void);
#endif //TL

/*********************************************************************
 * EXTERN FUNCTIONS
 */
extern void AssertHandler(uint8 assertCause, uint8 assertSubcause);

/*********************************************************************
 * PROFILE CALLBACKS
 */

// Peripheral GAPRole Callbacks
static gapRolesCBs_t Movedetector_gapRoleCBs =
{
     Movedetector_stateChangeCB     // GAPRole State Change Callbacks
};

// GAP Bond Manager Callbacks
// These are set to NULL since they are not needed. The application
// is set up to only perform justworks pairing.
static gapBondCBs_t Movedetector_BondMgrCBs =
{
  NULL, // Passcode callback
  NULL  // Pairing / Bonding state Callback
};

// Simple GATT Profile Callbacks
#ifndef FEATURE_OAD_ONCHIP
static movedetectorCBs_t MovedetectorCBs =
{
 Movedetector_charValueChangeCB // Simple GATT Characteristic value change callback
};
#endif //!FEATURE_OAD_ONCHIP

#ifdef FEATURE_OAD
static oadTargetCBs_t movedetector_oadCBs =
{
     Movedetector_processOadWriteCB // OAD Profile Characteristic value change callback.
};
#endif //FEATURE_OAD

#if defined (NPI_USE_UART) || defined (NPI_USE_SPI)
static TLCBs_t SimpleBLEPeripheral_TLCBs =
{
  SimpleBLEPeripheral_TLpacketParser // parse data read from transport layer
};
#endif

/*********************************************************************
 * PUBLIC FUNCTIONS
 */

/*
// -----------------------------------------------------------------------------
//! \brief      This routine initializes the transport layer and opens the port
//!             of the device.
//!
//! \param[in]  tRxBuf - pointer to NPI TL Tx Buffer
//! \param[in]  tTxBuf - pointer to NPI TL Rx Buffer
//! \param[in]  npiCBack - NPI TL call back function to be invoked at the end of
//!             a UART transaction
//!
//! \return     void
// -----------------------------------------------------------------------------
uint8_t SimpleUART_initialize(Char *tRxBuf, Char *tTxBuf)
{
    UART_Params params;

//    TransportRxBuf = tRxBuf;
    TransportTxBuf = tTxBuf;
//    npiTransmitCB = npiCBack;

    // Initialize the UART driver
    Board_initUART();

    // Configure UART parameters.
    UART_Params_init(&params);
    params.baudRate = 115200;
    params.readDataMode = UART_DATA_BINARY;
    params.writeDataMode = UART_DATA_BINARY;
    params.dataLength = UART_LEN_8;
    params.stopBits = UART_STOP_ONE;
    params.readMode = UART_MODE_BLOCKING;
    params.writeMode = UART_MODE_BLOCKING;
    params.readEcho = UART_ECHO_OFF;

    params.readCallback = NPITLUART_readCallBack;
    params.writeCallback = NPITLUART_writeCallBack;

    // Open / power on the UART.
    uartHandle = UART_open(Board_UART0, &params);
    //Enable Partial Reads on all subsequent UART_read()
//    UART_control(uartHandle, UARTCC26XX_CMD_RETURN_PARTIAL_ENABLE,  NULL);


#if (NPI_FLOW_CTRL == 0)
    // This call will start repeated Uart Reads when Power Savings is disabled
    NPITLUART_readTransport();
#endif // NPI_FLOW_CTRL = 0

    return 0;
}

void Write_Hello(void)
{
//    uint8_t tempstatus = 13;
    ICall_CSState key;
    key = ICall_enterCriticalSection();

    //TransportTxLen = len;

#if (NPI_FLOW_CTRL == 1)
    TxActive = TRUE;

    // Start reading prior to impending write transaction
    // We can only call UART_write() once MRDY has been signaled from Master
    // device
    //NPITLUART_readTransport();
#else
    // Check to see if transport is successful. If not, reset TxLen to allow
    // another write to be processed
    if(UART_write(uartHandle, TransportTxBuf, 13) == UART_ERROR ) //UART_write(uartHandle, TransportTxBuf, TransportTxLen) == UART_ERROR
    {
      //TransportTxLen = 0;
    }
#endif // NPI_FLOW_CTRL = 1
    ICall_leaveCriticalSection(key);

   // return TransportTxLen;
   // tempstatus =  UART_write(uartHandle, TransportTxBuf, 13) ;
}
// -----------------------------------------------------------------------------
//! \brief      This callback is invoked on Read completion of readSize/receive
//!             timeout
//!
//! \param[in]  handle - handle to the UART port
//! \param[in]  ptr    - pointer to buffer to read data into
//! \param[in]  size   - size of the data
//!
//! \return     void
// -----------------------------------------------------------------------------
static void NPITLUART_readCallBack(UART_Handle handle, void *ptr, size_t size)
{
    ICall_CSState key;
    key = ICall_enterCriticalSection();

    if (size)
    {
        if (size != NPITLUART_readIsrBuf(size))
        {
            // Buffer overflow imminent. Cancel read and pass to higher layers
            // for handling
#if (NPI_FLOW_CTRL == 1)
            RxActive = FALSE;
#endif // NPI_FLOW_CTRL = 1
            if ( npiTransmitCB )
            {
                npiTransmitCB(NPI_TL_BUF_SIZE,TransportTxLen);
            }
        }
    }

#if (NPI_FLOW_CTRL == 1)
    // Read has been cancelled by transport layer, or bus timeout and no bytes in FIFO
    //    - do not invoke another read
    if ( !UARTCharsAvail(((UARTCC26XX_HWAttrsV2 const *)(uartHandle->hwAttrs))->baseAddr) &&
            mrdy_flag )
    {
        RxActive = FALSE;

        // If TX has also completed then we are safe to issue call back
        if ( !TxActive && npiTransmitCB )
        {
            npiTransmitCB(TransportRxLen,TransportTxLen);
        }
    }
    else
    {
        UART_read(uartHandle, &isrRxBuf[0], UART_ISR_BUF_SIZE);
    }
#else
    if ( npiTransmitCB )
    {
        npiTransmitCB(size,0);
    }
    TransportRxLen = 0;
    UART_read(uartHandle, &isrRxBuf[0], UART_ISR_BUF_SIZE);
#endif // NPI_FLOW_CTRL = 1

    ICall_leaveCriticalSection(key);
}
// -----------------------------------------------------------------------------
//! \brief      This callback is invoked on Write completion
//!
//! \param[in]  handle - handle to the UART port
//! \param[in]  ptr    - pointer to data to be transmitted
//! \param[in]  size   - size of the data
//!
//! \return     void
// -----------------------------------------------------------------------------
static void NPITLUART_writeCallBack(UART_Handle handle, void *ptr, size_t size)
{
    ICall_CSState key;
    key = ICall_enterCriticalSection();

#if (NPI_FLOW_CTRL == 1)
    if ( !RxActive )
    {
        UART_readCancel(uartHandle);
        if ( npiTransmitCB )
        {
            npiTransmitCB(TransportRxLen,TransportTxLen);
        }
    }

    TxActive = FALSE;
#else
//    if ( npiTransmitCB )
//    {
//        npiTransmitCB(0,TransportTxLen);
//    }
#endif // NPI_FLOW_CTRL = 1

    ICall_leaveCriticalSection(key);
}
*/
/*********************************************************************
 * @fn      Movedetector_createTask
 *
 * @brief   Task creation function for the Simple Peripheral.
 *
 * @param   None.
 *
 * @return  None.
 */
void Movedetector_createTask(void)
{
  Task_Params taskParams;

  // Configure task
  Task_Params_init(&taskParams);
  taskParams.stack = sbpTaskStack;
  taskParams.stackSize = SBP_TASK_STACK_SIZE;
  taskParams.priority = SBP_TASK_PRIORITY;

  Task_construct(&sbpTask, Movedetector_taskFxn, &taskParams, NULL);
}

/*********************************************************************
 * @fn      Movedetector_init
 *
 * @brief   Called during initialization and contains application
 *          specific initialization (ie. hardware initialization/setup,
 *          table initialization, power up notification, etc), and
 *          profile initialization/setup.
 *
 * @param   None.
 *
 * @return  None.
 */
static void Movedetector_init(void)
{
    LIS3DH_Filter filter_Parms;
    uint8_t temp;
    filter_Parms.highPassFilterIntEnable = LIS3DH_HF_FILTER_INT_AI1; //LIS3DH_HF_FILTER_INT_NONE; //LIS3DH_HF_FILTER_INT_AI1;
    filter_Parms.highPassFilterDataSel = LIS3DH_HF_FILTER_DATA_SEL_OUT;
    filter_Parms.highPassFilterMode = LIS3DH_HF_FILTER_MODE_NORMAL_RESET;
    filter_Parms.highPassFilterCutOffFreq = LIS3DH_HF_FILTER_CUTOFF_FREQ_3;

    /*
    // Enable System_printf(..) UART output
    UART_Handle uart_handle;
    UART_Params uartParams;
    */
//    Char tRxBuf[] = "Hello World";    // Transmit buffer
//    uint8_t tTxBuf[] = "Hello World";    // Transmit buffer

  // ******************************************************************
  // N0 STACK API CALLS CAN OCCUR BEFORE THIS CALL TO ICall_registerApp
  // ******************************************************************
  // Register the current thread as an ICall dispatcher application
  // so that the application can send and receive messages.
  ICall_registerApp(&selfEntity, &syncEvent);
//  ICall_registerApp(&selfEntity, &syncEvent2);

#ifdef USE_RCOSC
  RCOSC_enableCalibration();
#endif // USE_RCOSC

#if defined( USE_FPGA )
  // configure RF Core SMI Data Link
  IOCPortConfigureSet(IOID_12, IOC_PORT_RFC_GPO0, IOC_STD_OUTPUT);
  IOCPortConfigureSet(IOID_11, IOC_PORT_RFC_GPI0, IOC_STD_INPUT);

  // configure RF Core SMI Command Link
  IOCPortConfigureSet(IOID_10, IOC_IOCFG0_PORT_ID_RFC_SMI_CL_OUT, IOC_STD_OUTPUT);
  IOCPortConfigureSet(IOID_9, IOC_IOCFG0_PORT_ID_RFC_SMI_CL_IN, IOC_STD_INPUT);

  // configure RF Core tracer IO
  IOCPortConfigureSet(IOID_8, IOC_PORT_RFC_TRC, IOC_STD_OUTPUT);
#else // !USE_FPGA
  #if defined( DEBUG_SW_TRACE )
    // configure RF Core tracer IO
    IOCPortConfigureSet(IOID_8, IOC_PORT_RFC_TRC, IOC_STD_OUTPUT | IOC_CURRENT_4MA | IOC_SLEW_ENABLE);
  #endif // DEBUG_SW_TRACE
#endif // USE_FPGA

  // Create an RTOS queue for message from profile to be sent to app.
  appMsgQueue = Util_constructQueue(&appMsg);

  // Create one-shot clocks for internal periodic events.
  Util_constructClock(&periodicClock, Movedetector_clockHandler,
                      MDP_PERIODIC_EVT_PERIOD, 0, false, MDP_PERIODIC_EVT);

  // Create one-shot clocks for led_blinking events.
  Util_constructClock(&ledBlinkClock, Movedetector_clockHandler,
                      MDP_LED_BLINK_EVT_PERIOD, 0, false, MDP_LED_BLINK_EVT);

  // Create one-shot clocks for sensor events.
  Util_constructClock(&sensorMovementClock, Movedetector_clockHandler,
                      MDP_SENSOR_MOVE_EVT_PERIOD, 0, false, MDP_SENSOR_MOVE_EVT);


  // Initialize keys
  Board_initKeys(MovedetectorSensor_keyChangeHandler);

  // Init LED
  Led_init();

#if defined (NPI_USE_UART) || defined (NPI_USE_SPI)
  //initialize and pass information to TL
  TLinit(&syncEvent, &SimpleBLEPeripheral_TLCBs, TRANSPORT_TX_DONE_EVENT, TRANSPORT_RX_EVENT, MRDY_EVENT); // sem
#endif //TL

//  while(1)
//  {
//  tTxBuf = "Hello World";
  uint8_t tTxBuf[] = "Hello World";
      TLwrite (tTxBuf, sizeof(tTxBuf));
      //This function will write len amount of bytes via SPI/UART starting at memory location buf
//  }
      //temp = 0;
      //RPrintf("******* %d\r\n", temp);

/*
  UART_Params_init(&uartParams);
  uartParams.baudRate = 115200;
  uartParams.dataLength = UART_LEN_8;
  uartParams.stopBits = UART_STOP_ONE;
  uartParams.readEcho = UART_ECHO_OFF;

  UART_init();
  //UartPrintf_init(UART_open(CC2640R2_SABLEXR2_UART0, &uartParams));
  uart_handle = UART_open(CC2640R2_SABLEXR2_UART0, &uartParams);
  if (uart_handle == NULL) {
      //System_abort("Error opening the UART");
  }
  UART_write(uart_handle, txBuf, sizeof(txBuf));
*/
//  SimpleUART_initialize(tRxBuf, tTxBuf);
//  Write_Hello();


//  PINCC26XX_setOutputValue(Board_SPI0_CSN, 1);
//  PINCC26XX_setOutputEnable(Board_SPI0_CLK, 1);
//  PINCC26XX_setOutputValue(Board_SPI0_CLK, 0);

  ////////////////////////////////// LIS3DH /////////////////////////////////////////////////////

  if (!LIS3DH_Initialize())
  {
      PINCC26XX_setOutputValue(Board_BLED, 0);
      while(1);
      //tempMem = 1;
  }

  LIS3DH_SetDeviceMode(LIS3DH_MODE_LOW_POWER, LIS3DH_ODR_25_HZ, LIS3DH_FULL_SCALE_SELECTION_4G);
  /////////////////////// THIS IS TO SETUP THE HIGH PASS FILTER INTERRUPT /////////////////////////////////////////////////////////////////////////////////
  LIS3DH_SetFilter(filter_Parms);
  LIS3DH_InterruptCtrl();
  LIS3DH_Interrupt1Threshold(0x05); // Best way to adjust sensitivity
  LIS3DH_Interrupt1Duration(2);// you can add duration here too
  LIS3DH_ReadRefrence(&temp); // Dummy read to force the HP filter to current acceleration value  (i.e. set reference acceleration/tilt value)
  LIS3DH_Interrupt1Config(0x2A); // Configure desired wake-up event (AOI 6D ZHIE ZLIE YHIE YLIE XHIE XLIE)
  RPrintf("8\r\n");


/////////////////////////////////////////////////////////////////////////////////////////////////

  dispHandle = Display_open(SBP_DISPLAY_TYPE, NULL);

  // Set GAP Parameters: After a connection was established, delay in seconds
  // before sending when GAPRole_SetParameter(GAPROLE_PARAM_UPDATE_ENABLE,...)
  // uses GAPROLE_LINK_PARAM_UPDATE_INITIATE_BOTH_PARAMS or
  // GAPROLE_LINK_PARAM_UPDATE_INITIATE_APP_PARAMS
  // For current defaults, this has no effect.
  GAP_SetParamValue(TGAP_CONN_PAUSE_PERIPHERAL, DEFAULT_CONN_PAUSE_PERIPHERAL);

  // Setup the Peripheral GAPRole Profile. For more information see the User's Guide:
  // http://software-dl.ti.com/lprf/ble5stack-docs-latest/html/ble-stack/gaprole.html
  {
    // Device starts advertising upon initialization of GAP
    uint8_t initialAdvertEnable = TRUE;

    // By setting this to zero, the device will go into the waiting state after
    // being discoverable for 30.72 second, and will not being advertising again
    // until re-enabled by the application
    uint16_t advertOffTime = 0;

    uint8_t enableUpdateRequest = DEFAULT_ENABLE_UPDATE_REQUEST;
    uint16_t desiredMinInterval = DEFAULT_DESIRED_MIN_CONN_INTERVAL;
    uint16_t desiredMaxInterval = DEFAULT_DESIRED_MAX_CONN_INTERVAL;
    uint16_t desiredSlaveLatency = DEFAULT_DESIRED_SLAVE_LATENCY;
    uint16_t desiredConnTimeout = DEFAULT_DESIRED_CONN_TIMEOUT;

    // Set the Peripheral GAPRole Parameters
    GAPRole_SetParameter(GAPROLE_ADVERT_ENABLED, sizeof(uint8_t),
                         &initialAdvertEnable);
    GAPRole_SetParameter(GAPROLE_ADVERT_OFF_TIME, sizeof(uint16_t),
                         &advertOffTime);

    GAPRole_SetParameter(GAPROLE_SCAN_RSP_DATA, sizeof(scanRspData),
                         scanRspData);
    GAPRole_SetParameter(GAPROLE_ADVERT_DATA, sizeof(advertData), advertData);

    GAPRole_SetParameter(GAPROLE_PARAM_UPDATE_ENABLE, sizeof(uint8_t),
                         &enableUpdateRequest);
    GAPRole_SetParameter(GAPROLE_MIN_CONN_INTERVAL, sizeof(uint16_t),
                         &desiredMinInterval);
    GAPRole_SetParameter(GAPROLE_MAX_CONN_INTERVAL, sizeof(uint16_t),
                         &desiredMaxInterval);
    GAPRole_SetParameter(GAPROLE_SLAVE_LATENCY, sizeof(uint16_t),
                         &desiredSlaveLatency);
    GAPRole_SetParameter(GAPROLE_TIMEOUT_MULTIPLIER, sizeof(uint16_t),
                         &desiredConnTimeout);
  }

  // Set the Device Name characteristic in the GAP GATT Service
  // For more information, see the section in the User's Guide:
  // http://software-dl.ti.com/lprf/ble5stack-docs-latest/html/ble-stack/gaprole.html
  GGS_SetParameter(GGS_DEVICE_NAME_ATT, GAP_DEVICE_NAME_LEN, attDeviceName);

  // Set GAP Parameters to set the advertising interval
  // For more information, see the GAP section of the User's Guide:
  // http://software-dl.ti.com/lprf/ble5stack-docs-latest/html/ble-stack/gatt.html#gap-gatt-service-ggs
  {
    // Use the same interval for general and limited advertising.
    // Note that only general advertising will occur based on the above configuration
    uint16_t advInt = DEFAULT_ADVERTISING_INTERVAL;

    GAP_SetParamValue(TGAP_LIM_DISC_ADV_INT_MIN, advInt);
    GAP_SetParamValue(TGAP_LIM_DISC_ADV_INT_MAX, advInt);
    GAP_SetParamValue(TGAP_GEN_DISC_ADV_INT_MIN, advInt);
    GAP_SetParamValue(TGAP_GEN_DISC_ADV_INT_MAX, advInt);
  }

  // Setup the GAP Bond Manager. For more information see the section in the
  // User's Guide:
  // http://software-dl.ti.com/lprf/ble5stack-docs-latest/html/ble-stack/gapbondmngr.html#
  {
    // Hard code the passkey that will be used for pairing. The GAPBondMgr will
    // use this key instead of issuing a callback to the application. This only
    // works if both sides of the connection know to use this same key at
    // compile-time.
    uint32_t passkey = 0; // passkey "000000"
    // Don't send a pairing request after connecting; the peer device must
    // initiate pairing
    uint8_t pairMode = GAPBOND_PAIRING_MODE_WAIT_FOR_REQ;
    // Use authenticated pairing: require passcode.
    uint8_t mitm = TRUE;
    // This device only has display capabilities. Therefore, it will display the
    // passcode during pairing. However, since the default passcode is being
    // used, there is no need to display anything.
    uint8_t ioCap = GAPBOND_IO_CAP_DISPLAY_ONLY;
    // Request bonding (storing long-term keys for re-encryption upon subsequent
    // connections without repairing)
    uint8_t bonding = TRUE;

    GAPBondMgr_SetParameter(GAPBOND_DEFAULT_PASSCODE, sizeof(uint32_t),
                            &passkey);
    GAPBondMgr_SetParameter(GAPBOND_PAIRING_MODE, sizeof(uint8_t), &pairMode);
    GAPBondMgr_SetParameter(GAPBOND_MITM_PROTECTION, sizeof(uint8_t), &mitm);
    GAPBondMgr_SetParameter(GAPBOND_IO_CAPABILITIES, sizeof(uint8_t), &ioCap);
    GAPBondMgr_SetParameter(GAPBOND_BONDING_ENABLED, sizeof(uint8_t), &bonding);
  }

  // Initialize GATT attributes
  GGS_AddService(GATT_ALL_SERVICES);           // GAP GATT Service
  GATTServApp_AddService(GATT_ALL_SERVICES);   // GATT Service
  DevInfo_AddService();                        // Device Information Service

#ifndef FEATURE_OAD_ONCHIP
  Movedetector_AddService(GATT_ALL_SERVICES); // Simple GATT Profile
#endif //!FEATURE_OAD_ONCHIP

#ifdef FEATURE_OAD
  VOID OAD_addService();                       // OAD Profile
  OAD_register((oadTargetCBs_t *)&simpleBLEPeripheral_oadCBs);
  hOadQ = Util_constructQueue(&oadQ);
#endif //FEATURE_OAD

#ifdef IMAGE_INVALIDATE
  Reset_addService();
#endif //IMAGE_INVALIDATE


#ifndef FEATURE_OAD_ONCHIP
  // Setup the movedetector Characteristic Values
  // For more information, see the sections in the User's Guide:
  // http://software-dl.ti.com/lprf/ble5stack-docs-latest/html/ble-stack/gatt.html#
  // http://software-dl.ti.com/lprf/ble5stack-docs-latest/html/ble-stack/gatt.html#gattservapp-module
  {
/*    uint8_t charValue1 = 1;
    uint8_t charValue2 = 2;
    uint8_t charValue3 = 3;
    uint8_t charValue4 = 4;
    uint8_t charValue5[SIMPLEPROFILE_CHAR5_LEN] = { 1, 2, 3, 4, 5 };

    SimpleProfile_SetParameter(SIMPLEPROFILE_CHAR1, sizeof(uint8_t),
                               &charValue1);
    SimpleProfile_SetParameter(SIMPLEPROFILE_CHAR2, sizeof(uint8_t),
                               &charValue2);
    SimpleProfile_SetParameter(SIMPLEPROFILE_CHAR3, sizeof(uint8_t),
                               &charValue3);
    SimpleProfile_SetParameter(SIMPLEPROFILE_CHAR4, sizeof(uint8_t),
                               &charValue4);
    SimpleProfile_SetParameter(SIMPLEPROFILE_CHAR5, SIMPLEPROFILE_CHAR5_LEN,
                               charValue5);*/
      uint8_t charValue1 = 0;
      uint8_t charValue2 = 23;
      uint16_t charValue3 = 0;
  //    uint8_t charValue4 = 4;
  //    uint8_t charValue5[SIMPLEPROFILE_CHAR5_LEN] = { 1, 2, 3, 4, 5 };

      Movedetector_SetParameter(MOVEDETECTOR_CHAR1, sizeof(uint8_t),
                                 &charValue1);
      Movedetector_SetParameter(MOVEDETECTOR_CHAR2, sizeof(uint8_t),
                                 &charValue2);
      Movedetector_SetParameter(MOVEDETECTOR_CHAR3, sizeof(uint16_t),
                                 &charValue3);
  /*    Movedetector_SetParameter(SIMPLEPROFILE_CHAR4, sizeof(uint8_t),
                                 &charValue4);
      Movedetector_SetParameter(SIMPLEPROFILE_CHAR5, SIMPLEPROFILE_CHAR5_LEN,
                                 charValue5);
                                 */
  }

  // Register callback with SimpleGATTprofile
  Movedetector_RegisterAppCBs(&MovedetectorCBs);
#endif //!FEATURE_OAD_ONCHIP

  // Start Bond Manager and register callback
  VOID GAPBondMgr_Register(&Movedetector_BondMgrCBs);

  // Register with GAP for HCI/Host messages. This is needed to receive HCI
  // events. For more information, see the section in the User's Guide:
  // http://software-dl.ti.com/lprf/ble5stack-docs-latest/html/ble-stack/hci.html
  GAP_RegisterForMsgs(selfEntity);

  // Register for GATT local events and ATT Responses pending for transmission
  GATT_RegisterForMsgs(selfEntity);

  // Set default values for Data Length Extension
  // This should be included only if Extended Data Length Feature is enabled
  // in build_config.opt in stack project.
  {
    //Set initial values to maximum, RX is set to max. by default(251 octets, 2120us)
    #define APP_SUGGESTED_PDU_SIZE 251 //default is 27 octets(TX)
    #define APP_SUGGESTED_TX_TIME 2120 //default is 328us(TX)

    // This API is documented in hci.h
    // See BLE5-Stack User's Guide for information on using this command:
    // http://software-dl.ti.com/lprf/ble5stack-docs-latest/html/ble-stack/data-length-extensions.html
    // HCI_LE_WriteSuggestedDefaultDataLenCmd(APP_SUGGESTED_PDU_SIZE, APP_SUGGESTED_TX_TIME);
  }

#if defined (BLE_V42_FEATURES) && (BLE_V42_FEATURES & PRIVACY_1_2_CFG)
  // Initialize GATT Client
  GATT_InitClient();

  // This line masks the Resolvable Private Address Only (RPAO) Characteristic
  // in the GAP GATT Server from being detected by remote devices. This value
  // cannot be toggled without power cycling but should remain consistent across
  // power-cycles. Removing this command when Privacy is used will cause this
  // device to be treated in Network Privacy Mode by bonded devices - this means
  // that after disconnecting they will not respond to this device's PDUs which
  // contain its Identity Address.
  // Devices wanting to use Network Privacy Mode with other BT5 devices, this
  // line should be commented out.
  GGS_SetParamValue(GGS_DISABLE_RPAO_CHARACTERISTIC);
#endif // BLE_V42_FEATURES & PRIVACY_1_2_CFG

#if !defined(Display_DISABLE_ALL)
  // Set the title of the main menu
  #if defined FEATURE_OAD
    #if defined (HAL_IMAGE_A)
      TBM_SET_TITLE(&sbpMenuMain, "BLE Peripheral A");
    #else
      TBM_SET_TITLE(&sbpMenuMain, "BLE Peripheral B");
    #endif // HAL_IMAGE_A
  #else
//    TBM_SET_TITLE(&sbpMenuMain, "BLE Peripheral");
  #endif // FEATURE_OAD

  // Initialize Two-Button Menu module
//  tbm_setItemStatus(&sbpMenuMain, TBM_ITEM_NONE, TBM_ITEM_ALL);
//  tbm_initTwoBtnMenu(dispHandle, &sbpMenuMain, 3, NULL);

  // Init key debouncer
//  Board_initKeys(Movedetector_keyChangeHandler);
#endif  // !Display_DISABLE_ALL

#if !defined (USE_LL_CONN_PARAM_UPDATE)
  // Get the currently set local supported LE features
  // The will result in a HCI_LE_READ_LOCAL_SUPPORTED_FEATURES event that
  // will get received in the main task processing loop. At this point,
  // feature bits can be set / cleared and the features can be updated.
//  HCI_LE_ReadLocalSupportedFeaturesCmd();
#endif // !defined (USE_LL_CONN_PARAM_UPDATE)

  // Start the GAPRole
  VOID GAPRole_StartDevice(&Movedetector_gapRoleCBs);
}

/*********************************************************************
 * @fn      Movedetector_taskFxn
 *
 * @brief   Application task entry point for the Simple Peripheral.
 *
 * @param   a0, a1 - not used.
 *
 * @return  None.
 */
static void Movedetector_taskFxn(UArg a0, UArg a1)
{
  // Initialize application
  Movedetector_init();

  // Application main loop
  for (;;)
  {
    uint32_t events;

    // Waits for an event to be posted associated with the calling thread.
    // Note that an event associated with a thread is posted when a
    // message is queued to the message receive queue of the thread
    events = Event_pend(syncEvent, Event_Id_NONE, SBP_ALL_EVENTS,
                        ICALL_TIMEOUT_FOREVER);

    if(events)// (events)
    {
      ICall_EntityID dest;
      ICall_ServiceEnum src;
      ICall_HciExtEvt *pMsg = NULL;

      // Fetch any available messages that might have been sent from the stack
      if (ICall_fetchServiceMsg(&src, &dest,
                                (void **)&pMsg) == ICALL_ERRNO_SUCCESS)
      {
        uint8 safeToDealloc = TRUE;

        if ((src == ICALL_SERVICE_CLASS_BLE) && (dest == selfEntity))
        {
          ICall_Stack_Event *pEvt = (ICall_Stack_Event *)pMsg;

          // Check for BLE stack events first
          if (pEvt->signature == 0xffff)
          {
            // The GATT server might have returned a blePending as it was trying
            // to process an ATT Response. Now that we finished with this
            // connection event, let's try sending any remaining ATT Responses
            // on the next connection event.
            if (pEvt->event_flag & SBP_HCI_CONN_EVT_END_EVT)
            {
              // Try to retransmit pending ATT Response (if any)
              Movedetector_sendAttRsp();
            }
          }
          else
          {
            // Process inter-task message
            safeToDealloc = Movedetector_processStackMsg((ICall_Hdr *)pMsg);
          }
        }

        if (pMsg && safeToDealloc)
        {
          ICall_freeMsg(pMsg);
        }
      }

      // If RTOS queue is not empty, process app message.
      if (events & UTIL_QUEUE_EVENT_ID)
      {
        while (!Queue_empty(appMsgQueue))
        {
          sbpEvt_t *pMsg = (sbpEvt_t *)Util_dequeueMsg(appMsgQueue);
          if (pMsg)
          {
            // Process message.
            Movedetector_processAppMsg(pMsg);

            // Free the space from the message.
            ICall_free(pMsg);
          }
        }
      }

      if (events & MDP_PERIODIC_EVT)
      {
        Util_startClock(&periodicClock);

        // Perform periodic application task
        Movedetector_performPeriodicTask();
      }
      if ( events & MDP_LED_BLINK_EVT && ledBlinkCount > 0) // events & MDP_LED_BLINK_EVT &&
      {
          events &= ~MDP_LED_BLINK_EVT;
          ledBlinkCount--;
          Util_startClock(&ledBlinkClock);  // start the next clock, this will continue until the led blink counter is zero
     //     Log_print0(Diags_USER1, "Toggling led\r\n");
          Toggle_led();
          valueForTest++;
          Movedetector_SetParameter(MOVEDETECTOR_CHAR2, sizeof(uint8_t), &valueForTest);
          PINCC26XX_setOutputValue(Board_RLED, Board_LED_OFF);
      }
      if (events & MDP_SENSOR_MOVE_EVT)
      {
          events &= ~MDP_SENSOR_MOVE_EVT;
          if(sensorCheckCount > 0)
          {
              sensorCheckCount--;
              Util_startClock(&sensorMovementClock);
              ReadSensorValue();
          }
          else
          {
              ////sensorCheckCount = SENSOR_MOVE_COUNT;
              CheckForAlarm();
          }
      }

#if defined (NPI_USE_UART) || defined (NPI_USE_SPI)
      if (events & MRDY_EVENT || events & TRANSPORT_RX_EVENT || events & TRANSPORT_TX_DONE_EVENT)
      {
          //TL handles driver events. this must be done first
//          TL_handleISRevent();
      }
#endif //TL

#ifdef FEATURE_OAD
      if (events & SBP_QUEUE_PING_EVT)
      {
        while (!Queue_empty(hOadQ))
        {
          oadTargetWrite_t *oadWriteEvt = Queue_get(hOadQ);

          // Identify new image.
          if (oadWriteEvt->event == OAD_WRITE_IDENTIFY_REQ)
          {
            OAD_imgIdentifyWrite(oadWriteEvt->connHandle, oadWriteEvt->pData);
          }
          // Write a next block request.
          else if (oadWriteEvt->event == OAD_WRITE_BLOCK_REQ)
          {
            OAD_imgBlockWrite(oadWriteEvt->connHandle, oadWriteEvt->pData);
          }

          // Free buffer.
          ICall_free(oadWriteEvt);
        }
      }
#endif //FEATURE_OAD
    }
  }
}

/*********************************************************************
 * @fn      Movedetector_processStackMsg
 *
 * @brief   Process an incoming stack message.
 *
 * @param   pMsg - message to process
 *
 * @return  TRUE if safe to deallocate incoming message, FALSE otherwise.
 */
static uint8_t Movedetector_processStackMsg(ICall_Hdr *pMsg)
{
  uint8_t safeToDealloc = TRUE;

  switch (pMsg->event)
  {
    case GATT_MSG_EVENT:
      // Process GATT message
      safeToDealloc = Movedetector_processGATTMsg((gattMsgEvent_t *)pMsg);
      break;

    case HCI_GAP_EVENT_EVENT:
      {

        // Process HCI message
        switch(pMsg->status)
        {
          case HCI_COMMAND_COMPLETE_EVENT_CODE:
            // Process HCI Command Complete Event
            {

#if !defined (USE_LL_CONN_PARAM_UPDATE)
              // This code will disable the use of the LL_CONNECTION_PARAM_REQ
              // control procedure (for connection parameter updates, the
              // L2CAP Connection Parameter Update procedure will be used
              // instead). To re-enable the LL_CONNECTION_PARAM_REQ control
              // procedures, define the symbol USE_LL_CONN_PARAM_UPDATE
              // The L2CAP Connection Parameter Update procedure is used to
              // support a delta between the minimum and maximum connection
              // intervals required by some iOS devices.

              // Parse Command Complete Event for opcode and status
              hciEvt_CmdComplete_t* command_complete = (hciEvt_CmdComplete_t*) pMsg;
              uint8_t   pktStatus = command_complete->pReturnParam[0];

              //find which command this command complete is for
              switch (command_complete->cmdOpcode)
              {
                case HCI_LE_READ_LOCAL_SUPPORTED_FEATURES:
                  {
                    if (pktStatus == SUCCESS)
                    {
                      uint8_t featSet[8];

                      // Get current feature set from received event (bits 1-9
                      // of the returned data
                      memcpy( featSet, &command_complete->pReturnParam[1], 8 );

                      // Clear bit 1 of byte 0 of feature set to disable LL
                      // Connection Parameter Updates
                      CLR_FEATURE_FLAG( featSet[0], LL_FEATURE_CONN_PARAMS_REQ );

                      // Update controller with modified features
                      HCI_EXT_SetLocalSupportedFeaturesCmd( featSet );
                    }
                  }
                  break;

                default:
                  //do nothing
                  break;
              }
#endif // !defined (USE_LL_CONN_PARAM_UPDATE)

            }
            break;

          case HCI_BLE_HARDWARE_ERROR_EVENT_CODE:
            AssertHandler(HAL_ASSERT_CAUSE_HARDWARE_ERROR,0);
            break;

          // LE Events
          case HCI_LE_EVENT_CODE:
            {
              hciEvt_BLEPhyUpdateComplete_t *pPUC
                = (hciEvt_BLEPhyUpdateComplete_t*) pMsg;

              // A Phy Update Has Completed or Failed
              if (pPUC->BLEEventCode == HCI_BLE_PHY_UPDATE_COMPLETE_EVENT)
              {
                if (pPUC->status != SUCCESS)
                {
                  Display_print0(dispHandle, SBP_ROW_STATUS_1, 0,
                                 "PHY Change failure");
                }
                else
                {
                  Display_print0(dispHandle, SBP_ROW_STATUS_1, 0,
                                 "PHY Update Complete");
                  // Only symmetrical PHY is supported.
                  // rxPhy should be equal to txPhy.
                  Display_print1(dispHandle, SBP_ROW_STATUS_2, 0,
                                 "Current PHY: %s",
                                 (pPUC->rxPhy == HCI_PHY_1_MBPS) ? "1 Mbps" :

// Note: BLE_V50_FEATURES is always defined and long range phy (PHY_LR_CFG) is
//       defined in build_config.opt
#if (BLE_V50_FEATURES & PHY_LR_CFG)
                                   ((pPUC->rxPhy == HCI_PHY_2_MBPS) ? "2 Mbps" :
                                       "Coded:S2"));
#else  // !PHY_LR_CFG
                                   "2 Mbps");
#endif // PHY_LR_CFG
                }
              }
            }
            break;

          default:
            break;
        }
      }
      break;

      default:
        // do nothing
        break;

    }

  return (safeToDealloc);
}

/*********************************************************************
 * @fn      Movedetector_processGATTMsg
 *
 * @brief   Process GATT messages and events.
 *
 * @return  TRUE if safe to deallocate incoming message, FALSE otherwise.
 */
static uint8_t Movedetector_processGATTMsg(gattMsgEvent_t *pMsg)
{
  // See if GATT server was unable to transmit an ATT response
  if (pMsg->hdr.status == blePending)
  {
    // No HCI buffer was available. Let's try to retransmit the response
    // on the next connection event.
    if (HCI_EXT_ConnEventNoticeCmd(pMsg->connHandle, selfEntity,
                                   SBP_HCI_CONN_EVT_END_EVT) == SUCCESS)
    {
      // First free any pending response
      Movedetector_freeAttRsp(FAILURE);

      // Hold on to the response message for retransmission
      pAttRsp = pMsg;

      // Don't free the response message yet
      return (FALSE);
    }
  }
  else if (pMsg->method == ATT_FLOW_CTRL_VIOLATED_EVENT)
  {
    // ATT request-response or indication-confirmation flow control is
    // violated. All subsequent ATT requests or indications will be dropped.
    // The app is informed in case it wants to drop the connection.

    // Display the opcode of the message that caused the violation.
    Display_print1(dispHandle, SBP_ROW_RESULT, 0, "FC Violated: %d", pMsg->msg.flowCtrlEvt.opcode);
  }
  else if (pMsg->method == ATT_MTU_UPDATED_EVENT)
  {
    // MTU size updated
    Display_print1(dispHandle, SBP_ROW_RESULT, 0, "MTU Size: %d", pMsg->msg.mtuEvt.MTU);
  }

  // Free message payload. Needed only for ATT Protocol messages
  GATT_bm_free(&pMsg->msg, pMsg->method);

  // It's safe to free the incoming message
  return (TRUE);
}

/*********************************************************************
 * @fn      Movedetector_sendAttRsp
 *
 * @brief   Send a pending ATT response message.
 *
 * @param   none
 *
 * @return  none
 */
static void Movedetector_sendAttRsp(void)
{
  // See if there's a pending ATT Response to be transmitted
  if (pAttRsp != NULL)
  {
    uint8_t status;

    // Increment retransmission count
    rspTxRetry++;

    // Try to retransmit ATT response till either we're successful or
    // the ATT Client times out (after 30s) and drops the connection.
    status = GATT_SendRsp(pAttRsp->connHandle, pAttRsp->method, &(pAttRsp->msg));
    if ((status != blePending) && (status != MSG_BUFFER_NOT_AVAIL))
    {
      // Disable connection event end notice
      HCI_EXT_ConnEventNoticeCmd(pAttRsp->connHandle, selfEntity, 0);

      // We're done with the response message
      Movedetector_freeAttRsp(status);
    }
    else
    {
      // Continue retrying
      Display_print1(dispHandle, SBP_ROW_STATUS_1, 0, "Rsp send retry: %d", rspTxRetry);
    }
  }
}

/*********************************************************************
 * @fn      Movedetector_freeAttRsp
 *
 * @brief   Free ATT response message.
 *
 * @param   status - response transmit status
 *
 * @return  none
 */
static void Movedetector_freeAttRsp(uint8_t status)
{
  // See if there's a pending ATT response message
  if (pAttRsp != NULL)
  {
    // See if the response was sent out successfully
    if (status == SUCCESS)
    {
      Display_print1(dispHandle, SBP_ROW_STATUS_1, 0, "Rsp sent retry: %d", rspTxRetry);
    }
    else
    {
      // Free response payload
      GATT_bm_free(&pAttRsp->msg, pAttRsp->method);

      Display_print1(dispHandle, SBP_ROW_STATUS_1, 0, "Rsp retry failed: %d", rspTxRetry);
    }

    // Free response message
    ICall_freeMsg(pAttRsp);

    // Reset our globals
    pAttRsp = NULL;
    rspTxRetry = 0;
  }
}

/*********************************************************************
 * @fn      Movedetector_processAppMsg
 *
 * @brief   Process an incoming callback from a profile.
 *
 * @param   pMsg - message to process
 *
 * @return  None.
 */
static void Movedetector_processAppMsg(sbpEvt_t *pMsg)
{
  switch (pMsg->hdr.event)
  {
    case MDP_STATE_CHANGE_EVT:
      Movedetector_processStateChangeEvt((gaprole_States_t)pMsg->
                                                hdr.state);
      break;

    case MDP_CHAR_CHANGE_EVT:
      Movedetector_processCharValueChangeEvt(pMsg->hdr.state);
      break;

    case MDP_KEY_CHANGE_EVT:
        // RPrintf("M\r\n");
//         PINCC26XX_setOutputValue(Board_GLED, LED_OFF);
         MovedetectorSensor_handleKeys(0, pMsg->hdr.state);
      break;
/*
#if !defined(Display_DISABLE_ALL)
    case MDP_KEY_CHANGE_EVT:
      Movedetector_handleKeys(pMsg->hdr.state);
      break;
#endif  // !Display_DISABLE_ALL
*/
    default:
      // Do nothing.
      break;
  }
}

/*********************************************************************
 * @fn      Movedetector_stateChangeCB
 *
 * @brief   Callback from GAP Role indicating a role state change.
 *
 * @param   newState - new state
 *
 * @return  None.
 */
static void Movedetector_stateChangeCB(gaprole_States_t newState)
{
  Movedetector_enqueueMsg(MDP_STATE_CHANGE_EVT, newState);
}

/*********************************************************************
 * @fn      Movedetector_processStateChangeEvt
 *
 * @brief   Process a pending GAP Role state change event.
 *
 * @param   newState - new state
 *
 * @return  None.
 */
static void Movedetector_processStateChangeEvt(gaprole_States_t newState)
{
#ifdef PLUS_BROADCASTER
  static bool firstConnFlag = false;
#endif // PLUS_BROADCASTER

  switch ( newState )
  {
    case GAPROLE_STARTED:
      {
        uint8_t ownAddress[B_ADDR_LEN];
        uint8_t systemId[DEVINFO_SYSTEM_ID_LEN];

        GAPRole_GetParameter(GAPROLE_BD_ADDR, ownAddress);

        // use 6 bytes of device address for 8 bytes of system ID value
        systemId[0] = ownAddress[0];
        systemId[1] = ownAddress[1];
        systemId[2] = ownAddress[2];

        // set middle bytes to zero
        systemId[4] = 0x00;
        systemId[3] = 0x00;

        // shift three bytes up
        systemId[7] = ownAddress[5];
        systemId[6] = ownAddress[4];
        systemId[5] = ownAddress[3];

        DevInfo_SetParameter(DEVINFO_SYSTEM_ID, DEVINFO_SYSTEM_ID_LEN, systemId);

        // Display device address
        Display_print0(dispHandle, SBP_ROW_BDADDR, 0, Util_convertBdAddr2Str(ownAddress));
        Display_print0(dispHandle, SBP_ROW_ROLESTATE, 0, "Initialized");
      }
      break;

    case GAPROLE_ADVERTISING:
//      Display_print0(dispHandle, SBP_ROW_ROLESTATE, 0, "Advertising");
//      Log_print0(0, "Advertising\r\n");
      break;

#ifdef PLUS_BROADCASTER
    // After a connection is dropped, a device in PLUS_BROADCASTER will continue
    // sending non-connectable advertisements and shall send this change of
    // state to the application.  These are then disabled here so that sending
    // connectable advertisements can resume.
    case GAPROLE_ADVERTISING_NONCONN:
      {
        uint8_t advertEnabled = FALSE;

        // Disable non-connectable advertising.
        GAPRole_SetParameter(GAPROLE_ADV_NONCONN_ENABLED, sizeof(uint8_t),
                           &advertEnabled);

        advertEnabled = TRUE;

        // Enabled connectable advertising.
        GAPRole_SetParameter(GAPROLE_ADVERT_ENABLED, sizeof(uint8_t),
                             &advertEnabled);

        // Reset flag for next connection.
        firstConnFlag = false;

        Movedetector_freeAttRsp(bleNotConnected);
      }
      break;
#endif //PLUS_BROADCASTER

    case GAPROLE_CONNECTED:
      {
        linkDBInfo_t linkInfo;
        uint8_t numActive = 0;

        Util_startClock(&periodicClock);

        numActive = linkDB_NumActive();

        // Use numActive to determine the connection handle of the last
        // connection
        RPrintf("GAPROLE_CONNECTED\r\n");
        if ( linkDB_GetInfo( numActive - 1, &linkInfo ) == SUCCESS )
        {
          Display_print1(dispHandle, SBP_ROW_ROLESTATE, 0, "Num Conns: %d", (uint16_t)numActive);
          Display_print0(dispHandle, SBP_ROW_STATUS_1, 0, Util_convertBdAddr2Str(linkInfo.addr));
        }
        else
        {
          uint8_t peerAddress[B_ADDR_LEN];

          GAPRole_GetParameter(GAPROLE_CONN_BD_ADDR, peerAddress);

          Display_print0(dispHandle, SBP_ROW_ROLESTATE, 0, "Connected");
          Display_print0(dispHandle, SBP_ROW_STATUS_1, 0, Util_convertBdAddr2Str(peerAddress));
        }

#if !defined(Display_DISABLE_ALL)
        tbm_setItemStatus(&sbpMenuMain, TBM_ITEM_ALL, TBM_ITEM_NONE);
#endif  // !Display_DISABLE_ALL

        #ifdef PLUS_BROADCASTER
          // Only turn advertising on for this state when we first connect
          // otherwise, when we go from connected_advertising back to this state
          // we will be turning advertising back on.
          if (firstConnFlag == false)
          {
            uint8_t advertEnabled = FALSE; // Turn on Advertising

            // Disable connectable advertising.
            GAPRole_SetParameter(GAPROLE_ADVERT_ENABLED, sizeof(uint8_t),
                                 &advertEnabled);

            // Set to true for non-connectable advertising.
            advertEnabled = TRUE;

            // Enable non-connectable advertising.
            GAPRole_SetParameter(GAPROLE_ADV_NONCONN_ENABLED, sizeof(uint8_t),
                                 &advertEnabled);
            firstConnFlag = true;
          }
        #endif // PLUS_BROADCASTER
      }
      break;

    case GAPROLE_CONNECTED_ADV:
      //Display_print0(dispHandle, SBP_ROW_ROLESTATE, 0, "Connected Advertising");
        RPrintf("GAPROLE_CONNECTED_ADV\r\n");
      break;

    case GAPROLE_WAITING:
      Util_stopClock(&periodicClock);
      Movedetector_freeAttRsp(bleNotConnected);
      RPrintf("GAPROLE_WAITING\r\n");

      //Display_print0(dispHandle, SBP_ROW_ROLESTATE, 0, "Disconnected");

#if !defined(Display_DISABLE_ALL)
      // Disable PHY change
      tbm_setItemStatus(&sbpMenuMain, TBM_ITEM_NONE, TBM_ITEM_ALL);
#endif  // !Display_DISABLE_ALL

      // Clear remaining lines
      //Display_clearLines(dispHandle, SBP_ROW_RESULT, SBP_ROW_STATUS_2);
      break;

    case GAPROLE_WAITING_AFTER_TIMEOUT:
      Movedetector_freeAttRsp(bleNotConnected);

      //Display_print0(dispHandle, SBP_ROW_RESULT, 0, "Timed Out");

#if !defined(Display_DISABLE_ALL)
      tbm_setItemStatus(&sbpMenuMain, TBM_ITEM_NONE, TBM_ITEM_ALL);
#endif  // !Display_DISABLE_ALL

      // Clear remaining lines
      //Display_clearLines(dispHandle, SBP_ROW_STATUS_1, SBP_ROW_STATUS_2);

      #ifdef PLUS_BROADCASTER
        // Reset flag for next connection.
        firstConnFlag = false;
      #endif // PLUS_BROADCASTER
      break;

    case GAPROLE_ERROR:
      //Display_print0(dispHandle, SBP_ROW_RESULT, 0, "Error");
      break;

    default:
      //Display_clearLines(dispHandle, SBP_ROW_RESULT, SBP_ROW_STATUS_2);
      break;
  }

}

#ifndef FEATURE_OAD_ONCHIP
/*********************************************************************
 * @fn      Movedetector_charValueChangeCB
 *
 * @brief   Callback from Simple Profile indicating a characteristic
 *          value change.
 *
 * @param   paramID - parameter ID of the value that was changed.
 *
 * @return  None.
 */
static void Movedetector_charValueChangeCB(uint8_t paramID)
{
  Movedetector_enqueueMsg(MDP_CHAR_CHANGE_EVT, paramID);
}
#endif //!FEATURE_OAD_ONCHIP

/*********************************************************************
 * @fn      Movedetector_processCharValueChangeEvt
 *
 * @brief   Process a pending Simple Profile characteristic value change
 *          event.
 *
 * @param   paramID - parameter ID of the value that was changed.
 *
 * @return  None.
 */
static void Movedetector_processCharValueChangeEvt(uint8_t paramID)
{
#ifndef FEATURE_OAD_ONCHIP
  uint8_t newValue;

  switch(paramID)
  {
    case MOVEDETECTOR_CHAR1: //SIMPLEPROFILE_CHAR1:
      Movedetector_GetParameter(MOVEDETECTOR_CHAR1, &newValue);
//      Log_print1(Diags_USER1, "CharValueChangeEvt 1 = %d\r\n", &newValue);

      switch(newValue)
      {
      case LED_STATE_OFF:
        PINCC26XX_setOutputValue(Board_BLED, LED_OFF);
//        Log_print0(Diags_USER1, "LED is OFF\r\n");
        break;

      case LED_STATE_ON:
        PINCC26XX_setOutputValue(Board_BLED, LED_ON);
//        Log_print0(Diags_USER1, "LED is ON\r\n");
        break;

      case LED_STATE_FLASH_1:
          ledBlinkCount = LED_BLINK_COUNT_1;
          PINCC26XX_setOutputValue(Board_BLED, LED_OFF);
          Util_startClock(&ledBlinkClock);
//          Log_print0(Diags_USER1, "LED is flashing in mode 1\r\n");
        break;

      default:
        PINCC26XX_setOutputValue(Board_BLED, LED_OFF); // Shirzad
//        Log_print0(Diags_USER1, "LED ERROR!!!\r\n");
        break;

      }
//      LCD_WRITE_STRING_VALUE("Char 1:", (uint16_t)newValue, 10, LCD_PAGE4);
      break;

    case MOVEDETECTOR_CHAR2: //SIMPLEPROFILE_CHAR3:
      //SimpleProfile_GetParameter(SIMPLEPROFILE_CHAR3, &newValue);
      Movedetector_GetParameter(MOVEDETECTOR_CHAR2, &newValue);
//      Log_print1(Diags_USER1, "CharValueChangeEvt 2 = %d\r\n", &newValue);
//      Log_print0(Diags_USER1, "CharValueChangeEvt 2\r\n");
//      LCD_WRITE_STRING_VALUE("Char 3:", (uint16_t)newValue, 10, LCD_PAGE4);
      break;

    case MOVEDETECTOR_CHAR3:
      Start_Alarm();
//      Log_print0(Diags_USER1, "CharValueChangeEvt 3\r\n");
      break;

    default:
      // should not reach here!
      break;
  }
#endif //!FEATURE_OAD_ONCHIP
}

/*********************************************************************
 * @fn      Movedetector_performPeriodicTask
 *
 * @brief   Perform a periodic application task. This function gets called
 *          every five seconds (SBP_PERIODIC_EVT_PERIOD). In this example,
 *          the value of the third characteristic in the SimpleGATTProfile
 *          service is retrieved from the profile, and then copied into the
 *          value of the the fourth characteristic.
 *
 * @param   None.
 *
 * @return  None.
 */
static void Movedetector_performPeriodicTask(void)
{
#ifndef FEATURE_OAD_ONCHIP
  uint8_t valueToCopy;
//  uint8_t tTxBuf[] = "Hello World";    // Transmit buffer
//  uint8_t bVal = PINCC26XX_getOutputValue(Board_SPI0_CLK);
//  PINCC26XX_setOutputValue(Board_SPI0_CLK, !bVal);
  //PINCC26XX_setOutputValue(Board_BLED, LED_OFF);
//  Toggle_led();
  // Call to retrieve the value of the third characteristic in the profile
//  TransportTxBuf = tTxBuf;
//  Write_Hello();
//  TLwrite (tTxBuf, sizeof(tTxBuf));
//
  if(1) //(tempMem == 1)
  {
      RPrintf("Hola Hola Hola Hola\r\n");
      //Toggle_led();
  }
  if (Movedetector_GetParameter(MOVEDETECTOR_CHAR1, &valueToCopy) == SUCCESS)
  {
    // Call to set that value of the fourth characteristic in the profile.
    // Note that if notifications of the fourth characteristic have been
    // enabled by a GATT client device, then a notification will be sent
    // every time this function is called.
    //SimpleProfile_SetParameter(SIMPLEPROFILE_CHAR4, sizeof(uint8_t), &valueToCopy);
  }
#endif //!FEATURE_OAD_ONCHIP
}


#ifdef FEATURE_OAD
/*********************************************************************
 * @fn      Movedetector_processOadWriteCB
 *
 * @brief   Process a write request to the OAD profile.
 *
 * @param   event      - event type:
 *                       OAD_WRITE_IDENTIFY_REQ
 *                       OAD_WRITE_BLOCK_REQ
 * @param   connHandle - the connection Handle this request is from.
 * @param   pData      - pointer to data for processing and/or storing.
 *
 * @return  None.
 */
void Movedetector_processOadWriteCB(uint8_t event, uint16_t connHandle,
                                           uint8_t *pData)
{
  oadTargetWrite_t *oadWriteEvt = ICall_malloc( sizeof(oadTargetWrite_t) + \
                                             sizeof(uint8_t) * OAD_PACKET_SIZE);

  if ( oadWriteEvt != NULL )
  {
    oadWriteEvt->event = event;
    oadWriteEvt->connHandle = connHandle;

    oadWriteEvt->pData = (uint8_t *)(&oadWriteEvt->pData + 1);
    memcpy(oadWriteEvt->pData, pData, OAD_PACKET_SIZE);

    Queue_put(hOadQ, (Queue_Elem *)oadWriteEvt);

    // Post the application's event.  For OAD, no event flag is used.
    Event_post(syncEvent, SBP_QUEUE_PING_EVT);
  }
  else
  {
    // Fail silently.
  }
}
#endif //FEATURE_OAD

/*********************************************************************
 * @fn      Movedetector_clockHandler
 *
 * @brief   Handler function for clock timeouts.
 *
 * @param   arg - event type
 *
 * @return  None.
 */
static void Movedetector_clockHandler(UArg arg)
{
  // Wake up the application.
  Event_post(syncEvent, arg);
}

//#if !defined(Display_DISABLE_ALL)
/*********************************************************************
 * @fn      Movedetector_keyChangeHandler
 *
 * @brief   Key event handler function
 *
 * @param   keys - bitmap of pressed keys
 *
 * @return  none
 */
//void Movedetector_keyChangeHandler(uint8 keys)
//{
//  Movedetector_enqueueMsg(MDP_KEY_CHANGE_EVT, keys);
//}
//#endif  // !Display_DISABLE_ALL

/*********************************************************************
 * @fn      Movedetector_enqueueMsg
 *
 * @brief   Creates a message and puts the message in RTOS queue.
 *
 * @param   event - message event.
 * @param   state - message state.
 *
 * @return  None.
 */
static void Movedetector_enqueueMsg(uint8_t event, uint8_t state)
{
  sbpEvt_t *pMsg;

  // Create dynamic pointer to message.
  if ((pMsg = ICall_malloc(sizeof(sbpEvt_t))))
  {
    pMsg->hdr.event = event;
    pMsg->hdr.state = state;

    // Enqueue the message.
    Util_enqueueMsg(appMsgQueue, syncEvent, (uint8*)pMsg);
  }
}


//#if !defined(Display_DISABLE_ALL)
/*********************************************************************
 * @fn      Movedetector_handleKeys
 *
 * @brief   Handles all key events for this device.
 *
 * @param   keys - bit field for key events. Valid entries:
 *                 KEY_LEFT
 *                 KEY_RIGHT
 *
 * @return  none
 */

//static void Movedetector_handleKeys(uint8_t keys)
//{
/*  if (keys & KEY_LEFT)
  {
    // Check if the key is still pressed. WA for possible bouncing.
#if defined(CC2650DK_7ID)
    if (PIN_getInputValue(Board_KEY_LEFT) == 0)
#elif defined(CC2650_LAUNCHXL) || defined(CC2640R2_LAUNCHXL) || defined(SABLEXR2_DEV_BOARD)
    if (PIN_getInputValue(Board_PIN_BUTTON0) == 0)
#endif // CC2650DK_7ID, CC2650_LAUNCHXL, CC2640R2_LAUNCHXL
    {
      tbm_buttonLeft();
    }
  }
  else if (keys & KEY_RIGHT)
  {
    // Check if the key is still pressed. WA for possible bouncing.
#if defined(CC2650DK_7ID)
    if (PIN_getInputValue(Board_KEY_RIGHT) == 0)
#elif defined(CC2650_LAUNCHXL) || defined(CC2640R2_LAUNCHXL) || defined(SABLEXR2_DEV_BOARD)
    if (PIN_getInputValue(Board_PIN_BUTTON1) == 0)
#endif // CC2650DK_7ID, CC2650_LAUNCHXL, CC2640R2_LAUNCHXL
    {
      tbm_buttonRight();
    }
  }
  */
//    (void)shift;  // Intentionally unreferenced parameter
//  return;
//}

/*********************************************************************
 * @fn      Movedetector_doSetPhy
 *
 * @brief   Set PHY preference.
 *
 * @param   index - 0: 1M PHY
 *                  1: 2M PHY
 *                  2: 1M + 2M PHY
 *                  3: CODED PHY (Long range) (when PHY_LR_CFG is defined)
 *                  4: 1M + 2M + CODED PHY (when PHY_LR_CFG is defined)
 *
 * @return  always true
 */
bool Movedetector_doSetPhy(uint8 index)
{
  uint8_t gapRoleState;
  uint16_t connectionHandle;
  static uint8_t phy[] = {
    HCI_PHY_1_MBPS, HCI_PHY_2_MBPS, HCI_PHY_1_MBPS | HCI_PHY_2_MBPS,

  // Note: BLE_V50_FEATURES is always defined and long range phy (PHY_LR_CFG) is
  //       defined in build_config.opt
  // To use the long range phy, HCI_PHY_CODED needs to be included
  #if (BLE_V50_FEATURES & PHY_LR_CFG)
    HCI_PHY_CODED, HCI_PHY_1_MBPS | HCI_PHY_2_MBPS | HCI_PHY_CODED,
  #endif  // PHY_LR_CFG
  };

  GAPRole_GetParameter(GAPROLE_STATE, &gapRoleState);
  GAPRole_GetParameter(GAPROLE_CONNHANDLE, &connectionHandle);

  // Set Phy Preference on the current connection. Apply the same value
  // for RX and TX.
  HCI_LE_SetPhyCmd(connectionHandle, 0, phy[index], phy[index], 0);

  Display_print1(dispHandle, SBP_ROW_RESULT, 0, "PHY preference: %s",
                 TBM_GET_ACTION_DESC(&sbpMenuMain, index));

  Display_clearLine(dispHandle, SBP_ROW_STATUS_1);

  return true;
}
//#endif  // !Display_DISABLE_ALL

/*********************************************************************
*********************************************************************/
/*********************************************************************
*********************************************************************/
/*********************************************************************
 * @fn      Movedetector_enqueueMsg
 *
 * @brief   Creates a message and puts the message in RTOS queue.
 *
 * @param   event - message event.
 * @param   state - message state.
 *
 * @return  None.
 */
/*
static void MovedetectorSensor_enqueueMsg(uint8_t event, uint8_t state)
{
  sbpEvt_t *pMsg;

  // Create dynamic pointer to message.
  if ((pMsg = ICall_malloc(sizeof(sbpEvt_t))))
  {
    pMsg->hdr.event = event;
    pMsg->hdr.state = state;

    // Enqueue the message.
    //Util_enqueueMsg(appMsgQueue, sem, (uint8*)pMsg);
    Util_enqueueMsg(appMsgQueue, syncEvent, (uint8*)pMsg);
  }
}
*/

/*********************************************************************
 * @fn      MovedetectorSensor_handleKeys
 *
 * @brief   Handles all key events for this device.
 *
 * @param   shift - true if in shift/alt.
 * @param   keys - bit field for key events. Valid entries:
 *                 HAL_KEY_SW_2
 *                 HAL_KEY_SW_1
 *
 * @return  none
 */
static void MovedetectorSensor_handleKeys(uint8_t shift, uint8_t keys)
{
  (void)shift;  // Intentionally unreferenced parameter
    LIS3DH_Filter filter_Parms;
    uint8_t temp;

    RPrintf("Testing for sure!!!\r\n");
    PINCC26XX_setOutputValue(Board_RLED, Board_LED_ON);

if (sensorCheckCount == 0) // wait for the previous interrupt routine to finish
{
  PINCC26XX_setOutputValue(Board_BLED, LED_ON);
//  Log_print0(Diags_USER1, "**\r\n");

    filter_Parms.highPassFilterIntEnable = LIS3DH_HF_FILTER_INT_NONE; //LIS3DH_HF_FILTER_INT_NONE; //LIS3DH_HF_FILTER_INT_AI1;
    filter_Parms.highPassFilterDataSel = LIS3DH_HF_FILTER_DATA_SEL_BYPASS;
    filter_Parms.highPassFilterMode = LIS3DH_HF_FILTER_MODE_NORMAL_RESET;
    filter_Parms.highPassFilterCutOffFreq = LIS3DH_HF_FILTER_CUTOFF_FREQ_3;

    /////////////////////// THIS IS TO SETUP THE HIGH PASS FILTER INTERRUPT /////////////////////////////////////////////////////////////////////////////////
    LIS3DH_SetFilter(filter_Parms);
//  LIS3DH_InterruptCtrl();
//  LIS3DH_Interrupt1Threshold(0x05); // Best way to adjust sensitivity
//  LIS3DH_Interrupt1Duration(2);// you can add duration here too
//  LIS3DH_ReadRefrence(&temp); // Dummy read to force the HP filter to current acceleration value  (i.e. set reference acceleration/tilt value)
    LIS3DH_Interrupt1Config(0x00); // Configure desired wake-up event (AOI 6D ZHIE ZLIE YHIE YLIE XHIE XLIE)

    LIS3DH_ReadINT1Source(&temp); // Return the event that has triggered the interrupt and clear interrupt
    sensorCheckCount = SENSOR_MOVE_COUNT;
  Util_startClock(&sensorMovementClock);
}
/*  uint8_t moveDetector;
  Movedetector_GetParameter(MOVEDETECTOR_CHAR1, &moveDetector);
  if (keys & KEY_UP)
  {
    if (moveDetector < 255)
    {
      moveDetector++;
      Movedetector_SetParameter(MOVEDETECTOR_CHAR1, sizeof(uint8_t),
                                   &moveDetector);
    }
  }

  if (keys & KEY_DOWN)
  {
    if (moveDetector > 0)
    {
        moveDetector--;
        Movedetector_SetParameter(MOVEDETECTOR_CHAR1, sizeof(uint8_t),
                                   &moveDetector);
    }
  }
  */
//  LCD_WRITE_STRING_VALUE("Sunlight:", (uint16_t)moveDetector, 10, LCD_PAGE5);
  return;
}

/*********************************************************************
 * @fn      MovedetectorSensor_keyChangeHandler
 *
 * @brief   Key event handler function
 *
 * @param   a0 - ignored
 *
 * @return  none
 */
void MovedetectorSensor_keyChangeHandler(uint8 keysPressed)
{
//    RPrintf("Testing for sure2!!!\r\n");
//  Movedetector_enqueueMsg(MDP_KEY_CHANGE_EVT, keysPressed); //HOLA
  Movedetector_enqueueMsg(MDP_KEY_CHANGE_EVT, keysPressed);
}


/////// Read XYZ and see if the detected movement from HPF is real (not noise) //////////////
void ReadSensorValue(void)
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

    static_xyzValue[0][sensorCheckCount] = xValue;
    static_xyzValue[1][sensorCheckCount] = yValue;
    static_xyzValue[2][sensorCheckCount] = zValue;
//  static_xValue[sensorCheckCount] = xValue;
//  static_yValue[sensorCheckCount] = yValue;
//  static_zValue[sensorCheckCount] = zValue;

    // create a periodic task to read XYZ, compare with the previous numbers and interrupt if more change then the threshold
}

static void CheckForAlarm(void)
{
    uint8_t i, j;
    uint8_t bVal;
    LIS3DH_Filter filter_Parms;
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

    if (maxDiff > ALARM_MOVEMENT_THRESHOLD)
    {
        bVal = PINCC26XX_getOutputValue(Board_GLED);
        PINCC26XX_setOutputValue(Board_GLED, !bVal);
        //PINCC26XX_setOutputValue(BOARD_LED1, LED_ON);
    }

    filter_Parms.highPassFilterIntEnable = LIS3DH_HF_FILTER_INT_AI1; //LIS3DH_HF_FILTER_INT_NONE; //LIS3DH_HF_FILTER_INT_AI1;
    filter_Parms.highPassFilterDataSel = LIS3DH_HF_FILTER_DATA_SEL_OUT;
    filter_Parms.highPassFilterMode = LIS3DH_HF_FILTER_MODE_NORMAL_RESET;
    filter_Parms.highPassFilterCutOffFreq = LIS3DH_HF_FILTER_CUTOFF_FREQ_3;

  //LIS3DH_Initialize();
  //LIS3DH_SetDeviceMode(LIS3DH_MODE_LOW_POWER, LIS3DH_ODR_25_HZ, LIS3DH_FULL_SCALE_SELECTION_4G);
  /////////////////////// THIS IS TO SETUP THE HIGH PASS FILTER INTERRUPT /////////////////////////////////////////////////////////////////////////////////
  LIS3DH_SetFilter(filter_Parms);
  LIS3DH_InterruptCtrl();
  LIS3DH_Interrupt1Threshold(0x05); // Best way to adjust sensitivity
  LIS3DH_Interrupt1Duration(2);// you can add duration here too
  LIS3DH_ReadRefrence(&bVal); // Dummy read to force the HP filter to current acceleration value  (i.e. set reference acceleration/tilt value)
  LIS3DH_Interrupt1Config(0x2A); // Configure desired wake-up event (AOI 6D ZHIE ZLIE YHIE YLIE XHIE XLIE)
}

//Define the callback function which will parse any data received from the TL. Instead of echoing, this should be modified to parse the user's custom packet format and proceed as desired.
#if defined (NPI_USE_UART) || defined (NPI_USE_SPI)
static void SimpleBLEPeripheral_TLpacketParser(void)
{
  //read available bytes
  uint8_t len = TLgetRxBufLen();
  if (len >= APP_TL_BUFF_SIZE)
  {
    len = APP_TL_BUFF_SIZE;
  }
  TLread(appRxBuf, len);

  // ADD PACKET PARSER HERE
  // for now we just echo...

 // TLwrite(appRxBuf, len);
}
#endif //TL

/*
 * The follow TL functions are available for the application to use (externally declared in tl.h):

void TLinit(ICall_Semaphore *pAppSem, TLCBs_t *appCallbacks, uint16_t TX_DONE_EVENT, uint16_t RX_EVENT, uint16_t MRDY_EVENT)
this function should only be called once - in the application's init function. This will pass application information to the TL and also configures and initializes the SPI / UART driver.
void TLwrite (uint8_t *buf, uint8_t len)
This function will write len amount of bytes via SPI/UART starting at memory location buf
void TLread (uint8_t *buf, uint8_t len)
This function will read len amount of bytes from SPI/UART and place them starting at memory location buf
uint16_t TLgetRxBufLen(void)
This function will return the amount of bytes available to be read from the NPI circular buffer. See the Software Architecture and Considerations section below for more information.
*/


