/******************************************************************************

 @file       simple_gatt_profile.c

 @brief This file contains the Simple GATT profile sample GATT service profile
        for use with the BLE sample application.

 Group: CMCU, SCS
 Target Device: CC2640R2

 ******************************************************************************
 
 Copyright (c) 2010-2017, Texas Instruments Incorporated
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
#include <icall.h>
#include "util.h"
/* This Header file contains all BLE API and icall structure definition */
#include "icall_ble_api.h"

#include "simple_gatt_profile.h"

#include "alarm.h"

/*********************************************************************
 * MACROS
 */

/*********************************************************************
 * CONSTANTS
 */

#define SERVAPP_NUM_ATTR_SUPPORTED        17

/*********************************************************************
 * TYPEDEFS
 */

/*********************************************************************
 * GLOBAL VARIABLES
 */
/*
// Simple GATT Profile Service UUID: 0xFFF0
CONST uint8 simpleProfileServUUID[ATT_BT_UUID_SIZE] =
{
  LO_UINT16(SIMPLEPROFILE_SERV_UUID), HI_UINT16(SIMPLEPROFILE_SERV_UUID)
};

// Characteristic 1 UUID: 0xFFF1
CONST uint8 simpleProfilechar1UUID[ATT_BT_UUID_SIZE] =
{
  LO_UINT16(SIMPLEPROFILE_CHAR1_UUID), HI_UINT16(SIMPLEPROFILE_CHAR1_UUID)
};

// Characteristic 2 UUID: 0xFFF2
CONST uint8 simpleProfilechar2UUID[ATT_BT_UUID_SIZE] =
{
  LO_UINT16(SIMPLEPROFILE_CHAR2_UUID), HI_UINT16(SIMPLEPROFILE_CHAR2_UUID)
};

// Characteristic 3 UUID: 0xFFF3
CONST uint8 simpleProfilechar3UUID[ATT_BT_UUID_SIZE] =
{
  LO_UINT16(SIMPLEPROFILE_CHAR3_UUID), HI_UINT16(SIMPLEPROFILE_CHAR3_UUID)
};

// Characteristic 4 UUID: 0xFFF4
CONST uint8 simpleProfilechar4UUID[ATT_BT_UUID_SIZE] =
{
  LO_UINT16(SIMPLEPROFILE_CHAR4_UUID), HI_UINT16(SIMPLEPROFILE_CHAR4_UUID)
};

// Characteristic 5 UUID: 0xFFF5
CONST uint8 simpleProfilechar5UUID[ATT_BT_UUID_SIZE] =
{
  LO_UINT16(SIMPLEPROFILE_CHAR5_UUID), HI_UINT16(SIMPLEPROFILE_CHAR5_UUID)
};
*/// MoveDetector Service UUID: 0xBB00
CONST uint8 movedetectorServUUID[ATT_UUID_SIZE] =
{
    TI_BASE_UUID_128(MOVEDETECTOR_SERV_UUID),
};

// Characteristic 1 UUID: 0xBB01
CONST uint8 movedetectorchar1UUID[ATT_UUID_SIZE] =
{
    TI_BASE_UUID_128(MOVEDETECTOR_CHAR1_UUID),
};
// Characteristic 2 UUID: 0xBB02
CONST uint8 movedetectorchar2UUID[ATT_UUID_SIZE] =
{
    TI_BASE_UUID_128(MOVEDETECTOR_CHAR2_UUID),
};

// Characteristic 3 UUID: 0xBB03
CONST uint8 movedetectorchar3UUID[ATT_UUID_SIZE] =
{
    TI_BASE_UUID_128(MOVEDETECTOR_CHAR3_UUID),
};

/*********************************************************************
 * EXTERNAL VARIABLES
 */

/*********************************************************************
 * EXTERNAL FUNCTIONS
 */

/*********************************************************************
 * LOCAL VARIABLES
 */

static movedetectorCBs_t *movedetector_AppCBs = NULL;

/*********************************************************************
 * Profile Attributes - variables
 */

