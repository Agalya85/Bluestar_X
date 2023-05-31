/*******************************************************************************
* Title                 :   System Payload Generation
* Filename              :   payload.c
* Author                :   Hrishikesh Limaye
* Origin Date           :   08/03/2022
* Version               :   1.0.0
* Compiler              :
* Target                :   STM32F437VITx,3 - Tor4GEth
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
/** @file  payload.h
 *  @brief Utility for system payload generation
 *
 *  This is the header file for the definition(s) related to device payload
 */
#ifndef PAYLOAD_H_
#define PAYLOAD_H_
/******************************************************************************
* Includes
*******************************************************************************/

/******************************************************************************
* Module Preprocessor Constants
*******************************************************************************/
/**
 * \brief Defines Max queue size  \a .
   \param None.
   \returns None \a .
 */
#define GSM_PAYLOAD_MAX_SIZE (2000)
/**
 * \brief Defines SOF for payload  \a .
   \param None.
   \returns None \a .
 */
#define START_OF_FRAME			"*"									/* SOF Character */
/**
 * \brief Defines EOF for payload  \a .
   \param None.
   \returns None \a .
 */
#define END_OF_FRAME			"#"									/* EOF Character */
/**
 * \brief Defines model number for payload  \a .
   \param None.
   \returns None \a .
 */
#define MODEL_NUMBER    		"Tor4Eth"						    /* Model Number */
/**
 * \brief Defines FW version for payload  \a .
   \param None.
   \returns None \a .
 */
#define FIRMWARE_VER			"BSL_Prod_2_0_0"		        //"LCAX2-X56H-2"/* Firmware Version */
/**
 * \brief Defines RTC Date separator for payload  \a .
   \param None.
   \returns None \a .
 */
#define RTC_DATESEPARATOR       "/"
/**
 * \brief Defines RTC Param separator for payload  \a .
   \param None.
   \returns None \a .
 */
#define RTC_PARAM_SEPARATOR     " "
/**
 * \brief Defines RTC time separator for payload  \a .
   \param None.
   \returns None \a .
 */
#define RTC_TIMESEPARATOR       ":"

/******************************************************************************
* Module Preprocessor Macros
*******************************************************************************/

/******************************************************************************
* Module Typedefs
*******************************************************************************/

/******************************************************************************
* Function Prototypes
*******************************************************************************/
#ifdef __cplusplus
extern "C"{
#endif

void updateInputVoltage(void);
char * getSystemDataString(void);
char * getSystemConfig(void);
void formatGPSSring(void);

#ifdef __cplusplus
} // extern "C"
#endif
#endif /* PAYLOAD_H_ */
