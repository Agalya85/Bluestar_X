/*******************************************************************************
* Title                 :   Modbus Client Implementation - RS485 Interface
* Filename              :   modbus_rs485.c
* Author                :   Hrishikesh Limaye
* Origin Date           :   08/03/2022
* Version               :   1.0.0
* Compiler              :
* Target                :   STM32F437VI - Tor4Eth
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

/** @file  modbus_rs485.c
 *  @brief Modbus RTU Client Implementation
 *
 *
------------------------------------------------------------------------------
How is data stored in Standard Modbus ?
------------------------------------------------------------------------------

Information is stored in the Slave device in four different tables.
Two tables store on/off discrete values (coils) and two store numerical values (registers).
The coils and registers each have a read-only table and read-write table.

Each table has 9999 values.
Each coil or contact is 1 bit and assigned a data address between 0000 and 270E.
Each register is 1 word = 16 bits = 2 bytes and also has data address between 0000 and 270E.

Coil/Register Numbers

Data Addresses      Type			            Table Name
1-9999  		0000 to 270E   Read-Write	Discrete Output Coils
10001-19999     0000 to 270E   Read-Only	Discrete Input Contacts
30001-39999     0000 to 270E   Read-Only	Analog Input Registers
40001-49999     0000 to 270E   Read-Write	Analog Output Holding Registers

Coil/Register Numbers can be thought of as location names since they do not appear in
the actual messages.The Data Addresses are used in the messages.

For example, the first Holding Register, number 40001, has the Data Address 0000.
The difference between these two values is the offset.
Each table has a different offset. 1, 10001, 30001 and 40001.


What are extended register addresses?

Since the range of the analog output holding registers is 40001 to 49999,
it implies that there cannot be more than 9999 registers.
Although this is usually enough for most applications, there are cases where more registers
would be beneficial.

Registers 40001 to 49999 correspond to data addresses 0000 to 270E.
If we utilize the remaining data addresses 270F to FFFF, over six times as many registers
can be available,65536 in total.This would correspond to register numbers from 40001 to 105536.

Many modbus software drivers (for Master PCs) were written with the 40001 to 49999 limits
and cannot access extended registers in slave devices. And many slave devices do not support
maps using the extended registers. But on the other hand, some slave devices do support these
registers and some Master software can access it, especially if custom software is written.


-------------------------------------------------------------------------------
Limitations of Modbus
-------------------------------------------------------------------------------

Since Modbus was designed in the late 1970s to communicate to programmable logic controllers, the number of
data types is limited to those understood by PLCs at the time. Large binary objects are not
supported.
No standard way exists for a node to find the description of a data object, for example,
to determine whether a register value represents a temperature between 30 and 175 degrees.
Since Modbus is a master/slave protocol, there is no way for a field device to "report by
exception" (except over Ethernet TCP/IP, called open-mbus) – the master node must routinely
poll each field device and look for changes in the data. This consumes bandwidth and network
time in applications where bandwidth may be expensive, such as over a low-bit-rate radio link.

Modbus is restricted to addressing 254 devices on one data link, which limits the number of
field devices that may be connected to a master station (once again, Ethernet TCP/IP being
an exception).
Modbus transmissions must be contiguous, which limits the types of remote communications
devices to those that can buffer data to avoid gaps in the transmission.
Modbus protocol itself provides no security against unauthorized commands or interception
of data.
-------------------------------------------------------------------------------
 */

/******************************************************************************
* Includes
*******************************************************************************/

#include "applicationdefines.h"
#include "externs.h"
#include <string.h>
#include "externs.h"
#include "user_timer.h"
#include "modbus_rs485.h"

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
strctModbusMaster master;
uint8_t gu8MBResponseFlag = 0;
uint8_t gu8MBCycleActiveFlag = 0;
uint32_t gu32MBCharacterDelay = 0;
uint8_t u8MBQueryRegisterAddressCounter = 0;
uint8_t gu8MBFrameTransmittedFlag = 0;

char gau8MBPayloadStringRT[1100] = {'0'};
char gau8TempMBRTUPayloadString[1100] = {'0'};