/*
// Simple Profile Service attribute
//static CONST gattAttrType_t simpleProfileService = { ATT_BT_UUID_SIZE, simpleProfileServUUID };
static CONST gattAttrType_t movedetectorService = { ATT_UUID_SIZE, movedetectorServUUID };


// Simple Profile Characteristic 1 Properties
//static uint8 simpleProfileChar1Props = GATT_PROP_READ | GATT_PROP_WRITE;
static uint8 movedetectorChar1Props = GATT_PROP_READ | GATT_PROP_WRITE;

// Characteristic 1 Value
static uint8 simpleProfileChar1 = 0;

// Simple Profile Characteristic 1 User Description
static uint8 simpleProfileChar1UserDesp[17] = "Characteristic 1";


// Simple Profile Characteristic 2 Properties
static uint8 simpleProfileChar2Props = GATT_PROP_READ;

// Characteristic 2 Value
static uint8 simpleProfileChar2 = 0;

// Simple Profile Characteristic 2 User Description
static uint8 simpleProfileChar2UserDesp[17] = "Characteristic 2";


// Simple Profile Characteristic 3 Properties
static uint8 simpleProfileChar3Props = GATT_PROP_WRITE;

// Characteristic 3 Value
static uint8 simpleProfileChar3 = 0;

// Simple Profile Characteristic 3 User Description
static uint8 simpleProfileChar3UserDesp[17] = "Characteristic 3";


// Simple Profile Characteristic 4 Properties
static uint8 simpleProfileChar4Props = GATT_PROP_NOTIFY;

// Characteristic 4 Value
static uint8 simpleProfileChar4 = 0;

// Simple Profile Characteristic 4 Configuration Each client has its own
// instantiation of the Client Characteristic Configuration. Reads of the
// Client Characteristic Configuration only shows the configuration for
// that client and writes only affect the configuration of that client.
static gattCharCfg_t *simpleProfileChar4Config;

// Simple Profile Characteristic 4 User Description
static uint8 simpleProfileChar4UserDesp[17] = "Characteristic 4";


// Simple Profile Characteristic 5 Properties
static uint8 simpleProfileChar5Props = GATT_PROP_READ;

// Characteristic 5 Value
static uint8 simpleProfileChar5[SIMPLEPROFILE_CHAR5_LEN] = { 0, 0, 0, 0, 0 };

// Simple Profile Characteristic 5 User Description
static uint8 simpleProfileChar5UserDesp[17] = "Characteristic 5";

*/

// SModeDetector Service attribute
static CONST gattAttrType_t movedetectorService = { ATT_UUID_SIZE, movedetectorServUUID};

// MoveDetector Service Characteristic 1 Properties
static uint8 movedetectorChar1Props = GATT_PROP_READ | GATT_PROP_WRITE;

// Characteristic 1 Value
static uint8 movedetectorChar1 = 0;

// MoveDetector Service Characteristic 1 User Description
static uint8 movedetectorChar1UserDesp[10] = "LED CTRL\0";

// MoveDetector Service Characteristic 2 Properties
static uint8 movedetectorChar2Props = GATT_PROP_NOTIFY;

// Characteristic 2 Value
static uint8 movedetectorChar2 = 22;

// MoveDetector Service Characteristic 2 Configuration.
static gattCharCfg_t *movedetectorChar2Config;

// MoveDetector Service Characteristic 2 User Description
static uint8 movedetectorChar2UserDesp[14] = "Value Notify\0";

// MoveDetector Service Characteristic 3 Properties
static uint8 movedetectorChar3Props = GATT_PROP_READ | GATT_PROP_WRITE;

// Characteristic 1 Value
static uint8 movedetectorChar3 = 0;

// MoveDetector Service Characteristic 1 User Description
static uint8 movedetectorChar3UserDesp[14] = "Sensor Value\0";

/*********************************************************************
 * Profile Attributes - Table
 */
