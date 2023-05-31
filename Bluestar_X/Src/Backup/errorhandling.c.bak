/*******************************************************************************
* Title                 :   Application Error Handling
* Filename              :   errorhandling.c
* Author                :   Hrishikesh Limaye
* Origin Date           :   8/3/2022
* Version               :   1.0.0
* Compiler              :
* Target                :   STM32F437 - Tor4Eth
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
*    Date    Version   Author         	  Description
*  08/03/22   1.0.0    Hrishikesh Limaye   Initial Release.
*
*******************************************************************************/

/** @file  errorhandling.c
 *  @brief Application Error Handling Functions
 */

/******************************************************************************
* Includes
*******************************************************************************/
#include "main.h"
#include "applicationdefines.h"
#include "externs.h"
#include "errorhandling.h"
/******************************************************************************
* Module Preprocessor Constants
*******************************************************************************/
/**
 * Doxygen tag for documenting variables and constants
 */

/******************************************************************************
* Module Preprocessor Macros
*******************************************************************************/

/******************************************************************************
* Module Typedefs
*******************************************************************************/

/******************************************************************************
* Module Variable Definitions
*******************************************************************************/
static strctSystemErrors strsystemErrorLog;

/******************************************************************************
* Function Prototypes
*******************************************************************************/

/******************************************************************************
* Function Definitions
*******************************************************************************/

/******************************************************************************
* Function : assertError(enmSystemErrorType errorType ,enmErrorStatus errorStatus)
*//**
* \b Description:
*
* This function is used to Fetch Device UUID and Flash Size from MCU memory.
*
* PRE-CONDITION: None
*
* POST-CONDITION: None
*
*
* @return 		None.
*
* \b Example Example:
* @code
*
* 	assertError(enmSystemErrorType errorType ,enmErrorStatus errorStatus);
*
* @endcode
*
* @see
*
* <br><b> - HISTORY OF CHANGES - </b>
*
* <table align="left" style="width:800px">
* <tr><td> Date       </td><td> Software Version </td><td> Initials </td><td> Description </td></tr>
* <tr><td> 08/03/2022 </td><td> 0.0.1            </td><td> HL100133 </td><td> Interface Created </td></tr>
* </table><br><br>
* <hr>
*
*******************************************************************************/
void assertError(enmSystemErrorType errorType ,enmErrorStatus errorStatus)
{
	switch(errorType)
	{
		case enmTORERRORS_TIMER7:
			strsystemErrorLog.u32Timer7Error = errorStatus;
		break;

		case enmTORERRORS_CAN1_CONFIGFILTER:
			strsystemErrorLog.u32Can1ConfigFilterError = errorStatus;
		break;

		case enmTORERRORS_CAN1_INIT:
			strsystemErrorLog.u32Can1InitError = errorStatus;
			break;

		case enmTORERRORS_CAN1_START:
			strsystemErrorLog.u32Can1StartError = errorStatus;
		break;

		case enmTORERRORS_CAN1_NOTIFYFIFO0:
			strsystemErrorLog.u32Can1NotifyFIFO0Error = errorStatus;
		break;

		case enmTORERRORS_CAN2_INIT:
			strsystemErrorLog.u32Can1InitError = errorStatus;
			break;

		case enmTORERRORS_CAN2_CONFIGFILTER:
			strsystemErrorLog.u32Can2ConfigFilterError = errorStatus;
		break;

		case enmTORERRORS_CAN2_START:
			strsystemErrorLog.u32Can2StartError = errorStatus;
		break;

		case enmTORERRORS_CAN2_NOTIFYFIFO0:
			strsystemErrorLog.u32Can2NotifyFIFO0Error = errorStatus;
		break;

		case enmTORERRORS_CAN1_TXERROR:
			strsystemErrorLog.u32Can1TxError = errorStatus;
		break;

		case enmTORERRORS_CAN2_TXERROR:
			strsystemErrorLog.u32Can2TxError = errorStatus;
		break;

		case enmTORERRORS_ENQUEUE:
			strsystemErrorLog.u32EnqueueError = errorStatus;
		break;

		case enmTORERRORS_MEM_I2C:
			strsystemErrorLog.u32MemI2CError = errorStatus;
		break;

		case enmTORERRORS_MEM_QUEUE:
			strsystemErrorLog.u32MemQueueError = errorStatus;
		break;

		default :
			/* Unwanted / Undefined Error Occured
			 * System Will Halt Completely */
			//while(1);
			break;
	}
}

//***************************************** End of File ********************************************************
