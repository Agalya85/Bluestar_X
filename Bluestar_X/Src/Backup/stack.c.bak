///*
// * stack.c
// *
// *  Created on: Jul 7, 2021
// *      Author: admin
// */
//
//#include "main.h"
//#include "externs.h"
//#include <stdlib.h>
//#include <stdint.h>
//#include "applicationdefines.h"
//#include <string.h>
//#include "stack.h"
//
//strctPayloadStack gstrGMSPayloadLIFO;
//
///****************************************************************************
// Function initStack
// Purpose: Initialize Stack
// Input:	strctPayloadStack * stack.
// Return value: None.
//
// Note(s)(if-any) :
//
//
// Change History:
// Author           	Date                Remarks
// KloudQ Team       24-03-2020			Initial Definition
//******************************************************************************/
//void initStack(strctPayloadStack * stack)
//{
//	stack->u32MaxStackDepth = MAX_STACK_SIZE;
//	stack->u32Stacktop = -1;
//	memset(stack->au8PayloadStack,'\0',sizeof(char) * sizeof(stack->au8PayloadStack));
//}
//
///****************************************************************************
// Function isStackEmpty
// Purpose: Check if Stack is empty
// Input:	strctPayloadStack * stack.
// Return value: uint32_t boolean .
//				TRUE = Stack is Empty
//				FASLE = Stack is not Empty
//
// Note(s)(if-any) :
//
//
// Change History:
// Author           	Date                Remarks
// KloudQ Team       24-03-2020			Initial Definition
//******************************************************************************/
//uint32_t isStackEmpty(strctPayloadStack * stack)
//{
//   if(stack->u32Stacktop == -1)
//      return TRUE;
//   else
//      return FALSE;
//}
//
///****************************************************************************
// Function isStackFull
// Purpose: Check if Stack is Full
// Input:	strctPayloadStack * stack.
// Return value: uint32_t boolean .
//				TRUE = Stack is Empty
//				FASLE = Stack is not Empty
//
// Note(s)(if-any) :
//
//
// Change History:
// Author           	Date                Remarks
// KloudQ Team       24-03-2020			Initial Definition
//******************************************************************************/
//uint32_t isStackFull(strctPayloadStack * stack)
//{
//   if(stack->u32Stacktop == (MAX_STACK_SIZE - 1))
//      return TRUE;
//   else
//      return FALSE;
//}
//
///****************************************************************************
// Function getStackPeek
// Purpose: Return element at stack top
// Input:	strctPayloadStack * stack.
// Return value: void * data .
//
// Note(s)(if-any) :
// Will return Char * for Payload String
//
// Kindly Use proper dereferencing / Typecasting
//
//
// Change History:
// Author           	Date                Remarks
// KloudQ Team       24-03-2020			Initial Definition
//******************************************************************************/
//void * getStackPeek(strctPayloadStack * stack)
//{
//   return stack->au8PayloadStack[stack->u32Stacktop];
//}
//
///****************************************************************************
// Function popDataFromStack
// Purpose: Pop Data from Stack
// Input:	strctPayloadStack * stack.
// Return value: void * data .
//
//
// Note(s)(if-any) :
//Kindly Use proper dereferencing / Typecasting
//
//
// Change History:
// Author           	Date                Remarks
// KloudQ Team       24-03-2020			Initial Definition
//******************************************************************************/
//void popDataFromStack(strctPayloadStack * stack)
//{
//	/* After data is uploaded free the pointer in stack->data */
//	if((stack->u32Stacktop == -1))
//	{
//		// printf(" Stack is Empty \n ");
//	}
//	else
//	{
//		free(stack->au8PayloadStack[stack->u32Stacktop]);
//		stack->au8PayloadStack[stack->u32Stacktop] = NULL;
//		stack->u32Stacktop = (stack->u32Stacktop - 1);
//
//		if(stack->u32Stacktop < -1)
//			stack->u32Stacktop = -1;
//	}
//}
//
//
///****************************************************************************
// Function pushDataToStack
// Purpose: Push Data to Stack
// Input:	strctPayloadStack * stack . void * data
// Return value: void * data .
//
//
// Note(s)(if-any) :
//Kindly Use proper dereferencing / Typecasting
//
//
// Change History:
// Author           	Date                Remarks
// KloudQ Team       24-03-2020			Initial Definition
//******************************************************************************/
//void pushDataToStack(strctPayloadStack * stack , char * data)
//{
//	if(data != NULL)
//	{
//		if(stack->u32Stacktop == (MAX_STACK_SIZE - 1))
//		{
//			/*Stack Overflow */
//			stack->u32Stacktop = -1;
//		}
//		stack->u32Stacktop++;
//		/* If stack has data overwrite it */
//		if(stack->au8PayloadStack[stack->u32Stacktop] != NULL)
//		{
//			free(stack->au8PayloadStack[stack->u32Stacktop]);
//			stack->au8PayloadStack[stack->u32Stacktop] = NULL;
//			stack->au8PayloadStack[stack->u32Stacktop]= data;
//		}
//		else
//			stack->au8PayloadStack[stack->u32Stacktop]= data;
//	}
//}