static gattAttribute_t movedetectorAttrTbl[SERVAPP_NUM_ATTR_SUPPORTED] =
{
  // movedetector Service  (the attribute for the primary Service declaration)// UUID size is 16bit since this is a standard uuid
  {
    { ATT_BT_UUID_SIZE, primaryServiceUUID },   /* type */
    GATT_PERMIT_READ,                           /* permissions */
    0,                                          /* handle */
    (uint8 *)&movedetectorService                   /* pValue */
  },

  // Characteristic 1 Declaration
  {
    { ATT_BT_UUID_SIZE, characterUUID },
    GATT_PERMIT_READ,
    0,
    &movedetectorChar1Props
  },

  // Characteristic Value 1, this is a custom uuid
  {
    { ATT_UUID_SIZE, movedetectorchar1UUID },
    GATT_PERMIT_READ | GATT_PERMIT_WRITE,
    0,
    &movedetectorChar1
  },

  // Characteristic 1 User Description
  {
    { ATT_BT_UUID_SIZE, charUserDescUUID },
    GATT_PERMIT_READ,
    0,
    movedetectorChar1UserDesp
  },


  // Characteristic 2 Declaration
  {
    { ATT_BT_UUID_SIZE, characterUUID },
    GATT_PERMIT_READ,
    0,
    &movedetectorChar2Props
  },

  // Characteristic Value 2
  {
    { ATT_UUID_SIZE, movedetectorchar2UUID },
    0,
    0,
    &movedetectorChar2
  },

  // Characteristic 2 configuration
  {
    { ATT_BT_UUID_SIZE, clientCharCfgUUID },
    GATT_PERMIT_READ | GATT_PERMIT_WRITE,
    0,
    (uint8 *)&movedetectorChar2Config
  },

  // Characteristic 2 User Description
  {
    { ATT_BT_UUID_SIZE, charUserDescUUID },
    GATT_PERMIT_READ,
    0,
    movedetectorChar2UserDesp
  },

  // Characteristic 3 Declaration
  {
    { ATT_BT_UUID_SIZE, characterUUID },
    GATT_PERMIT_READ,
    0,
    &movedetectorChar3Props
  },

  // Characteristic Value 3, this is a custom uuid
  {
    { ATT_UUID_SIZE, movedetectorchar3UUID },
    GATT_PERMIT_READ | GATT_PERMIT_WRITE,
    0,
    &movedetectorChar3
  },

  // Characteristic 3 User Description
  {
    { ATT_BT_UUID_SIZE, charUserDescUUID },
    GATT_PERMIT_READ,
    0,
    movedetectorChar3UserDesp
  },
};
/*
static gattAttribute_t simpleProfileAttrTbl[SERVAPP_NUM_ATTR_SUPPORTED] =
{
  // Simple Profile Service
  {

// //   { ATT_BT_UUID_SIZE, primaryServiceUUID },

//    GATT_PERMIT_READ,                         /* permissions */
//    0,                                        /* handle */
//    (uint8 *)&simpleProfileService            /* pValue */
/*  },

    // Characteristic 1 Declaration
    {
      { ATT_BT_UUID_SIZE, characterUUID },
      GATT_PERMIT_READ,
      0,
      &simpleProfileChar1Props
    },

      // Characteristic Value 1
      {
        { ATT_BT_UUID_SIZE, simpleProfilechar1UUID },
        GATT_PERMIT_READ | GATT_PERMIT_WRITE,
        0,
        &simpleProfileChar1
      },

      // Characteristic 1 User Description
      {
        { ATT_BT_UUID_SIZE, charUserDescUUID },
        GATT_PERMIT_READ,
        0,
        simpleProfileChar1UserDesp
      },

    // Characteristic 2 Declaration
    {
      { ATT_BT_UUID_SIZE, characterUUID },
      GATT_PERMIT_READ,
      0,
      &simpleProfileChar2Props
    },

      // Characteristic Value 2
      {
        { ATT_BT_UUID_SIZE, simpleProfilechar2UUID },
        GATT_PERMIT_READ,
        0,
        &simpleProfileChar2
      },

      // Characteristic 2 User Description
      {
        { ATT_BT_UUID_SIZE, charUserDescUUID },
        GATT_PERMIT_READ,
        0,
        simpleProfileChar2UserDesp
      },

    // Characteristic 3 Declaration
    {
      { ATT_BT_UUID_SIZE, characterUUID },
      GATT_PERMIT_READ,
      0,
      &simpleProfileChar3Props
    },

      // Characteristic Value 3
      {
        { ATT_BT_UUID_SIZE, simpleProfilechar3UUID },
        GATT_PERMIT_WRITE,
        0,
        &simpleProfileChar3
      },

      // Characteristic 3 User Description
      {
        { ATT_BT_UUID_SIZE, charUserDescUUID },
        GATT_PERMIT_READ,
        0,
        simpleProfileChar3UserDesp
      },

    // Characteristic 4 Declaration
    {
      { ATT_BT_UUID_SIZE, characterUUID },
      GATT_PERMIT_READ,
      0,
      &simpleProfileChar4Props
    },

      // Characteristic Value 4
      {
        { ATT_BT_UUID_SIZE, simpleProfilechar4UUID },
        0,
        0,
        &simpleProfileChar4
      },

      // Characteristic 4 configuration
      {
        { ATT_BT_UUID_SIZE, clientCharCfgUUID },
        GATT_PERMIT_READ | GATT_PERMIT_WRITE,
        0,
        (uint8 *)&simpleProfileChar4Config
      },

      // Characteristic 4 User Description
      {
        { ATT_BT_UUID_SIZE, charUserDescUUID },
        GATT_PERMIT_READ,
        0,
        simpleProfileChar4UserDesp
      },

    // Characteristic 5 Declaration
    {
      { ATT_BT_UUID_SIZE, characterUUID },
      GATT_PERMIT_READ,
      0,
      &simpleProfileChar5Props
    },

      // Characteristic Value 5
      {
        { ATT_BT_UUID_SIZE, simpleProfilechar5UUID },
        GATT_PERMIT_AUTHEN_READ,
        0,
        simpleProfileChar5
      },

      // Characteristic 5 User Description
      {
        { ATT_BT_UUID_SIZE, charUserDescUUID },
        GATT_PERMIT_READ,
        0,
        simpleProfileChar5UserDesp
      },
};
*/

