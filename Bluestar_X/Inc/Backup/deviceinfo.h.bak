/****************************************************************************
* Title                 :   Device Info - 29 Bit UUID and Flash Size of MCU
* Filename              :   deviceinfo.h
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
*****************************************************************************/
/*************** FILE REVISION LOG *****************************************
*
*    Date    Version   Author         	  Description
*  01/06/19   1.0.0    Hrishikesh Limaye   Initial Release.
*
*******************************************************************************/

/** @file  deviceinfo.h
 *  @brief Utility for fetching device information (UUID and FlashSize)
 *
 *  This is the header file for the definition(s) related to MCU UUID and
 *  flash size .
 */

#ifndef DEVICEINFO_H_
#define DEVICEINFO_H_

/******************************************************************************
* Includes
*******************************************************************************/
/******************************************************************************
* Module Preprocessor Constants
*******************************************************************************/
/**
 * \brief Fetches UUID form MCU flash  \a .
   \param None.
   \returns 29 Bit UUID \a .
 */
#define STM32_UUID ((uint32_t *) 0x1FFF7A10)						/* 96 bit Unique Identification ID */
/**
 * \brief Fetches Flashsize form MCU flash  \a .
   \param  None.
   \returns flashsize \a .
 */
#define STM32_FLASHSIZE (*((uint32_t *)   0x1FFF7A22))				/* 16 bit Flash Size */

/******************************************************************************
* Module Preprocessor Macros
*******************************************************************************/

/******************************************************************************
* Module Typedefs
*******************************************************************************/
/**
 * This Structure stores MCU UUID and FlashSize
 */
typedef struct
{
	uint32_t u32UUID0;
	uint32_t u32UUID1;
	uint32_t u32UUID2;
	uint32_t u32FlashSize;
}strSTMDeviceSignature;

/******************************************************************************
* Module Variable Definitions
*******************************************************************************/

/******************************************************************************
* Function Prototypes
*******************************************************************************/

#ifdef __cplusplus
extern "C"{
#endif

void updateDeviceSignature(void);

#ifdef __cplusplus
} // extern "C"
#endif

#endif /* DEVICEINFO_H_ */
