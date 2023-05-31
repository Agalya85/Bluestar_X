/*******************************************************************************
* Title                 :   Device Internal Timer Interface
* Filename              :   user_timer.c
* Author                :   Hrishikesh Limaye
* Origin Date           :   08/03/2022
* Version               :   1.0.0
* Compiler              :
* Target                :   STM32F43x - Tor4Eth
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

/** @file  user_timer.c
 *  @brief Utilities for Accessing Internal Timer
 */

/******************************************************************************
* Includes
*******************************************************************************/
#include "stm32f4xx_hal.h"
#include "applicationdefines.h"
#include "externs.h"
#include "user_timer.h"
#include "gsmSim7600.h"
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
uint32_t gu32GSMCharacterTimeout = 0;   /* Used for checking GSM response*/
uint32_t gu32GPSCharacterTimeout = 0;   /* Used for checking GSM response*/
uint32_t gu32RTCUpdateTimer = 0;
uint32_t gu32GSMDelay = 0;
uint32_t gu32GSMTimeout = 0;

volatile uint32_t gu32PayloadQueueEnqueue = 0;
volatile uint32_t gu32TempPayloadQueueEnqueue = 0;

uint32_t gu32SerialQueryResponseTimeout = 0;
uint32_t gu32I2CMemoryOperationTimeout = 0;
uint32_t gu32ADCOperationTimer = 0;
uint32_t gu32ADCPollTimer = 0;

volatile uint32_t gu32FotaFileReadTimer = 0;
volatile uint32_t gu32FlashMemoryTimer = 0;

/*Link Disconnect Timer Handling Patch */
volatile uint32_t gu32LinkDisconnectTimer = FIVE_MIN;

volatile uint32_t gu32LedTimer = 0;
volatile uint32_t gu32GSMHangTimer = 0;
volatile uint32_t gu32GSMRestartTimer = FIVE_MIN;
volatile uint32_t gu32GSMConfigCheckTimer = 0;//FIVE_MIN;
extern strctGSM gsmInstance;

volatile uint32_t gu32LinkDownTimer = FIFTEEN_MIN;
volatile uint32_t gu32SystemResetTimer = THIRTY_MIN;
volatile uint32_t gu32UploadFailTimer = TEN_MIN;

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	/* Timer 7 . 1 ms Interrupt */
	if(htim->Instance == TIM7)
	{

		/* Modbus/TCP Timer variables */
		if(gu32MBTCPTimer)gu32MBTCPTimer--;
		if(gu32MBPOLLTimer)gu32MBPOLLTimer--;
		if(master.u32PollDelay)master.u32PollDelay--;

		/* Modbus  Timer Variable */
		if(gu32MBCharacterDelay)gu32MBCharacterDelay--;
		if(gu32ModbusFrameEndTimer)gu32ModbusFrameEndTimer--;
		if(gu32ModbusResponseTimeout)gu32ModbusResponseTimeout--;
		if(gu32LinkDisconnectTimer)gu32LinkDisconnectTimer--;

		/* GSM Timer Variables */
		if(gu32GSMCharacterTimeout)gu32GSMCharacterTimeout--;
		if(gsmInstance.u32GSMTimer)gsmInstance.u32GSMTimer--;
		if(gsmInstance.u32GSMResponseTimer)gsmInstance.u32GSMResponseTimer--;
		if(gsmInstance.u32GSMHeartbeatTimer)gsmInstance.u32GSMHeartbeatTimer--;
		if(gu32GSMHangTimer)gu32GSMHangTimer--;
		if(gu32GPSCharacterTimeout)gu32GPSCharacterTimeout--;
		if(gu32PayloadQueueEnqueue)gu32PayloadQueueEnqueue--;
		if(gu32TempPayloadQueueEnqueue)gu32TempPayloadQueueEnqueue--;
		if(gu32FotaFileReadTimer)gu32FotaFileReadTimer--;
		if(gu32GSMRestartTimer)gu32GSMRestartTimer--; /* Added on 6/2/22 . Was Missing :P */

		/* RTC Timer Variables */
		if(gu32RTCUpdateTimer)gu32RTCUpdateTimer--;

		/* I2C Memory */
		if(gu32I2CMemoryOperationTimeout)gu32I2CMemoryOperationTimeout--;

		/* ADC */
		if(gu32ADCOperationTimer)gu32ADCOperationTimer--;
		if(gu32ADCPollTimer)gu32ADCPollTimer--;

		/* Timer Test LED Toggle */
		if(gu32LedTimer)gu32LedTimer--;

		/* Config Check */
		if(gu32GSMConfigCheckTimer)gu32GSMConfigCheckTimer--;
		if(gu32LinkDownTimer)gu32LinkDownTimer--;
		if(gu32SystemResetTimer)gu32SystemResetTimer--;
		if(gu32UploadFailTimer)gu32UploadFailTimer--;

	}
	}

//******************************* End of File *******************************************************************