bStatus_t utilExtractUuid16(gattAttribute_t *pAttr, uint16_t *pUuid)
{
  bStatus_t status = SUCCESS;

  if (pAttr->type.len == ATT_BT_UUID_SIZE )
  {
    // 16-bit UUID direct
    *pUuid = BUILD_UINT16( pAttr->type.uuid[0], pAttr->type.uuid[1]);
#ifdef GATT_TI_UUID_128_BIT
  }
  else if (pAttr->type.len == ATT_UUID_SIZE)
  {
    // 16-bit UUID extracted bytes 12 and 13
    *pUuid = BUILD_UINT16( pAttr->type.uuid[12], pAttr->type.uuid[13]);
#endif
  } else {
    *pUuid = 0xFFFF;
    status = FAILURE;
  }

  return status;
}

/*********************************************************************
 * LOCAL FUNCTIONS
 */

static bStatus_t movedetector_ReadAttrCB(uint16_t connHandle,
                                          gattAttribute_t *pAttr,
                                          uint8_t *pValue, uint16_t *pLen,
                                          uint16_t offset, uint16_t maxLen,
                                          uint8_t method);
static bStatus_t movedetector_WriteAttrCB(uint16_t connHandle,
                                           gattAttribute_t *pAttr,
                                           uint8_t *pValue, uint16_t len,
                                           uint16_t offset, uint8_t method);

/*
static bStatus_t movedetector_ReadAttrCB(uint16_t connHandle,
                                          gattAttribute_t *pAttr,
                                          uint8_t *pValue, uint16_t *pLen,
                                          uint16_t offset, uint16_t maxLen,
                                          uint8_t method);
static bStatus_t movedetector_WriteAttrCB(uint16_t connHandle,
                                           gattAttribute_t *pAttr,
                                           uint8_t *pValue, uint16_t len,
                                           uint16_t offset, uint8_t method);
*/
/*********************************************************************
 * PROFILE CALLBACKS
 */

// Simple Profile Service Callbacks
// Note: When an operation on a characteristic requires authorization and
// pfnAuthorizeAttrCB is not defined for that characteristic's service, the
// Stack will report a status of ATT_ERR_UNLIKELY to the client.  When an
// operation on a characteristic requires authorization the Stack will call
// pfnAuthorizeAttrCB to check a client's authorization prior to calling
// pfnReadAttrCB or pfnWriteAttrCB, so no checks for authorization need to be
// made within these functions.
/*CONST gattServiceCBs_t simpleProfileCBs =
{
  simpleProfile_ReadAttrCB,  // Read callback function pointer
  simpleProfile_WriteAttrCB, // Write callback function pointer
  NULL                       // Authorization callback function pointer
};
*/
CONST gattServiceCBs_t movedetectorCBs =
{
  movedetector_ReadAttrCB,  // Read callback function pointer
  movedetector_WriteAttrCB, // Write callback function pointer
  NULL                       // Authorization callback function pointer
};

/*********************************************************************
 * PUBLIC FUNCTIONS
 */

/*********************************************************************
 * @fn      SimpleProfile_AddService
 *
 * @brief   Initializes the Simple Profile service by registering
 *          GATT attributes with the GATT server.
 *
 * @param   services - services to add. This is a bit map and can
 *                     contain more than one service.
 *
 * @return  Success or Failure
 */
