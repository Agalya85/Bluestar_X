/*******************************************************************************
* Title                 :   Modbus Client Implementation - RS485 Interface
* Filename              :   modbus_rs485.h
* Author                :   Hrishikesh Limaye
* Origin Date           :   08/03/2022
* Version               :   1.0.0
* Compiler              :
* Target                :   STM32F437VITx - Tor4GEth
* Notes                 :   None
*
* Copyright (c) by KloudQ Technologies Limited.

  This software is copyrighted by and is the sole property of KloudQ
  Technologies Limited.
  All rights, title, ownership, or other interests in the software remain the
  property of  KloudQ Technologies Limited. This software may only be used in
  accordance with the corresponding license agreement. Any unauthorized use,
  duplication, transmission, distribution, or disclosure of this software is
  expressly forbidden.

  This Copyright notice may not be removed or modified without prior written
  consent of KloudQ Technologies Limited.

  KloudQ Technologies Limited reserves the right to modify this software
  without notice.
*
*
*******************************************************************************/


/*************** FILE REVISION LOG *****************************************
*
*    Date    Version   Author         	   Description
*  08/03/22   1.0.0    Hrishikesh Limaye   Initial Release.
*******************************************************************************/

/** @file modbus_rs485.h
*  @brief Modbus RTU Client Implementation
*
*  This is the header file for the definition(s) related to Modbus RTU Client
*  Implementation
*/

#ifndef MODBUS_RS485_H_
#define MODBUS_RS485_H_

/******************************************************************************
* Includes
*******************************************************************************/
/******************************************************************************
* Module Preprocessor Constants
*******************************************************************************/
#include "stdint.h"
/**
* \brief Defines dereferencing for MB address \a .
  \param None.
  \returns None \a .
*/
#define MB_ADDRESS_DEREF (1)
/**
* \brief Defines payload radix MB data . 10- dec, 16-hex , 2 - bin \a .
  \param None.
  \returns None \a .
*/
#define MB_PAYLOAD_RADIX (10)
/**
* \brief Defines dereferencing for MB address \a .
  \param None.
  \returns None \a .
*/
#define MB_485_QUERY_RETRY_ATTEMPTS (3)

/******************************************************************************
* Module Preprocessor Macros
*******************************************************************************/

/******************************************************************************
* Module Typedefs
*******************************************************************************/
/**
* This Structure contains modbus master details
*/
typedef struct
{
	uint8_t u8MBSlave_Address; 					/* Slave Device Address */
	uint8_t u8QueryFrame[8]; 					/* Modbus Query TX buffer */
	int32_t u8SlaveResponseArray[260];			/* Modbus Slave Response Array */
	uint8_t u8FunctionCodeArray[260];			/* Modbus Slave Response Array */
	uint8_t u8MBResponseCharacterCounter;		/* Counts received characters */
	uint8_t u8MBOperationStatus;				/* Tracks Slave Data Completion 0:-> Not Started 1: In process 2: Completed */
	uint8_t u8MBNoQueryAttempts;
	uint8_t u8MBResponseStatus[260];			/* Stores Response status : 0 : Success 1: Exception*/

	uint16_t u16MBStartAddress;  				/* Starting Address*/
	uint16_t u16MBNoPoints[260];       				/* No of Points */
	uint16_t u16NoRegisterstoFetch;

	uint16_t u16MBAddressArray[260];   			/* Slave Register Address Map */
	uint32_t u32SlaveData[260];					/* Holds Temperory Data Received From Slave*/
	uint32_t u32PollDelay;
}strctModbusMaster;

/******************************************************************************
* Function Prototypes
*******************************************************************************/
#ifdef __cplusplus
extern "C"{
#endif

void resetModbusPort();
uint16_t modbusCRC16(uint8_t * data , uint8_t length);
void updateModbusQueryFrameRTU(strctModbusMaster *master);
void modbusQueryFSM(void);
void uploadSlaveData(void);
void setupModbusRTU(void);

#ifdef __cplusplus
} // extern "C"
#endif

#endif /* MODBUS_RS485_H_ */

//***************************************** End of File ********************************************************