uint32_t gu32ModbusFrameEndTimer = 0;
uint32_t gu32ModbusResponseTimeout = 0;
uint32_t gu32MBDataByteCounterLimitRTU = 0;
uint32_t gu32MBRTUDataByteCounter = 0;

enmMODBUSFSMState modbusState = enmMODBUS_IDLE;

volatile uint32_t gu32MBRTUClientConnectedFlag = 0;
volatile uint8_t u8MBQueryCharacterCounter = 0;

/* Modbus Addresses to fetch . Arranged as seperate
 * array(s) for function code , address and No of points to fetch.
 * These arrays are updated after over the air configuration
 * Currently provision of 75 queries is done based on project requirement .
 * If more queries are to be accomodated increase the array size and payload size accordingly .
 *
 * NOTE : DO NOT MAKE THEM CONST .
 *  */
uint32_t gu32MBRTUClientFuncCode[75]=
{
	1,	1,	1,	1,	1,	1,	4,	4,	4,	4,
	4,	4,	4,	4,	4,	4,	4,	4,	4,	4,
	4,	4,	3,	3,	3,	3,	3,	3,	3,	3,
	3,	3,	3,	3,	3,	3,	3,	3,	3,	3,
	3,	3,	3,	3,	3,	3,	3,	3,	3
};

uint32_t gu32MBRTUClientAddress[75]=
{
	1,		6,		9,		11,		16,		21,		1,		33,		201,	306,
	315,	327,	375,	668,	673,	678,	680,	686,	690,	693,
	698,	758,	301,	308,	318,	323,	325,	345,	356,	359,
	361,	373,	375,	387,	392,	394,	397,	399,	401,	403,
	404,	409,	439,	455,	471,	499,	505,	511,	521
};

uint32_t gu32MBRTUClientNoofPoints[75]=
{
	4,	1,	1,	4,	2,	13,	29,	5,	2,	3,
	6,	8,	2,	1,	1,	1,	4,	3,	2,	2,
	13,	1,	3,	9,	3,	1,	14,	5,	1,	1,
	4,	1,	11,	3,	2,	2,	1,	1,	1,	1,
	1,	3,	1,	2,	2,	4,	1,	1,	1
};


uint32_t gu32MBRTUDataStatus[300]={0};

/******************************************************************************
* Function Prototypes
*******************************************************************************/

/******************************************************************************
* Function Definitions
*******************************************************************************/

/******************************************************************************
* Function : modbusCRC16(uint8_t * data , uint8_t length)
*//**
* \b Description:
*
* This function is used to calculate MODBUS CRC.
*  Low and High bytes are swapped
*
* PRE-CONDITION: Init modbus structure instance
*
* POST-CONDITION: None
*
* @return 		None.
*
* \b Example Example:
* @code
*
* 	modbusCRC16(master->u8QueryFrame , 6);
*
* @endcode
*
* @see
*
* <br><b> - HISTORY OF CHANGES - </b>
*
* <table align="left" style="width:800px">
* <tr><td> Date       </td><td> Software Version </td><td> Initials </td><td> Description </td></tr>
* <tr><td> 08/03/2022 </td><td> 0.0.1            </td><td> HL100133 </td><td> Interface Created </td></tr>
* </table><br><br>
* <hr>
*
*******************************************************************************/

uint16_t modbusCRC16(uint8_t * data , uint8_t length)
{
	uint16_t crc = 0xFFFF;

	  for (int pos = 0; pos < length; pos++)
	  {
	    crc ^= (uint16_t)data[pos];         // XOR byte into least sig. byte of crc

	    for (int i = 8; i != 0; i--)  		// Loop over each bit
	    {
	      if ((crc & 0x0001) != 0) 			// If the LSB is set
	      {
	        crc >>= 1;                    	// Shift right and XOR 0xA001
	        crc ^= 0xA001;
	      }
	      else                            	// Else LSB is not set
	        crc >>= 1;                    	// Just shift right
	    }
	  }
	  return crc;
}