/*bStatus_t SimpleProfile_AddService( uint32 services )
{
  uint8 status;

  // Allocate Client Characteristic Configuration table
  simpleProfileChar4Config = (gattCharCfg_t *)ICall_malloc( sizeof(gattCharCfg_t) *
                                                            linkDBNumConns );
  if ( simpleProfileChar4Config == NULL )
  {
    return ( bleMemAllocError );
  }

  // Initialize Client Characteristic Configuration attributes
  GATTServApp_InitCharCfg( INVALID_CONNHANDLE, simpleProfileChar4Config );

  if ( services & SIMPLEPROFILE_SERVICE )
  {
    // Register GATT attribute list and CBs with GATT Server App
    status = GATTServApp_RegisterService( simpleProfileAttrTbl,
                                          GATT_NUM_ATTRS( simpleProfileAttrTbl ),
                                          GATT_MAX_ENCRYPT_KEY_SIZE,
                                          &simpleProfileCBs );
  }
  else
  {
    status = SUCCESS;
  }

  return ( status );
}
*/
bStatus_t Movedetector_AddService( uint32 services )
{
  uint8 status;

  // Allocate Client Characteristic Configuration table
  movedetectorChar2Config = (gattCharCfg_t *)ICall_malloc( sizeof(gattCharCfg_t) *
                                                            linkDBNumConns );
  if ( movedetectorChar2Config == NULL )
  {
    return ( bleMemAllocError );
  }

  // Initialize Client Characteristic Configuration attributes
  GATTServApp_InitCharCfg( INVALID_CONNHANDLE, movedetectorChar2Config );

  // Register GATT attribute list and CBs with GATT Server App
  status = GATTServApp_RegisterService( movedetectorAttrTbl,
                                        GATT_NUM_ATTRS( movedetectorAttrTbl ),
                                        GATT_MAX_ENCRYPT_KEY_SIZE,
                                        &movedetectorCBs );

  return ( status );
}
/*********************************************************************
 * @fn      SimpleProfile_RegisterAppCBs
 *
 * @brief   Registers the application callback function. Only call
 *          this function once.
 *
 * @param   callbacks - pointer to application callbacks.
 *
 * @return  SUCCESS or bleAlreadyInRequestedMode
 */
/*bStatus_t SimpleProfile_RegisterAppCBs( simpleProfileCBs_t *appCallbacks )
{
  if ( appCallbacks )
  {
    simpleProfile_AppCBs = appCallbacks;

    return ( SUCCESS );
  }
  else
  {
    return ( bleAlreadyInRequestedMode );
  }
}*/
bStatus_t Movedetector_RegisterAppCBs( movedetectorCBs_t *appCallbacks )
{
  if ( appCallbacks )
  {
    movedetector_AppCBs = appCallbacks;

    return ( SUCCESS );
  }
  else
  {
    return ( bleAlreadyInRequestedMode );
  }
}

/*********************************************************************
 * @fn      SimpleProfile_SetParameter
 *
 * @brief   Set a Simple Profile parameter.
 *
 * @param   param - Profile parameter ID
 * @param   len - length of data to write
 * @param   value - pointer to data to write.  This is dependent on
 *          the parameter ID and WILL be cast to the appropriate
 *          data type (example: data type of uint16 will be cast to
 *          uint16 pointer).
 *
 * @return  bStatus_t
 */
/*bStatus_t Movedetector_SetParameter( uint8 param, uint8 len, void *value )
{
    bStatus_t ret = SUCCESS;
    switch ( param )
    {
      case MOVEDETECTOR_CHAR1:
        if ( len == sizeof ( uint8 ) )
        {
          movedetectorChar1 = *((uint8*)value);
        }
        else
        {
          ret = bleInvalidRange;
        }
        break;

      case MOVEDETECTOR_CHAR2:
        if ( len == sizeof ( uint8 ) )
        {
          movedetectorChar2 = *((uint8*)value);

          // See if Notification has been enabled
          GATTServApp_ProcessCharCfg( movedetectorChar2Config, &movedetectorChar2, FALSE,
                                      movedetectorAttrTbl, GATT_NUM_ATTRS( movedetectorAttrTbl ),
                                      INVALID_TASK_ID, movedetector_ReadAttrCB );
        }
        else
        {
          ret = bleInvalidRange;
        }
        break;

      default:
        ret = INVALIDPARAMETER;
        break;
    }

    return ( ret );
}
*/
bStatus_t Movedetector_SetParameter( uint8 param, uint8 len, void *value )
{
 bStatus_t ret = SUCCESS;
 switch ( param )
 {
   case MOVEDETECTOR_CHAR1:
     if ( len == sizeof ( uint8 ) )
     {
       movedetectorChar1 = *((uint8*)value);
     }
     else
     {
       ret = bleInvalidRange;
     }
     break;

   case MOVEDETECTOR_CHAR2:
     if ( len == sizeof ( uint8 ) )
     {
       movedetectorChar2 = *((uint8*)value);

       // See if Notification has been enabled
       GATTServApp_ProcessCharCfg( movedetectorChar2Config, &movedetectorChar2, FALSE,
                                   movedetectorAttrTbl, GATT_NUM_ATTRS( movedetectorAttrTbl ),
                                   INVALID_TASK_ID, movedetector_ReadAttrCB );
     }
     else
     {
       ret = bleInvalidRange;
     }
     break;

   default:
     ret = INVALIDPARAMETER;
     break;
 }

 return ( ret );
}
/*********************************************************************
 * @fn      Movedetector_GetParameter
 *
 * @brief   Get a Simple Profile parameter.
 *
 * @param   param - Profile parameter ID
 * @param   value - pointer to data to put.  This is dependent on
 *          the parameter ID and WILL be cast to the appropriate
 *          data type (example: data type of uint16 will be cast to
 *          uint16 pointer).
 *
 * @return  bStatus_t
 */
