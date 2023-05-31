/*******************************************************************************
* Title                 :   Generic Queue DS implementation
* Filename              :   queue.c
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
/*************** FILE REVISION LOG *****************************************
*
*    Date    Version   Author         	  Description
*  01/06/19   1.0.0    Hrishikesh Limaye   Initial Release.
*
*******************************************************************************/
/** @file  queue.h
 *  @brief Utility for implementing generic queue(s)
 *
 *  This is the header file for the definition(s) related to queue DS
 */

#ifndef __QUEUE_H
#define __QUEUE_H

/******************************************************************************
* Includes
*******************************************************************************/

/******************************************************************************
* Module Preprocessor Constants
*******************************************************************************/

/**
 * \brief Define Max queue size  \a .
   \param None.
   \returns None \a .
 */
#define MAX_QUEUE_SIZE         (10)

/******************************************************************************
* Module Preprocessor Macros
*******************************************************************************/

/******************************************************************************
* Module Typedefs
*******************************************************************************/
/**
 * This Structure stores queue DS
 */
typedef struct{
    int head;
    int tail;
    char* data[MAX_QUEUE_SIZE];
}strctQUEUE;

/******************************************************************************
* Module Variable Definitions
*******************************************************************************/

/******************************************************************************
* Function Prototypes
*******************************************************************************/
#ifdef __cplusplus
extern "C"{
#endif

void initQueue(strctQUEUE *queue);
void enqueue(strctQUEUE * queue,char * data);
void dequeue(strctQUEUE * queue);
void displayQueue(strctQUEUE *queue);
uint32_t isQueueEmpty(strctQUEUE * queue);

#ifdef __cplusplus
} // extern "C"
#endif

#endif

//******************************* End of File *******************************************************************