/******************************************************************************
* Function :updateModbusQueryFrameRTU(strctModbusMaster *master)
*//**
* \b Description:
*
* This function is used to Updates MODBUS RTU query.
*
*
* PRE-CONDITION: Init modbus structure instance
*
* POST-CONDITION: None
*
* @return 		None.
*
* \b Example Example:
* @code
*
* 	updateModbusQueryFrameRTU(&master);
*
* @endcode
*
* @see
*
* <br><b> - HISTORY OF CHANGES - </b>
*
* <table align="left" style="width:800px">
* <tr><td> Date       </td><td> Software Version </td><td> Initials </td><td> Description </td></tr>
* <tr><td> 08/03/2022 </td><td> 0.0.1            </td><td> HL100133 </td><td> Interface Created </td></tr>
* </table><br><br>
* <hr>
*
*******************************************************************************/
void updateModbusQueryFrameRTU(strctModbusMaster *master)
{
	uint16_t crc ;
	/* Form Query Frame String */
	master->u8QueryFrame[0] = master->u8MBSlave_Address;
	master->u8QueryFrame[1] = gu32MBRTUClientFuncCode[u8MBQueryRegisterAddressCounter];
	master->u8QueryFrame[2] = ((gu32MBRTUClientAddress[u8MBQueryRegisterAddressCounter] - MB_ADDRESS_DEREF) >> 8);
	master->u8QueryFrame[3] = ((gu32MBRTUClientAddress[u8MBQueryRegisterAddressCounter] - MB_ADDRESS_DEREF)& 0xFF);
	master->u8QueryFrame[4] = (gu32MBRTUClientNoofPoints[u8MBQueryRegisterAddressCounter] >> 8);
	master->u8QueryFrame[5] = (gu32MBRTUClientNoofPoints[u8MBQueryRegisterAddressCounter] & 0xFF);
	crc = modbusCRC16(master->u8QueryFrame , 6);
	master->u8QueryFrame[6] = crc;
	master->u8QueryFrame[7] = (crc >> 8);
	/* End of Query Frame */
}