/*bStatus_t Movedetector_GetParameter( uint8 param, void *value )
{
    bStatus_t ret = SUCCESS;
    switch ( param )
    {
      case MOVEDETECTOR_CHAR1:
        *((uint8*)value) = movedetectorChar1;
        break;

      case MOVEDETECTOR_CHAR2:
        *((uint8*)value) = movedetectorChar2;
        break;

      default:
        ret = INVALIDPARAMETER;
        break;
    }

    return ( ret );
}
*/
bStatus_t  Movedetector_GetParameter( uint8 param, void *value )
{
  bStatus_t ret = SUCCESS;
  switch ( param )
  {
    case MOVEDETECTOR_CHAR1:
      *((uint8*)value) = movedetectorChar1;
      break;

    case MOVEDETECTOR_CHAR2:
      *((uint8*)value) = movedetectorChar2;
      break;

    default:
      ret = INVALIDPARAMETER;
      break;
  }

  return ( ret );
}
/*********************************************************************
 * @fn          simpleProfile_ReadAttrCB
 *
 * @brief       Read an attribute.
 *
 * @param       connHandle - connection message was received on
 * @param       pAttr - pointer to attribute
 * @param       pValue - pointer to data to be read
 * @param       pLen - length of data to be read
 * @param       offset - offset of the first octet to be read
 * @param       maxLen - maximum length of data to be read
 * @param       method - type of read message
 *
 * @return      SUCCESS, blePending or Failure
 */
