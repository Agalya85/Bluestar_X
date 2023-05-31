/*******************************************************************************
* Title                 :   Application Error Handling
* Filename              :   gsmSim7600.h
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

/** @file  errorhandling.h
*   @brief Application Error Handling
*
*  This is the header file for the definition(s) related to application
*  error handling
*/


/******************************************************************************
* Includes
*******************************************************************************/
/******************************************************************************
* Module Preprocessor Constants
*******************************************************************************/

#ifndef ERRORHANDLING_H_
#define ERRORHANDLING_H_

/******************************************************************************
* Module Preprocessor Macros
*******************************************************************************/

/******************************************************************************
* Module Typedefs
*******************************************************************************/
/**
* This enumeration is a list of System Error Type
*/
typedef enum{
	enmTORERRORS_TIMER7 = 0,
	enmTORERRORS_CAN1_INIT,
	enmTORERRORS_CAN1_CONFIGFILTER,
	enmTORERRORS_CAN1_START,
	enmTORERRORS_CAN1_NOTIFYFIFO0,
	enmTORERRORS_CAN1_TXERROR,
	enmTORERRORS_CAN1_RXERROR,
	enmTORERRORS_CAN2_INIT,
	enmTORERRORS_CAN2_CONFIGFILTER,
	enmTORERRORS_CAN2_START,
	enmTORERRORS_CAN2_NOTIFYFIFO0,
	enmTORERRORS_CAN2_TXERROR,
	enmTORERRORS_CAN2_RXERROR,
	enmTORERRORS_ENQUEUE,
	enmTORERRORS_MEM_I2C,
	enmTORERRORS_MEM_QUEUE,
	enmTORERRORS_LSM9DS1_I2C,
}enmSystemErrorType;

/**
* This enumeration is a list of System Error Status
*/
typedef enum
{
	enmERRORSTATE_NOERROR = 0,
	enmERRORSTATE_ACTIVE
}enmErrorStatus;
/**
* This Structure contains System Errors
*/
typedef struct
{
	uint32_t u32Timer7Error;
	uint32_t u32Can1InitError;
	uint32_t u32Can1ConfigFilterError;
	uint32_t u32Can1StartError;
	uint32_t u32Can1NotifyFIFO0Error;
	uint32_t u32Can1TxError;
	uint32_t u32Can1RxError;
	uint32_t u32Can2ConfigFilterError;
	uint32_t u32Can2StartError;
	uint32_t u32Can2NotifyFIFO0Error;
	uint32_t u32Can2TxError;
	uint32_t u32CanRxError;
	uint32_t u32EnqueueError;
	uint32_t u32MemI2CError;
	uint32_t u32MemQueueError;

	uint32_t u32UnknownError;
}strctSystemErrors;

/******************************************************************************
* Function Prototypes
*******************************************************************************/
#ifdef __cplusplus
extern "C"{
#endif

void assertError(enmSystemErrorType errorType ,enmErrorStatus errorStatus);
void processSystemErrors(void);

#ifdef __cplusplus
} // extern "C"
#endif

//***************************************** End of File ********************************************************
#endif /* ERRORHANDLING_H_ */
