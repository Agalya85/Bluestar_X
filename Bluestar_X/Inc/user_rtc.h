/*******************************************************************************
* Title                 :   Internal RTC Interface
* Filename              :   user_rtc.h
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

/** @file user_rtc.h
*  @brief Internal RTC Implementation
*
*  This is the header file for the definition(s) related to Internal RTC
*  implementation
*/


#ifndef USER_RTC_H_
#define USER_RTC_H_

/******************************************************************************
* Includes
*******************************************************************************/
#include "stdint.h"
/******************************************************************************
* Module Preprocessor Constants
*******************************************************************************/

/******************************************************************************
* Module Typedefs
*******************************************************************************/
/**
* This Structure contains elapsed rtc details
*/
typedef struct
{
	uint32_t u32RefTimeHH;
	uint32_t u32RefTimeMin;
	uint32_t u32RefTimeSec;
	uint32_t u32RefTimeMilliSec;

	uint32_t u32TimeHH;
	uint32_t u32TimeMin;
	uint32_t u32TimeSec;
	uint32_t u32TimeMilliSec;

	uint32_t u32ActTimeHH;
	uint32_t u32ActTimeMin;
	uint32_t u32ActTimeSec;
	uint32_t u32ActTimeMilliSec;

}strTimeElapsedAftersync;

/******************************************************************************
* Function Prototypes
*******************************************************************************/
#ifdef __cplusplus
extern "C"{
#endif

void getrtcStamp(void);
uint32_t DecimalToBCD (uint32_t Decimal);
uint32_t BCDToDecimal(uint32_t BCD);
void updateElapsedTime(uint32_t millisecond);
void syncRTCGPS(void);
void getRTCCalender(void);
void backupRTCCalender(void);
void readbackedupRTCCalender(void);

#ifdef __cplusplus
} // extern "C"
#endif

#endif /* USER_RTC_H_ */