/******************************************************************************
* Function :void modbusQueryFSM(void)
*//**
* \b Description:
*
* This function is used to Fetch Data over Modbus.
*
*
* PRE-CONDITION: Init respective UART interface and enable Interrupt(s)
*  				 use gu32ModbusPollDelay - As per slave device specification
 	 	 	 	 Default : 1000 ms
*
* POST-CONDITION: None
*
* @return 		None.
*
* \b Example Example:
* @code
*
* 	modbusQueryFSM(&master);
*
* @endcode
*
* @see
*
* <br><b> - HISTORY OF CHANGES - </b>
*
* <table align="left" style="width:800px">
* <tr><td> Date       </td><td> Software Version </td><td> Initials </td><td> Description </td></tr>
* <tr><td> 08/03/2022 </td><td> 0.0.1            </td><td> HL100133 </td><td> Interface Created </td></tr>
* </table><br><br>
* <hr>
*
*******************************************************************************/
void modbusQueryFSM(void)
{
	switch(modbusState)
	{
		case enmMODBUS_IDLE:
			if(master.u32PollDelay == 0)
			{
				if(master.u8MBOperationStatus == 1)
				{

					HAL_GPIO_WritePin(MB_DE_GPIO_Port,MB_DE_Pin,GPIO_PIN_SET); // DE -> High
					HAL_GPIO_WritePin(MB_RE_GPIO_Port,MB_RE_Pin,GPIO_PIN_SET); // ~RE -> HIGH
					memset(master.u8SlaveResponseArray, 0x00, 260 * sizeof(int32_t));
					master.u8MBResponseCharacterCounter = 0;
					gu8MBResponseFlag = 0;
					if(u8MBQueryRegisterAddressCounter >= (master.u16NoRegisterstoFetch))
					{
						/* All Registers Fetched . Update payload */
						//master.u8MBOperationStatus = 2;
						u8MBQueryRegisterAddressCounter = 0;
						gu32MBRTUDataByteCounter = 0;
						break;
					}
					else
						updateModbusQueryFrameRTU(&master);

					LL_USART_EnableIT_TXE(USART3);
					LL_USART_EnableIT_TC(USART3);

					/*Balach Logic .u8MBQueryCharacterCounter increments randomly */
					u8MBQueryCharacterCounter = 0;
					LL_USART_TransmitData8(USART3,master.u8QueryFrame[u8MBQueryCharacterCounter++]);

					modbusState = enmMODBUS_SEND_QUERY;
				}
			}
			break;

		case enmMODBUS_SEND_QUERY:
			/* Handled from ISR */
			break;

		case enmMODBBUS_AWAIT_RESPONSE:
			if(master.u32PollDelay == 0)
			{
				if(gu8MBResponseFlag == 1)
				{
					/* Check for EOF */
					if((gu32ModbusFrameEndTimer == 0) && (gu32ModbusResponseTimeout != 0))
					{
						/* Response Frame Received . Parse Response */
						modbusState = enmMODBUS_PARSE_RESPONSE;
						gu8MBResponseFlag = 0;
					}
					else if(gu32ModbusResponseTimeout == 0)
					{
						modbusState = enmMODBUS_PARSE_RESPONSE;
						gu8MBResponseFlag = 0;
					}
				}
				else
				{
					/* Response Timeout */
					if(gu32ModbusResponseTimeout == 0)
						modbusState = enmMODBUS_RESPONSE_TIMEOUT;
				}
			}
			break;

		case enmMODBUS_PARSE_RESPONSE:
			/* Process received response */
			if(master.u8SlaveResponseArray[0] == gu32MBRTUClientFuncCode[u8MBQueryRegisterAddressCounter])
			{
				/* Function code and slave id is Correct .
				 * Verify CRC and Extract Number of data bytes*/
				uint8_t u8TempCRCArray[125]= {0};
				uint16_t u16TempCRC = 0;
				uint8_t u8CrcLoopCounter = 0;

				/* Number of Data Bytes Received */
				uint8_t u8MBNoDataBytesRx = (uint8_t)master.u8SlaveResponseArray[1];

				/* Calculate CRC from Received Bytes */
				u8TempCRCArray[0]=master.u8MBSlave_Address;       // ID
				u8TempCRCArray[1]=master.u8SlaveResponseArray[0]; // FC
				u8TempCRCArray[2]=master.u8SlaveResponseArray[1]; // No of Bytes
				for(u8CrcLoopCounter = 0 ; u8CrcLoopCounter < u8MBNoDataBytesRx ;u8CrcLoopCounter ++ )
				{
					u8TempCRCArray[3 + u8CrcLoopCounter] = master.u8SlaveResponseArray[2 + u8CrcLoopCounter];
				}
				u16TempCRC = modbusCRC16(u8TempCRCArray,3 + u8CrcLoopCounter);

				/* Actual Received CRC Bytes */
				uint8_t u8CRCHi = master.u8SlaveResponseArray[u8MBNoDataBytesRx + 2 ];    // CRC High Location
				uint8_t u8CRCLow = master.u8SlaveResponseArray[u8MBNoDataBytesRx + 3];    // CRC Low Location
				uint16_t u16ReceivedCRC =  (u8CRCLow<<8)|u8CRCHi;

				if(u16ReceivedCRC == u16TempCRC)
				{
					gu32MBRTUClientConnectedFlag = 1; /* Indicates Response Received form client in payload .*/
					/*if function code is of coils */
					if(master.u8SlaveResponseArray[0] == 1)
					{
						/*Read coil . Directly parse bytes */
						uint32_t LoopCounter = 0;
						for(LoopCounter = 0; LoopCounter < u8MBNoDataBytesRx; LoopCounter++)
						{
							master.u32SlaveData[gu32MBRTUDataByteCounter] = master.u8SlaveResponseArray[2 + LoopCounter];
							gu32MBRTUDataStatus[gu32MBRTUDataByteCounter++] = 2;
						}
						if(u8MBNoDataBytesRx != gu32MBRTUClientNoofPoints[u8MBQueryRegisterAddressCounter])
							gu32MBRTUDataByteCounter += (gu32MBRTUClientNoofPoints[u8MBQueryRegisterAddressCounter] - u8MBNoDataBytesRx);
					}
					else
					{
						if(u8MBNoDataBytesRx == (2 * gu32MBRTUClientNoofPoints[u8MBQueryRegisterAddressCounter]))
						{
							/* Data Length is as expected . Capture the data*/
							/* strMbClient.s8SlaveResponseArray[9] - Start of data */
							uint32_t u32LoopCounter = 0;
							for(u32LoopCounter = 0; u32LoopCounter < gu32MBRTUClientNoofPoints[u8MBQueryRegisterAddressCounter];u32LoopCounter++)
							{
								if((int32_t)master.u8SlaveResponseArray[2 + (2 * u32LoopCounter)] < 0)
								{
									/*Second byte is negative. Not working  */
									master.u32SlaveData[gu32MBRTUDataByteCounter] = ((int16_t)master.u8SlaveResponseArray[2 + (2* u32LoopCounter)] << 8)
																					 + (int16_t)master.u8SlaveResponseArray[2 + (2 * u32LoopCounter) + 1] +256 ;
								}
								else
								{
									/*Second byte is positive */
									master.u32SlaveData[gu32MBRTUDataByteCounter] = ((int16_t)master.u8SlaveResponseArray[2 + (2* u32LoopCounter)] << 8)
																					 + (int16_t)master.u8SlaveResponseArray[2 + (2 * u32LoopCounter) + 1] ;
								}

								if(master.u32SlaveData[gu32MBRTUDataByteCounter] > 32768)
								{
									/* Negative Values */
									master.u32SlaveData[gu32MBRTUDataByteCounter] = master.u32SlaveData[gu32MBRTUDataByteCounter] - 65536;
								}

								/* Valid Data available for this address */
								gu32MBRTUDataStatus[gu32MBRTUDataByteCounter] = 0;
								gu32MBRTUDataByteCounter++;
								if(gu32MBRTUDataByteCounter >= gu32MBDataByteCounterLimitRTU)
									gu32MBRTUDataByteCounter = 0;
							}
						}
						else
						{
							/* Incorrect Length */
							gu32MBRTUDataByteCounter += gu32MBRTUClientNoofPoints[u8MBQueryRegisterAddressCounter];
						}
					}

					master.u8MBResponseStatus[u8MBQueryRegisterAddressCounter++] = 0;
					//u8MBQueryRegisterAddressCounter++;
					resetModbusPort();
					master.u32PollDelay = gu32Modbus485PollingTime;//ONE_SEC;
					modbusState = enmMODBUS_IDLE;
				}
				else
				{
					/* CRC is not valid . Try Again with same query */
					modbusState = enmMODBUS_RETRY_QUERY;
				}
			}
			else if((master.u8SlaveResponseArray[0] & 0x80) == 0x80)
			{
				/* Exception Function code . Check if MSB is 1 eg : 04 -> 84 .
				 * Extract Modbus exception code
				 * Add Response Status Array and update it with Exception */
				/* Function code is invalid .
				 * Log Error Code */
				uint32_t u32TempLoopCounter = 0;
				for(u32TempLoopCounter = 0; u32TempLoopCounter < gu32MBRTUClientNoofPoints[u8MBQueryRegisterAddressCounter];u32TempLoopCounter++)
				{
					master.u32SlaveData[gu32MBRTUDataByteCounter] = master.u8SlaveResponseArray[1] ;
					gu32MBRTUDataStatus[gu32MBRTUDataByteCounter++] = 1;
				}
				u8MBQueryRegisterAddressCounter++;
				resetModbusPort();
				master.u32PollDelay = gu32Modbus485PollingTime;//ONE_SEC;
				modbusState = enmMODBUS_IDLE;
			}
			else
			{
				modbusState = enmMODBUS_RETRY_QUERY;
			}
			memset(master.u8SlaveResponseArray,0x00,sizeof(master.u8SlaveResponseArray));
			break;


		case enmMODBUS_RETRY_QUERY:
			if(master.u8MBNoQueryAttempts > MB_485_QUERY_RETRY_ATTEMPTS)
			{
				/* Max Retry Attempt Reached . Log Error and fetch Next Address */
				gu32MBRTUDataByteCounter += gu32MBRTUClientNoofPoints[u8MBQueryRegisterAddressCounter];
				u8MBQueryRegisterAddressCounter++;
			}
			else
			{
				/* Retry Same Register Address Query */
				master.u8MBNoQueryAttempts++;
			}
			if(u8MBQueryRegisterAddressCounter > master.u16NoRegisterstoFetch)
			{
				/* Had missed . Added on 7-2-22 */
				gu32MBRTUDataByteCounter = 0;
				u8MBQueryRegisterAddressCounter = 0;
				gu32MBRTUClientConnectedFlag = 0;
			}

			resetModbusPort();
			master.u32PollDelay = gu32Modbus485PollingTime;//ONE_SEC;
			modbusState = enmMODBUS_IDLE;
			memset(master.u8SlaveResponseArray,0x00,sizeof(master.u8SlaveResponseArray));
			break;

		case enmMODBUS_RESPONSE_TIMEOUT:
			/*  */
			/* Response Time Out : Exception */
		{
			uint32_t temploopcountrt = gu32MBRTUDataByteCounter;
			uint32_t iteratorrt = 0;
			gu32MBRTUDataByteCounter += gu32MBRTUClientNoofPoints[u8MBQueryRegisterAddressCounter++];
			for(iteratorrt = temploopcountrt ; iteratorrt < gu32MBRTUDataByteCounter ; iteratorrt++)
				master.u32SlaveData[iteratorrt] = 0;
			//master.u32SlaveData[gu32MBRTUDataByteCounter] = 0;
			if(u8MBQueryRegisterAddressCounter >= (master.u16NoRegisterstoFetch))
			{
				/* Had Missed This . 4-2-22*/
				u8MBQueryRegisterAddressCounter = 0;
				gu32MBRTUClientConnectedFlag = 0;
				gu32MBRTUDataByteCounter = 0;
				//master.u8MBOperationStatus = 2;
			}

			resetModbusPort();
			master.u32PollDelay = gu32Modbus485PollingTime;//ONE_SEC;
			modbusState = enmMODBUS_IDLE;
			memset(master.u8SlaveResponseArray,0x00,sizeof(master.u8SlaveResponseArray));
		}
			break;

		default:
			modbusState = enmMODBUS_IDLE;
			break;
	}
}

