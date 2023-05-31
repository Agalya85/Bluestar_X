/*******************************************************************************
* Title                 :   Ethernet Interface Utilities
* Filename              :   user_ethernet.h
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

/** @file user_ethernet.h
*  @brief Ethernet Interface Implementation*
*
*/

#ifndef USER_ETHERNET_H_
#define USER_ETHERNET_H_

/******************************************************************************
* Includes
*******************************************************************************/
/******************************************************************************
* Module Preprocessor Constants
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

void updateEthLinkStatus(void);

#ifdef __cplusplus
} // extern "C"
#endif
#endif /* USER_ETHERNET_H_ */