/*static bStatus_t simpleProfile_ReadAttrCB(uint16_t connHandle,
                                          gattAttribute_t *pAttr,
                                          uint8_t *pValue, uint16_t *pLen,
                                          uint16_t offset, uint16_t maxLen,
                                          uint8_t method)
{
  bStatus_t status = SUCCESS;

  // Make sure it's not a blob operation (no attributes in the profile are long)
  if ( offset > 0 )
  {
    return ( ATT_ERR_ATTR_NOT_LONG );
  }

  if ( pAttr->type.len == ATT_BT_UUID_SIZE )
  {
    // 16-bit UUID
    uint16 uuid = BUILD_UINT16( pAttr->type.uuid[0], pAttr->type.uuid[1]);
    switch ( uuid )
    {
      // No need for "GATT_SERVICE_UUID" or "GATT_CLIENT_CHAR_CFG_UUID" cases;
      // gattserverapp handles those reads

      // characteristics 1 and 2 have read permissions
      // characteritisc 3 does not have read permissions; therefore it is not
      //   included here
      // characteristic 4 does not have read permissions, but because it
      //   can be sent as a notification, it is included here
      case SIMPLEPROFILE_CHAR1_UUID:
      case SIMPLEPROFILE_CHAR2_UUID:
      case SIMPLEPROFILE_CHAR4_UUID:
        *pLen = 1;
        pValue[0] = *pAttr->pValue;
        break;

      case SIMPLEPROFILE_CHAR5_UUID:
        *pLen = SIMPLEPROFILE_CHAR5_LEN;
        VOID memcpy( pValue, pAttr->pValue, SIMPLEPROFILE_CHAR5_LEN );
        break;

      default:
        // Should never get here! (characteristics 3 and 4 do not have read permissions)
        *pLen = 0;
        status = ATT_ERR_ATTR_NOT_FOUND;
        break;
    }
  }
  else
  {
    // 128-bit UUID
    *pLen = 0;
    status = ATT_ERR_INVALID_HANDLE;
  }

  return ( status );
}
*/
static bStatus_t movedetector_ReadAttrCB(uint16_t connHandle,
                                          gattAttribute_t *pAttr,
                                          uint8_t *pValue, uint16_t *pLen,
                                          uint16_t offset, uint16_t maxLen,
                                          uint8_t method)
{
  uint16 uuid;
  bStatus_t status = SUCCESS;

  uint8 valueToCopy;

/*
  // If attribute permissions require authorization to read, return error
  if ( gattPermitAuthorRead( pAttr->permissions ) )
  {
    // Insufficient authorization
    return ( ATT_ERR_INSUFFICIENT_AUTHOR );
  }
*/

  // Make sure it's not a blob operation (no attributes in the profile are long)
  if ( offset > 0 )
  {
    return ( ATT_ERR_ATTR_NOT_LONG );
  }

  if (utilExtractUuid16(pAttr,&uuid) == FAILURE) {
    // Invalid handle
    *pLen = 0;
    return ATT_ERR_INVALID_HANDLE;
  }

  switch ( uuid )
  {
    // No need for "GATT_SERVICE_UUID" or "GATT_CLIENT_CHAR_CFG_UUID" cases;
    // gattserverapp handles those reads

    // characteristics 1 has read permissions
    // characteristic 2 does not have read permissions, but because it
    //   can be sent as a notification, it is included here
  case MOVEDETECTOR_CHAR1_UUID:
      if (Movedetector_GetParameter(MOVEDETECTOR_CHAR1, &valueToCopy) == SUCCESS)
      {
          *pLen = 1;
          pValue[0] = valueToCopy;
      }
    break;
  case MOVEDETECTOR_CHAR2_UUID:
    *pLen = 1;
    pValue[0] = *pAttr->pValue;
    break;
  case MOVEDETECTOR_CHAR3_UUID:
     valueToCopy = Alarm_GetSetting();
    *pLen = 1;
    pValue[0] = valueToCopy;
    break;

  default:
    // Should never get here! (characteristics 3 and 4 do not have read permissions)
    *pLen = 0;
    status = ATT_ERR_ATTR_NOT_FOUND;
    break;
  }
  return ( status );
}
/*********************************************************************
 * @fn      simpleProfile_WriteAttrCB
 *
 * @brief   Validate attribute data prior to a write operation
 *
 * @param   connHandle - connection message was received on
 * @param   pAttr - pointer to attribute
 * @param   pValue - pointer to data to be written
 * @param   len - length of data
 * @param   offset - offset of the first octet to be written
 * @param   method - type of write message
 *
 * @return  SUCCESS, blePending or Failure
 */