/******************************************************************************
* Function :resetModbusPort
*//**
* \b Description:
*
* This function is used to reset MODBUS port pins.
*
*
* PRE-CONDITION: None
*
* POST-CONDITION: None
*
* @return 		None.
*
* \b Example Example:
* @code
*
* 	resetModbusPort();
*
* @endcode
*
* @see
*
* <br><b> - HISTORY OF CHANGES - </b>
*
* <table align="left" style="width:800px">
* <tr><td> Date       </td><td> Software Version </td><td> Initials </td><td> Description </td></tr>
* <tr><td> 08/03/2022 </td><td> 0.0.1            </td><td> HL100133 </td><td> Interface Created </td></tr>
* </table><br><br>
* <hr>
*
*******************************************************************************/
void resetModbusPort()
{
	HAL_GPIO_WritePin(MB_DE_GPIO_Port,MB_DE_Pin,GPIO_PIN_RESET);
	HAL_GPIO_WritePin(MB_RE_GPIO_Port,MB_RE_Pin,GPIO_PIN_SET);
}

/******************************************************************************
* Function :uploadSlaveData
*//**
* \b Description:
*
* This function is used to update GSM Payload array with modbus slave data .
*
*
* PRE-CONDITION: None
*
* POST-CONDITION: None
*
* @return 		None.
*
* \b Example Example:
* @code
*
* 	uploadSlaveData();
*
* @endcode
*
* @see
*
* <br><b> - HISTORY OF CHANGES - </b>
*
* <table align="left" style="width:800px">
* <tr><td> Date       </td><td> Software Version </td><td> Initials </td><td> Description </td></tr>
* <tr><td> 08/03/2022 </td><td> 0.0.1            </td><td> HL100133 </td><td> Interface Created </td></tr>
* </table><br><br>
* <hr>
*
*******************************************************************************/
void uploadSlaveData(void)
{
	/* Remove Loops and add status checking */
	static uint32_t Loopcounter = 0;
	static int32_t value;
	char buffer[6];

//	if(master.u8MBOperationStatus == 2)
//	{
		value = master.u32SlaveData[Loopcounter];    // Signed Integer
		if(Loopcounter == 0)
		{
			memset(gau8MBPayloadStringRT, 0x00, (1100 * sizeof(char)));
		}
		if(Loopcounter < gu32MBDataByteCounterLimitRTU)
		{
			/* Append Data to GSM Payload */
			if(gu32MBRTUDataStatus[Loopcounter] == 1)
			{
				/* Error Received for this data point .
				 * Error is parsed, indicate it in payload */
				strcat(gau8MBPayloadStringRT,"E");
				memset(buffer, 0x00, (6 * sizeof(char)));
				itoa(value,buffer,MB_PAYLOAD_RADIX);	   	   				  // Decimal String
				strcat(gau8MBPayloadStringRT,buffer);
				strcat(gau8MBPayloadStringRT,PAYLOAD_SEPARATOR);
			}
			else if(gu32MBRTUDataStatus[Loopcounter] == 2)
			{
				/* Coil Data .Represent in binary .
				 * */
				if(value < 0)
					value += 256;

				memset(buffer, 0x00, (6 * sizeof(char)));
				itoa(value,buffer,10);
				strcat(gau8MBPayloadStringRT,buffer);
				strcat(gau8MBPayloadStringRT,PAYLOAD_SEPARATOR);
			}
			else
			{
				memset(buffer, 0x00, (6 * sizeof(char)));
				itoa(value,buffer,MB_PAYLOAD_RADIX);
				strcat(gau8MBPayloadStringRT,buffer);
				strcat(gau8MBPayloadStringRT,PAYLOAD_SEPARATOR);
			}
			//master.u32SlaveData[Loopcounter] = 0;
			/*Added on 20-1-2020 */
			//gu32MBRTUDataStatus[Loopcounter] = 0;
			Loopcounter++;
		}
		else
		{
			/* GSM Payload is updated */
			//master.u8MBOperationStatus = 1;
			memcpy(gau8TempMBRTUPayloadString,gau8MBPayloadStringRT,sizeof(gau8MBPayloadStringRT));
			memset(gau8MBPayloadStringRT,0x00,1100 * sizeof(char));
			Loopcounter = 0;
			//gu32MBRTUDataByteCounter = 0;
		}
	//}
}


