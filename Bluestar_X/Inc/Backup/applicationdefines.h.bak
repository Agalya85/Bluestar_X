/*******************************************************************************
* Title                 :   Main Application Defines
* Filename              :   applicationdefines.h
* Author                :   Hrishikesh Limaye
* Origin Date           :   28/02/2022
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
*  28/02/22   1.0.0    Hrishikesh Limaye   Initial Release.
*******************************************************************************/

/** @file  applicationdefines.h
*   @brief Main Application Utility Fuctions and defines
*
*  This is the header file for the definition(s) related to main application
*/


#ifndef __APPLICATIONDEFINES_H
#define __APPLICATIONDEFINES_H

#ifdef __cplusplus
 extern "C" {
#endif

 /******************************************************************************
 * Includes
 *******************************************************************************/
#include "main.h"
#include "stm32f4xx_hal.h"
#include "stdlib.h"
#include "netif.h"
 /******************************************************************************
 * Module Preprocessor Constants
 *******************************************************************************/
 /**
 * \brief Defines Default APN \a .
   \param None.
   \returns None \a .
 */
#define DEFAULT_APN				"airteliot.com"
 /**
 * \brief Defines default data url \a .
   \param None.
   \returns None \a .
 */
#define DEFAULT_URL				"http://59.163.219.179:8021/api/Values/PostStringData"
 /**
 * \brief Defines default phone number \a .
   \param None.
   \returns None \a .
 */
#define DEFAULT_PHNO			"+918149798139"
 /**
 * \brief Defines default upload freq (ON) \a .
   \param None.
   \returns None \a .
 */
#define DEFAULT_UPFREQON		(60)								/* Default ON Up Freq */
 /**
 * \brief Defines default upload freq (Batt) \a .
   \param None.
   \returns None \a .
 */
#define DEFAULT_UPFREQOFF		(300)								/* Default OFF Up Freq */

 /**
 * \brief Defines TRUE \a .
   \param None.
   \returns None \a .
 */
#define TRUE 	(1)
 /**
 * \brief Defines FALSE \a .
   \param None.
   \returns None \a .
 */
#define FALSE (!TRUE)

/**
* \brief Defines Max payload queue size \a .
\param None.
\returns None \a .
*/
#define MAX_PAYLOAD_QUEUE_SIZE (10)
 /**
 * \brief Defines payload data string radix \a .
   \param None.
   \returns None \a .
 */
#define PAYLOAD_DATA_STRING_RADIX     (10)
/**
* \brief Defines Payload separator \a .
\param None.
\returns None \a .
*/
#define PAYLOAD_SEPARATOR       ","

/******************************************************************************
 * Module Preprocessor Macros
 *******************************************************************************/

 /******************************************************************************
 * Module Typedefs
 *******************************************************************************/
 /**
 * This enumeration is a list of Modbus Tasks
 */
 typedef enum
 {
 	enmMODBUS_IDLE = 0,
 	enmMODBUS_SEND_QUERY,
 	enmMODBBUS_AWAIT_RESPONSE,
 	enmMODBUS_RETRY_QUERY,
 	enmMODBUS_RESPONSE_TIMEOUT,
 	enmMODBUS_PARSE_RESPONSE,

 }enmMODBUSFSMState;

 /******************************************************************************
 * Function Prototypes
 *******************************************************************************/

#ifdef __cplusplus
extern "C"{
#endif

#ifdef __cplusplus
} // extern "C"
#endif


#endif /*__APPLICATIONDEFINES_H  */

//***************************************** End of File ********************************************************