/*static bStatus_t simpleProfile_WriteAttrCB(uint16_t connHandle,
                                           gattAttribute_t *pAttr,
                                           uint8_t *pValue, uint16_t len,
                                           uint16_t offset, uint8_t method)
{
  bStatus_t status = SUCCESS;
  uint8 notifyApp = 0xFF;

  if ( pAttr->type.len == ATT_BT_UUID_SIZE )
  {
    // 16-bit UUID
    uint16 uuid = BUILD_UINT16( pAttr->type.uuid[0], pAttr->type.uuid[1]);
    switch ( uuid )
    {
      case SIMPLEPROFILE_CHAR1_UUID:
      case SIMPLEPROFILE_CHAR3_UUID:

        //Validate the value
        // Make sure it's not a blob oper
        if ( offset == 0 )
        {
          if ( len != 1 )
          {
            status = ATT_ERR_INVALID_VALUE_SIZE;
          }
        }
        else
        {
          status = ATT_ERR_ATTR_NOT_LONG;
        }

        //Write the value
        if ( status == SUCCESS )
        {
          uint8 *pCurValue = (uint8 *)pAttr->pValue;
          *pCurValue = pValue[0];

          if( pAttr->pValue == &simpleProfileChar1 )
          {
            notifyApp = SIMPLEPROFILE_CHAR1;
          }
          else
          {
            notifyApp = SIMPLEPROFILE_CHAR3;
          }
        }

        break;

      case GATT_CLIENT_CHAR_CFG_UUID:
        status = GATTServApp_ProcessCCCWriteReq( connHandle, pAttr, pValue, len,
                                                 offset, GATT_CLIENT_CFG_NOTIFY );
        break;

      default:
        // Should never get here! (characteristics 2 and 4 do not have write permissions)
        status = ATT_ERR_ATTR_NOT_FOUND;
        break;
    }
  }
  else
  {
    // 128-bit UUID
    status = ATT_ERR_INVALID_HANDLE;
  }

  // If a characteristic value changed then callback function to notify application of change
  if ( (notifyApp != 0xFF ) && simpleProfile_AppCBs && simpleProfile_AppCBs->pfnSimpleProfileChange )
  {
    simpleProfile_AppCBs->pfnSimpleProfileChange( notifyApp );
  }

  return ( status );
}
*/
static bStatus_t movedetector_WriteAttrCB(uint16_t connHandle,
                                           gattAttribute_t *pAttr,
                                           uint8_t *pValue, uint16_t len,
                                           uint16_t offset, uint8_t method)
{
  uint16 uuid;
  bStatus_t status = SUCCESS;
  uint8_t notifyApp = 0xFF;

  // If attribute permissions require authorization to write, return error
  // you have to define a call back function if you want to check for authorization, it's Null right now, same place you defined _WriteAttrCB and _ReadAttrCB
/*  if ( gattPermitAuthorWrite( pAttr->permissions ) )
  {
    // Insufficient authorization
    return ( ATT_ERR_INSUFFICIENT_AUTHOR );
  }
*/

  if (utilExtractUuid16(pAttr,&uuid) == FAILURE) {
    // Invalid handle
    return ATT_ERR_INVALID_HANDLE;
  }

  // 16-bit UUID
  // uint16 uuid = BUILD_UINT16( pAttr->type.uuid[0], pAttr->type.uuid[1]);
  switch ( uuid )
  {
  case MOVEDETECTOR_CHAR1_UUID:
    //Validate the value
    // Make sure it's not a blob oper
    if ( offset == 0 )
    {
      if ( len != 1 )
      {
        status = ATT_ERR_INVALID_VALUE_SIZE;
      }
    }
    else
    {
      status = ATT_ERR_ATTR_NOT_LONG;
    }
    //Write the value
    if ( status == SUCCESS )
    {
        uint8 major = pValue[0];
/*      uint8 major[] = {
        pValue[0],pValue[1],pValue[2],pValue[3],pValue[4],pValue[5],pValue[6],pValue[7],
        pValue[8],pValue[9],pValue[10],pValue[11],pValue[12],pValue[13],pValue[14],pValue[15]
      };
      VOID osal_snv_write(MAJOR_ID, MAJOR_LEN, &major);
*/
      notifyApp = MOVEDETECTOR_CHAR1;
      Movedetector_SetParameter( MOVEDETECTOR_CHAR1, sizeof(uint8_t), &major );
      /*VOID osal_memcpy( pValue, pAttr->pValue, SIMPLEPROFILE_CHAR2_LEN );
      if( pAttr->pValue == simpleProfileChar2 )
      {
        notifyApp = SIMPLEPROFILE_CHAR2;
      }*/
    }

    break;
  case MOVEDETECTOR_CHAR3_UUID:
    //Validate the value
    // Make sure it's not a blob oper
    if ( offset == 0 )
    {
      if ( len != 1 )
      {
        status = ATT_ERR_INVALID_VALUE_SIZE;
      }
    }
    else
    {
      status = ATT_ERR_ATTR_NOT_LONG;
    }
    //Write the value
    if ( status == SUCCESS )
    {
        uint8 major = pValue[0];
/*      uint8 major[] = {
        pValue[0],pValue[1],pValue[2],pValue[3],pValue[4],pValue[5],pValue[6],pValue[7],
        pValue[8],pValue[9],pValue[10],pValue[11],pValue[12],pValue[13],pValue[14],pValue[15]
      };
      VOID osal_snv_write(MAJOR_ID, MAJOR_LEN, &major);
*/
      notifyApp = MOVEDETECTOR_CHAR3;
      Alarm_SetSetting(major);
    }

    break;

  case GATT_CLIENT_CHAR_CFG_UUID:
    status = GATTServApp_ProcessCCCWriteReq( connHandle, pAttr, pValue, len,
                                            offset, GATT_CLIENT_CFG_NOTIFY );
    break;

  default:
    // Should never get here! (characteristics 2 and 4 do not have write permissions)
    status = ATT_ERR_ATTR_NOT_FOUND;
    break;
  }


  // If a charactersitic value changed then callback function to notify application of change
  if ((notifyApp != 0xFF ) &&  movedetector_AppCBs && movedetector_AppCBs->pfnMovedetectorChange ) //
  {
      movedetector_AppCBs->pfnMovedetectorChange( notifyApp ); //
  }

  return ( status );
}
/*********************************************************************
*********************************************************************/
