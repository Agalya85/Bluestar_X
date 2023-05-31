/*
 * stack.h
 *
 *  Created on: Jul 7, 2021
 *      Author: admin
 */

#ifndef STACK_H_
#define STACK_H_

#define MAX_STACK_SIZE (10)

typedef struct
{
	uint32_t u32MaxStackDepth;
	int32_t u32Stacktop;
	char * au8PayloadStack[MAX_STACK_SIZE];
}strctPayloadStack;

void initStack(strctPayloadStack * stack);
uint32_t isStackEmpty(strctPayloadStack * stack);
uint32_t isStackFull(strctPayloadStack * stack);
void * getStackPeek(strctPayloadStack * stack);
void popDataFromStack(strctPayloadStack * stack);
void pushDataToStack(strctPayloadStack * stack , char * data);


#endif /* STACK_H_ */