/******************************************************************************
* Function :uploadSlaveData
*//**
* \b Description:
*
* This function is used to initialise MODBUS Structure .
*
*
* PRE-CONDITION: None
*
* POST-CONDITION: None
*
* @return 		None.
*
* \b Example Example:
* @code
*
* 	uploadSlaveData();
*
* @endcode
*
* @see
*
* <br><b> - HISTORY OF CHANGES - </b>
*
* <table align="left" style="width:800px">
* <tr><td> Date       </td><td> Software Version </td><td> Initials </td><td> Description </td></tr>
* <tr><td> 08/03/2022 </td><td> 0.0.1            </td><td> HL100133 </td><td> Interface Created </td></tr>
* </table><br><br>
* <hr>
*
*******************************************************************************/
void setupModbusRTU(void)
{
	uint32_t temp = 0;
	master.u16NoRegisterstoFetch = gu32Modbus485RegisterFetch;;
	master.u8MBSlave_Address = gu32Modbus485SlaveID;
	master.u8MBOperationStatus = 1;
	master.u32PollDelay = gu32Modbus485PollingTime;

	for(temp = 0; temp < master.u16NoRegisterstoFetch ;temp++)
	{
		/*Calculates how many data values will be there in payload */
		gu32MBDataByteCounterLimitRTU += gu32MBRTUClientNoofPoints[temp];
	}
	master.u8MBNoQueryAttempts = 0;
}

//***************************************** End of File ********************************************************
