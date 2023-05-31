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
/** @file  queue.c
 *  @brief Queue implementation for GSM ,Debug and Payload and utility Functions
 */

/******************************************************************************
* Includes
*******************************************************************************/
#include "main.h"
#include "stm32f4xx_hal.h"
#include "applicationdefines.h"
#include "externs.h"
#include <string.h>
#include <stdlib.h>
#include "queue.h"

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

/******************************************************************************
* Function Prototypes
*******************************************************************************/

/******************************************************************************
* Function Definitions
*******************************************************************************/

/******************************************************************************
* Function : initQueue(strctQUEUE *queue)
*//**
* \b Description:
*
* This function is used to Initialize used Queue.
*
* PRE-CONDITION: None
*
* POST-CONDITION: Passed Queue is initialized
*
*
* @return 		None.
*
* \b Example Example:
* @code
*
* 	initQueue(&queue);
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
* </table><br><br>
* <hr>
*
*******************************************************************************/
void initQueue(strctQUEUE *queue)
{
    queue->head = 0;
    queue->tail = 0;
}

/******************************************************************************
* Function : enqueue(strctQUEUE * queue,char * data)
*//**
* \b Description:
*
* This function is used to Add data to the Queue.
*
* PRE-CONDITION: The queue passed must be initialised
*
* POST-CONDITION: Data is added to the queue
*
*
* @return 		None.
*
* \b Example Example:
* @code
*
* 	enqueue(&queue,"data");
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
* </table><br><br>
* <hr>
*
*******************************************************************************/
void enqueue(strctQUEUE * queue,char * data)
{
	if(data != NULL)
	{
		if(queue->head == (MAX_QUEUE_SIZE))
		{
			/*Queue Overflow */
			queue->head = 0;
		}
		/* If queue has data overwrite it */
		if(queue->data[queue->head] != NULL)
		{
			free(queue->data[queue->head]);
			queue->data[queue->head] = NULL;
		}
		queue->data[queue->head]= data;
		queue->head++;
	}
	else
	{
		/* Log Error */
		//Error_Callback(enmERROR_ENQUEUE);
	}
}

/******************************************************************************
* Function : dequeue(strctQUEUE * queue)
*//**
* \b Description:
*
* This function is used to remove data to the Queue.
*
* PRE-CONDITION: The queue passed must be initialised
*
* POST-CONDITION: Data is removed from the queue
*
*
* @return 		None.
*
* \b Example Example:
* @code
*
* 	dequeue(&queue);
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
* </table><br><br>
* <hr>
*
*******************************************************************************/
void dequeue(strctQUEUE * queue)
{
	/* After data is uploaded free the pointer in queue->data */
    if(((queue->tail == 0) && (queue->head == 0)) || (queue->tail == queue->head))
    {
       // printf("Queue is Empty \n");
    }
    else
    {
        free(queue->data[queue->tail]);
        queue->data[queue->tail] = NULL;
        queue->tail++;
        if(queue->tail > (MAX_QUEUE_SIZE-1)) queue->tail=0;
    }
}

/******************************************************************************
* Function : displayQueue(strctQUEUE *queue)
*//**
* \b Description:
*
* This function is used to print all data from the Queue.
*
* PRE-CONDITION: The queue passed must be initialised
*
* POST-CONDITION: All data contained in the queue is printed
*
*
* @return 		None.
*
* \b Example Example:
* @code
*
* 	displayQueue(&queue);
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
* </table><br><br>
* <hr>
*
*******************************************************************************/
void displayQueue(strctQUEUE *queue)
{
    if((queue->tail==0) && (queue->head==0))
	{
    	//printf("Queue is Empty \n");
	}
    else
    {
        int i;
        for (i=queue->tail;i<queue->head;i++)
        {
           // printf("Queue Element : %d\n",(int)queue->data[i]);
        }
    }
}

/******************************************************************************
* Function : isQueueEmpty(strctQUEUE * queue)
*//**
* \b Description:
*
* This function is used to check for queue empty.
*
* PRE-CONDITION: The queue passed must be initialised
*
* POST-CONDITION: Returns 1 of queue is empty else returns 0
*
*
* @return 		None.
*
* \b Example Example:
* @code
*
* 	uint32_t queueStatus  = isQueueEmpty(&queue);
* 	if(queueStatus)
* 	{
* 	 	//Queue is empty
* 	}
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
* </table><br><br>
* <hr>
*
*******************************************************************************/
uint32_t isQueueEmpty(strctQUEUE * queue)
{
	return((queue->head) == (queue->tail));
}

/******************************************************************************
* Function : isQueueFull(strctQUEUE * queue)
*//**
* \b Description:
*
* This function is used to check for queue full.
*
* PRE-CONDITION: The queue passed must be initialised
*
* POST-CONDITION: Returns 1 of queue is full else returns 0
*
*
* @return 		None.
*
* \b Example Example:
* @code
*
* 	uint32_t queueStatus  = isQueueFull(&queue);
* 	if(queueStatus)
* 	{
* 	 	//Queue is Full
* 	}
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
* </table><br><br>
* <hr>
*
*******************************************************************************/
uint32_t isQueueFull(strctQUEUE * queue)
{
	return(queue->head == (MAX_QUEUE_SIZE));
}

//******************************* End of File *******************************************************************
