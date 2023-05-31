/*******************************************************************************
* Title                 :   Serial Communication (UART / USART) Interface
* Filename              :   serial_communication.c
* Author                :   Hrishikesh Limaye
* Origin Date           :   08/03/2022
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
*  08/03/21   1.0.0    Hrishikesh Limaye   Initial Release.
*
*******************************************************************************/

/** @file  serial_communication.c
 *  @brief Utilities for serial communication interface
 */

/******************************************************************************
* Includes
*******************************************************************************/
#include "applicationdefines.h"
#include "stm32f4xx_hal.h"
#include "externs.h"
#include <string.h>
#include <stdlib.h>
#include "user_timer.h"
#include "modbus_rs485.h"
#include "gsmSim7600.h"
#include "serial_communication.h"

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
volatile uint8_t  u8receivedChar;
volatile uint32_t u32GpsCharRcvd = 0;
volatile uint32_t gau32GpsCharacterCounter = 0;
/******************************************************************************
* Function Prototypes
*******************************************************************************/

/******************************************************************************
* Function Definitions
*******************************************************************************/

/******************************************************************************
* Function : GSM_CharReception_Callback(void)
*//**
* \b Description:
*
* This function is callback for gsm module communication.
* -> Called from ISR
  -> USART1
*
* PRE-CONDITION: Enable GSM UART interrupt
*
* POST-CONDITION: Stores character received in buffer
*
*
* @return 		None.
*
* \b Example Example:
* @code
*
*
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
extern uint32_t gu32GSMCharacterTimeout;
volatile uint32_t u8GSMCharRcv = 0;
uint8_t u8gsmResponse = 0;
extern strctGSM gsmInstance;
void GSM_CharReception_Callback(void)
{
	u8gsmResponse = LL_USART_ReceiveData8(UART4);
	gsmInstance.as8GSM_Response_Buff[gsmInstance.u8GSM_Response_Character_Counter++] = u8gsmResponse;
	gu32GSMCharacterTimeout = FIVEHUNDRED_MS;
	if(u8GSMCharRcv == 0)
		u8GSMCharRcv = 1;
}

//void GPS_CharReception_Callback(void)
//{
//	HAL_GPIO_TogglePin(LED_3_GPIO_Port,LED_3_Pin);
//	/* Add String Parsing Logic */
//	if(gau32GpsCharacterCounter >= 100)
//		gau32GpsCharacterCounter = 0;
//
//	gsmInstance.agpsLocationData[gau32GpsCharacterCounter] = LL_USART_ReceiveData8(USART6);
//	gu32GSMCharacterTimeout = FIVEHUNDRED_MS;
//	if(u32GpsCharRcvd == 0)
//		u32GpsCharRcvd = 1;
//}

/******************************************************************************
* Function : GSM_CharReception_Callback(void)
*//**
* \b Description:
*
* This function is callback for modbus RS485 slave communication.
* -> Called from ISR
  -> USART3
*
* PRE-CONDITION: Enable UART interrupt
*
* POST-CONDITION: Stores character received in buffer
*
*
* @return 		None.
*
* \b Example Example:
* @code
*
*
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
extern strctModbusMaster master;
extern uint8_t gu8MBResponseFlag;
extern uint32_t gu32ModbusFrameEndTimer;
uint8_t u8TestArray[30];
uint8_t u8TestArrayCounter = 0;
volatile uint8_t gu8ModbusInterruptFlag = 0;
void MODBUS_CharReception_Callback(void)
{

	u8receivedChar =LL_USART_ReceiveData8(USART3);
//		 /*Only for debug*/
//	 u8TestArray[u8TestArrayCounter++] = u8receivedChar;
//
//	 if(u8TestArrayCounter == 25)
//	 {
//		 u8TestArrayCounter = 0;
//	 }
	if(gu8MBResponseFlag == 0)
	{
		if(master.u8MBSlave_Address == u8receivedChar)
		{
			/* Response from slave is received */
			gu8MBResponseFlag = 1;
			gu32ModbusFrameEndTimer = gu32Modbus485ResponseTime;//ONE_SEC;//TWO_SEC;
			gu32MBRTUClientConnectedFlag = 1;
		}
	}
	else
	{
		master.u8SlaveResponseArray[master.u8MBResponseCharacterCounter++] = u8receivedChar;
		gu32ModbusFrameEndTimer = gu32Modbus485ResponseTime;//ONE_SEC;//TWO_SEC;
	}
}

/******************************************************************************
* Function : parseModbusResponse(void)
*//**
* \b Description:
*
* This function is callback for modbus slave data parsing .
*
* PRE-CONDITION: Enable UART interrupt
*
* POST-CONDITION:
*
*
* @return 		None.
*
* \b Example Example:
* @code
*
*	parseModbusResponse()
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
void parseModbusResponse(void)
{
	if(gu8ModbusInterruptFlag == 0)
		return;

//	u8receivedChar =LL_USART_ReceiveData8(USART3);
	/*Only for debug*/
	//u8TestArray[u8TestArrayCounter++] = u8receivedChar;

	if(u8TestArrayCounter == 25)
	{
		u8TestArrayCounter = 0;
	}
	if(gu8MBResponseFlag == 0)
	{
		if(master.u8MBSlave_Address == u8receivedChar)
		{
		/* Response from slave is received */
			gu8MBResponseFlag = 1;
			gu32MBRTUClientConnectedFlag = 1;
			gu32ModbusFrameEndTimer = TWO_SEC;
		}
	}
	else
	{
		master.u8SlaveResponseArray[master.u8MBResponseCharacterCounter++] = u8receivedChar;
		gu32ModbusFrameEndTimer = TWO_SEC;
	}
	gu8ModbusInterruptFlag = 0;
}

//******************************* End of File *******************************************************************
