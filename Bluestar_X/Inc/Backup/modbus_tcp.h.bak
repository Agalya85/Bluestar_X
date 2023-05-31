/*******************************************************************************
* Title                 :   Modbus Client Implementation - TCP/IP Interface
* Filename              :   modbus_tcp.h
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

/** @file modbus_tcp.h
*  @brief Modbus TCP/IP Client Implementation
*
*  This is the header file for the definition(s) related to Modbus TCP/IP Client
*  Implementation
*/

#ifndef MODBUS_TCP_H_
#define MODBUS_TCP_H_

/******************************************************************************
* Includes
*******************************************************************************/
/******************************************************************************
* Module Preprocessor Constants
*******************************************************************************/
/**
* \brief Defines Transaction ID for MB client \a .
  \param None.
  \returns None \a .
*/
#define MB_TCP_TID					(uint16_t) (3)  /* Transaction Id - 2 Bytes */
/**
* \brief Defines Protocol ID for MB client \a .
  \param None.
  \returns None \a .
*/
#define MB_TCP_PID					(uint16_t) (0)  /* Protocol Id - 2 Bytes . Always 0 for modbus */
/**
* \brief Defines Unit ID for MB client \a .
  \param None.
  \returns None \a .
*/
#define MB_TCP_UID			 		 (uint8_t) (1)  /* Unit Id - 1 Byte */
/**
* \brief Defines Address dereferencing for MB query \a .
  \param None.
  \returns None \a .
*/
#define MB_ADDRESS_DEREF					   (1)	/* Modbus Address dereferencing */
/**
* \brief Defines Max retries for MB query \a .
  \param None.
  \returns None \a .
*/
#define MB_TCP_QUERY_RETRY_ATTEMPTS	  		   (5)	/* Max Retry Attempts per query */
/**
* \brief Defines MB Payload radix . 10-dec, 16-hex , 2-bin \a .
  \param None.
  \returns None \a .
*/
#define MB_PAYLOAD_RADIX					   (10) /* Payload in decimal format */

/******************************************************************************
* Module Preprocessor Macros
*******************************************************************************/

/******************************************************************************
* Module Typedefs
*******************************************************************************/
/**
* This enumeration contains modbus exception details
*/
typedef enum{
	enmMB_ILLEGAL_FUCNTION = 1,  /* The function code received in the query is not
								    an allowable action for the slave. */
	enmMB_ILLEGAL_DATA_ADDRESS,  /* The data address received in the query
								    is not an allowable address for the
								    slave.*/
	enmMB_ILLEGAL_DATA_VALUE,    /* A value contained in the query data
								    field is not an allowable value for the
								    slave.*/
	enmMB_SLAVE_DEVICE_FAILURE,  /* An unrecoverable error occurred while
								    the slave was attempting to perform the
								    requested action.*/
	enmMB_ACKNOWLEDGE,			/*  The slave has accepted the request
									and is processing it, but a long duration
									of time will be required to do so. This
									response is returned to prevent a
									timeout error from occurring in the
									master. The master can next issue a
									Poll Program Complete message to
									determine if processing is completed.*/
	enmMB_SLAVE_DEVICE_BUSY,	/*  The slave is engaged in processing a
									long–duration program command. The
									master should retransmit the message
									later when the slave is free.*/
	enmMB_NEGATIVE_ACKNOWLEDGE, /* 	The slave cannot perform the program
									function received in the query. This
									code is returned for an unsuccessful
									programming request using function
									code 13 or 14 decimal. The master
									should request diagnostic or error
									information from the slave.*/
	enmMB_MEMORY_PARITY_ERROR,   /* The slave attempted to read extended
									memory, but detected a parity error in
									the memory. The master can retry the
									request, but service may be required on
									the slave device.*/
	enmMB_NO_EXCEPTION,

}enmModbusExceptionCode;

/**
* This enumeration contains modbus function codes
*/
typedef enum{
	enmMBFC_READ_COIL_STATUS = 0x01,
	enmMBFC_READ_INPUT_STATUS,
	enmMBFC_READ_HOLDING_REGISTERS,
	enmMBFC_READ_INPUT_REGISTERS,
	enmMBFC_FORCE_SINGLE_COIL,
	enmMBFC_PRESET_SINGLE_REGISTER,
	enmMBFC_READ_EXCEPTION_STATUS,
	enmMBFC_DIAGNOSTICS,
	enmMBFC_PROGRAM_485,
	enmMBFC_POLL_485,
	enmMBFC_FETCH_COMM_EVENT_CONTR,
	enmMBFC_FETCH_COMM_EVENT_LOG,
	enmMBFC_PROGRAM_CONTROLLER,
	enmMBFC_POLL_CONTROLLER,
	enmMBFC_FORCE_MULTIPLE_COIL,
	enmMBFC_PRESET_MULTIPLE_REGISTERS,
	enmMBFC_REPORT_SLAVE_ID,
	enmMBFC_PROGRAM_884_M84,
	enmMBFC_RESET_COMM_LINK,
	enmMBFC_READ_GENERAL_REFERENCE,
	enmMBFC_WRITE_GENERAL_REFERENCE,
	enmMBFC_MASK_WRITE_4X_REGISTER,
	enmMBFC_RW_4X_REGISTER,
	enmMBFC_READ_FIFO_QUEUE
}enmMBFunctionCode;

/**
* This structure contains modbus master data
*/
typedef struct
{
	uint32_t u32MBResponseCharacterCounter;		/* Counts received characters */
	uint32_t u32MBOperationStatus;				/* Tracks Slave Data Completion 0:-> Not Started 1: In process 2: Completed */
	uint32_t u32MBNoQueryAttempts;

	uint32_t u32UnitId;
	uint32_t u32MBQueryCounter;

	uint32_t u32MBStartAddress;  				/* Starting Address*/
	uint32_t u32MBNoPoints;       				/* No of Points */
	uint32_t u32NoRegisterstoFetch;

	volatile uint8_t u32QueryFrame[12]; 					/* Modbus Query TX buffer */
	uint32_t u32MBResponseStatus[50];			/* Stores Response status : 0 : Success 1: Exception*/
	int8_t s8SlaveResponseArray[350];			/* Modbus Slave Response Array */
	int32_t u32SlaveData[260];					/* Holds Temperory Data Received From Slave*/

	uint32_t u32PollDelay;
	uint32_t u32NoofRegConfigured;

	enmMBFunctionCode  u8MBFunction_Code; 		/* Modbus Function Code */
	enmModbusExceptionCode exception;			/* Modbus Exception Status*/

}strctModbusTCPMaster;

/******************************************************************************
* Function Prototypes
*******************************************************************************/
#ifdef __cplusplus
extern "C"{
#endif

void setupModbus(void);
void updateModbusQueryFrame(void);
void mbTCPPoll(void);
void updateModbusPayload(void);
void tcp_modbusclient_connect(void);

#ifdef __cplusplus
} // extern "C"
#endif
#endif /* MODBUS_TCP_H_ */

//***************************************** End of File ********************************************************

