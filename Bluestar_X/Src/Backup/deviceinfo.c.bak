/*******************************************************************************
* Title                 :   Device Info - 29 Bit UUID and Flash Size of MCU
* Filename              :   deviceinfo.c
* Author                :   Hrishikesh Limaye
* Origin Date           :   1/6/2019
* Version               :   1.0.0
* Compiler              :
* Target                :   STM32F437VITx,STM32F091CBTx,STM32L433 - Tor4GEth,TorMini
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
/****************************************************************************
 UUID :  All STM32 microcontrollers feature a 96-bit factory-programmed
 	     unique device ID.

 	     Device electronic signature :
 	     The electronic signature is stored in the System memory area in the
 	     Flash memory module,and can be read using the JTAG/SWD or the CPU.
 	     It contains factory-programmed identification data that allow the
 	     user firmware or other external devices to automatically
		 match its interface to the characteristics

		 Memory size register :
		 Flash size register :
		 Base address: 0x1FFFF7CC
		 Read only = 0xXXXX where X is factory-programmed

		 Bits 15:0 F_SIZE: Flash memory size
		 The value stored in this field indicates the Flash memory size
		 of the device expressed in Kbytes.
		 Example: 0x0040 = 64 Kbytes.

 	     Unique device ID registers (96 bits) :
 	     The unique device identifier is ideally suited:
			� for use as serial numbers
			� for use as security keys in order to increase the security of code in
			  Flash memory while using and combining this unique ID with software
			  cryptographic primitives and protocols before programming the internal Flash
			  memory
			� to activate secure boot processes, etc.

		The 96-bit unique device identifier provides a reference number which is
		unique for any device and in any context.
		These bits can never be altered by the user.

		The 96-bit unique device identifier can also be read in single
		bytes/half-words/words in different ways and then be concatenated
		using a custom algorithm.

		Base address: 0x1FFFF7CC
		Address offset: 0x00
		Read only = 0xXXXX XXXX where X is factory-programmed

		U_ID(23:0): LOT_NUM[55:32]
		Lot number (ASCII code)
		U_ID(31:24): WAF_NUM[7:0]
		Wafer number (8-bit unsigned number)
		U_ID(63:32): LOT_NUM[31:0]
		Lot number (ASCII code)
		U_ID(95:64): 95:64 unique ID bits

 *****************************************************************************/

/*************** FILE REVISION LOG *****************************************
*
*    Date    Version   Author         	  Description
*  01/06/19   1.0.0    Hrishikesh Limaye   Initial Release.
*
*******************************************************************************/

/** @file  deviceinfo.c
 *  @brief Utilities for fetching device information (UUID and FlashSize)
 */

/******************************************************************************
* Includes
*******************************************************************************/
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include "main.h"
#include "stm32f4xx_hal.h"
#include "deviceinfo.h"
#include "applicationdefines.h"

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
char dinfo[100];
char buffuuid0[32];
char buffuuid1[32];
char buffuuid2[32];
char buffflash[32];

uint32_t gu32DeviceId = 0;
strSTMDeviceSignature deviceSign;

/******************************************************************************
* Function Prototypes
*******************************************************************************/

/******************************************************************************
* Function Definitions
*******************************************************************************/

/******************************************************************************
* Function : updateDeviceSignature()
*//**
* \b Description:
*
* This function is used to Fetch Device UUID and Flash Size from MCU memory.
*
* PRE-CONDITION: Define STM32_UUID , STM32_FLASHSIZE and their MCU relevant
* 				 Address in deviceinfo.h
*
* POST-CONDITION: dinfo will be updated with UUID and Flash Size of MCU used
*
*
* @return 		None.
*
* \b Example Example:
* @code
*
* 	updateDeviceSignature();
*
* @endcode
*
* @see
*
* <br><b> - HISTORY OF CHANGES - </b>
*
* <table align="left" style="width:800px">
* <tr><td> Date       </td><td> Software Version </td><td> Initials </td><td> Description </td></tr>
* <tr><td> 01/06/2019 </td><td> 0.0.1            </td><td> HL100133 </td><td> Interface Created </td></tr>
* <tr><td> 01/09/2019 </td><td> 0.0.1            </td><td> HL100133 </td><td> Flash Size removed from payload </td></tr>
* </table><br><br>
* <hr>
*
*******************************************************************************/
void updateDeviceSignature(void)
{

	uint32_t u32UUID0 = 0;
	uint32_t u32UUID1 = 0;
	uint32_t u32UUID2 = 0;

	u32UUID0 = STM32_UUID[0];
	u32UUID1 = STM32_UUID[1];
	u32UUID2 = STM32_UUID[2];

	/* Variable for production */
	gu32DeviceId = STM32_UUID[0];
	/*Integer to ASCII UUID 2 */
	itoa(u32UUID2,buffuuid2,10);
	/*Integer to ASCII UUID 1 */
	itoa(u32UUID1,buffuuid1,10);
	/*Integer to ASCII UUID 0 */
	itoa(u32UUID0,buffuuid0,10);

	/*Integer to ASCII Flash Size */
#if (USEFLASHSIZEINDEVINFO == TRUE)
	itoa(deviceSign.u32FlashSize,buffflash,10);
#endif

	/* Append Device Info to Array */
	strcat(dinfo,buffuuid2);
	strcat(dinfo,buffuuid1);
	strcat(dinfo,buffuuid0);

#if (USEFLASHSIZEINDEVINFO == TRUE)
	strcat(dinfo,buffflash);
#endif
	/* Update Structure */
	/* 96 Bit UUID */
	deviceSign.u32UUID0 = STM32_UUID[0];
	deviceSign.u32UUID1 = STM32_UUID[1];
	deviceSign.u32UUID2 = STM32_UUID[2];
	/* 16 Bit Flash Size */
	deviceSign.u32FlashSize = STM32_FLASHSIZE;
}

/*************** END OF FUNCTIONS ***************************************************************************/
