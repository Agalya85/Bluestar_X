/*******************************************************************************
* Title                 :   EEPROM Over I2C interface
* Filename              :   user_eeprom.c
* Author                :   Hrishikesh Limaye
* Origin Date           :   1/6/2019
* Version               :   1.0.0
* Compiler              :
* Target                :   STM32F4xx - Tor4GEth
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
/******************************************************************************

 *	EEPROM - AT24C08
 *	Communication - I2C
 *
 *	Following data to be stored in EEPROM
 *
 *	1. Mobile Number length 	0x02
 *	2. URL string Length		0x04
 *	3. APN						0x08
 *	4. On time frequency		0x0A
 *	5. OFF time frequency		0x0D
 *
 *	6. Mobile number string 	0x20 onwards
 *	7. URL						0x50 Onwards
 *	8. APN		 				0x250 Onwards
 *	9. On time frequency		0x350 Onwards
 *	10. OFF time frequency		0x360 Onwards
 *
 *	## Memory Operation Indicator Array ##
 *
 *	 gau8MemoryOperationIndicator [0] = Signature Byte
 *	 gau8MemoryOperationIndicator [1] = Mobile Number Length;
	 gau8MemoryOperationIndicator [2] = Network APN Length;
	 gau8MemoryOperationIndicator [3] = Server URL Length;
	 gau8MemoryOperationIndicator [4] = On Frequency Length;
	 gau8MemoryOperationIndicator [5] = Off Frequency Length;
	 gau8MemoryOperationIndicator [6] = Mobile Number;
	 gau8MemoryOperationIndicator [7] = Network AUN;
	 gau8MemoryOperationIndicator [8] = Server URL;
	 gau8MemoryOperationIndicator [9] = Upload On Freq;
	 gau8MemoryOperationIndicator [10] = Upload Off Freq;
 */

/*************** FILE REVISION LOG *****************************************
*
*    Date    Version   Author         	  Description
*  01/06/19   1.0.0    Hrishikesh Limaye   Initial Release.
*
*******************************************************************************/

/** @file  user_eeprom.c
 *  @brief Utilities for EEPROM interfaced over I2C
 */

/******************************************************************************
* Includes
*******************************************************************************/
#include "main.h"
#include "i2c.h"
#include "applicationdefines.h"
#include "user_timer.h"
#include "externs.h"
#include <string.h>
#include <stdlib.h>
#include "user_eeprom.h"
#include "errorhandling.h"
#include "gsmSim7600.h"
/******************************************************************************
* Module Preprocessor Constants
*******************************************************************************/
/**
 * Doxygen tag for documenting variables and constants
 */
#define CONFIG_MEMLOC_NWLEN		512//(0x3E80)
#define CONFIG_MEMLOC_MB485LEN	544//(0x3E88)
#define CONFIG_MEMLOC_MBTCPLEN	576//(0x3E90)
#define CONFIG_MEMLOC_NW 	    800//(0x733C)
#define CONFIG_MEMLOC_MB485 	1824//(0x7530)
#define CONFIG_MEMLOC_MBTCP 	1952//(0x7918)

#define CONFIG_MEMLOC_SIGN		(704)

/******************************************************************************
* Module Preprocessor Macros
*******************************************************************************/

/******************************************************************************
* Module Typedefs
*******************************************************************************/

/******************************************************************************
* Module Variable Definitions
*******************************************************************************/
char gau8ConfigurationSignature[1]= "Y";
char gau8DeviceSignature[1]= "@";				// Make sure gu8DeviceSignature[1] ="EEPROM_SIGNATURE_BYTE"
uint8_t gu8MemoryOperationStatus = 0;
uint8_t gu8MemoryWrOperationFlag = 0;
uint8_t gu8MemoryRdOperationFlag = 0;
uint8_t gau8MemoryOperationWrite[I2CMEM_MAX_OPRATIONS]={0};//{FALSE}; // 12 for non config operations
uint8_t gau8MemoryOperationRead[I2CMEM_MAX_OPRATIONS]={0};//{FALSE};
uint8_t u8WrMemLoopCounter = 0;
uint8_t u8RdMemLoopCounter = 0;
uint8_t gu32MemoryOperation = EEPROM_OPR_IDLE;
uint8_t gu8RestoreSystemSettings = FALSE;
uint8_t gu8SignatureReadFlag = FALSE;
uint8_t gu8PageWriteCycle = FALSE;
uint8_t gu8PageReadCycle = 3;
uint8_t gu8OperateSystemStatus = TRUE;

uint32_t gu8PageReadCounter = 0;
uint32_t gu8PageWriteCounter = 0;  				// Tracks Page writing 0 - First Page 1- Second page ... N -nth page
uint32_t gu32PageWriteSize = 0;
uint32_t gu32MemStatus;
uint32_t urlLength = 0;
uint32_t availableSize = 0;
uint32_t pageWriteSize = 0;
uint32_t lastPageWriteSize = 0;
uint32_t cycleRequired = 0;

/* Config */
uint32_t dataLength = 0;

strctMemoryLayout strI2cEeprom;
volatile uint32_t u32DefautParamWriteStatus = FALSE;
volatile uint8_t gu8MemoryCycleComplete = TRUE;//FALSE;

/* EEPROM operation is hampered due to
 * serial flash data read .
 * */
volatile uint32_t gu32EEPROMInitforSerialFlash = 0;


extern strctGSM gsmInstance;
/******************************************************************************
* Function Prototypes
*******************************************************************************/

/******************************************************************************
* Function Definitions
*******************************************************************************/

/******************************************************************************
* Function : writeToMemory()
*//**
* \b Description:
*
* This function is used to  Initiate Memory Write Operation over I2C.
*
* 	|  Loop Counter | Parameter Details    |
	|_______________|______________________|
	| 	 [1] 		| Mobile Number Length;|
	|	 [2]  		| Network APN Length;  |
	|	 [3]  	    | Server URL Length;   |
	|	 [4]  		| On Frequency Length; |
	|	 [5]        | Off Frequency Length;|
	|    [6]        | Mobile Number;       |
	|	 [7]        | Network APN;         |
	|	 [8]        | Server URL;          |
	|	 [9]        | Upload On Freq;      |
	|	 [10]       | Upload Off Freq;     |
	|______________________________________|
*
* PRE-CONDITION: Initilize used I2C peripheral , Enable interrupts
*
* POST-CONDITION: Write data to eeprom
*
*
* @return 		None.
*
* \b Example Example:
* @code
*   gu32MemoryOperation = EEPROM_OPR_WRITE;
*   gu8MemoryOperationStatus = 0;
*   gau8MemoryOperationWrite[Parameter Counter] = TRUE;
* 	writeToMemory();
*
* @endcode
*
* @see I2C
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
void writeToMemory(void)
{
	if((gu32MemoryOperation != EEPROM_OPR_WRITE) || (HAL_I2C_IsDeviceReady(& hi2c1 ,EEPROM_ADDRESS , 3 ,HAL_MAX_DELAY)) != HAL_OK)
		return;

	if((gu8MemoryOperationStatus == 1))
	{
		if(gu32I2CMemoryOperationTimeout == 0)
		{
			/* if memory does not respond / DNP case*/
			u8WrMemLoopCounter ++;
			assertError(enmTORERRORS_MEM_I2C,enmERRORSTATE_ACTIVE);
			gu8MemoryOperationStatus = 0;
		}
	}
	else
	{
		if(gau8MemoryOperationWrite[u8WrMemLoopCounter] == TRUE)
		{
			gu8MemoryOperationStatus = 1;
			gu32I2CMemoryOperationTimeout = TEN_SEC;
			switch(u8WrMemLoopCounter)
			{
				case I2C_MEM_DEVSIGNATURE:
					if(HAL_I2C_Mem_Write_IT(&hi2c1, EEPROM_ADDRESS, 0,I2C_MEMADD_SIZE_16BIT,
							(uint8_t *)&gau8DeviceSignature,strlen(gau8DeviceSignature)) != HAL_OK)
					{
						assertError(enmTORERRORS_MEM_I2C,enmERRORSTATE_ACTIVE);
					}
					else
						assertError(enmTORERRORS_MEM_I2C,enmERRORSTATE_NOERROR);
					break;

				case I2C_MEM_MOBILELENGTH:
					if(HAL_I2C_Mem_Write_IT(&hi2c1, EEPROM_ADDRESS, 1,I2C_MEMADD_SIZE_16BIT,
					(uint8_t *)&strI2cEeprom.u32WrMobileNumberLength,sizeof(strI2cEeprom.u32WrMobileNumberLength)) != HAL_OK)
					{
						assertError(enmTORERRORS_MEM_I2C,enmERRORSTATE_ACTIVE);
					}
					else
						assertError(enmTORERRORS_MEM_I2C,enmERRORSTATE_NOERROR);
				break;

				case I2C_MEM_APNSTRLENGTH:
					if(HAL_I2C_Mem_Write_IT(&hi2c1, EEPROM_ADDRESS, 5,I2C_MEMADD_SIZE_16BIT,
					(uint8_t *)&strI2cEeprom.u32WrNetworkAPNLength,(strI2cEeprom.u32WrNetworkAPNLength)) != HAL_OK)
					{
						assertError(enmTORERRORS_MEM_I2C,enmERRORSTATE_ACTIVE);
					}
					else
						assertError(enmTORERRORS_MEM_I2C,enmERRORSTATE_NOERROR);
				break;

				case I2C_MEM_URLSTRLENGTH:
					if(HAL_I2C_Mem_Write_IT(&hi2c1, EEPROM_ADDRESS, 9,I2C_MEMADD_SIZE_16BIT,
					(uint8_t *)&strI2cEeprom.u32WrServerURLLength,sizeof(strI2cEeprom.u32WrServerURLLength)) != HAL_OK)
					{
						assertError(enmTORERRORS_MEM_I2C,enmERRORSTATE_ACTIVE);
					}
					else
						assertError(enmTORERRORS_MEM_I2C,enmERRORSTATE_NOERROR);
				break;

				case I2C_MEM_ONFREQLENGTH:
					if(HAL_I2C_Mem_Write_IT(&hi2c1, EEPROM_ADDRESS, 13,I2C_MEMADD_SIZE_16BIT,
					(uint8_t *)&strI2cEeprom.u32WrOnFrequencyLength,sizeof(strI2cEeprom.u32WrOnFrequencyLength)) != HAL_OK)
					{
						assertError(enmTORERRORS_MEM_I2C,enmERRORSTATE_ACTIVE);
					}
					else
						assertError(enmTORERRORS_MEM_I2C,enmERRORSTATE_NOERROR);
				break;

				case I2C_MEM_OFFFREQLENGTH:
					if(HAL_I2C_Mem_Write_IT(&hi2c1, EEPROM_ADDRESS, 18,I2C_MEMADD_SIZE_16BIT,
					(uint8_t *)&strI2cEeprom.u32WrOffFrequencyLength,sizeof(strI2cEeprom.u32WrOffFrequencyLength)) != HAL_OK)
					{
						assertError(enmTORERRORS_MEM_I2C,enmERRORSTATE_ACTIVE);
					}
					else
						assertError(enmTORERRORS_MEM_I2C,enmERRORSTATE_NOERROR);
				break;

				case I2C_MEM_MOBILENOSTR:
					if(HAL_I2C_Mem_Write_IT(&hi2c1, EEPROM_ADDRESS,33,I2C_MEMADD_SIZE_16BIT,
							(uint8_t *)&strI2cEeprom.pu8WrMobileNumber,sizeof(strI2cEeprom.pu8WrMobileNumber)))
					{
						assertError(enmTORERRORS_MEM_I2C,enmERRORSTATE_ACTIVE);
					}
					else
						assertError(enmTORERRORS_MEM_I2C,enmERRORSTATE_NOERROR);
				break;

				case I2C_MEM_APNSTR:

					if(HAL_I2C_Mem_Write_IT(&hi2c1, EEPROM_ADDRESS, 65,I2C_MEMADD_SIZE_16BIT,
					(uint8_t *)&gau8GSM4G_apn,strlen(gau8GSM4G_apn)) != HAL_OK)
					{
						assertError(enmTORERRORS_MEM_I2C,enmERRORSTATE_ACTIVE);
					}
					else
						assertError(enmTORERRORS_MEM_I2C,enmERRORSTATE_NOERROR);
				break;

				case I2C_MEM_URLSTR:
					if(gu8PageWriteCounter == 0)
						gu8PageWriteCycle = FALSE;

					if(gu8PageWriteCycle == FALSE)
					{
						urlLength = strlen((char *)gu8NewURL);
						availableSize = getAvailableSpaceInMemoryPage(MEM_URL_BASE_ADD); //128
						pageWriteSize = getFirstPageWriteSize(availableSize);
						lastPageWriteSize = getLastPageWriteSize(pageWriteSize ,urlLength);
						cycleRequired = getPageWriteCyclesRequired(pageWriteSize , urlLength);
						gu8PageWriteCycle = TRUE;
					}

					if(gu8PageWriteCounter == 0)
						gu32PageWriteSize = pageWriteSize;
					else if(gu8PageWriteCounter == (cycleRequired - 1))
						gu32PageWriteSize = lastPageWriteSize;
					else
						gu32PageWriteSize = AT24XX_PAGESIZE;

					if(gu8PageWriteCycle == TRUE && gu8PageWriteCounter == 0)
					{
						/* Attempting First Page Write Cycle */ //128
						if(HAL_I2C_Mem_Write_IT(&hi2c1, EEPROM_ADDRESS,MEM_URL_BASE_ADD,I2C_MEMADD_SIZE_16BIT,
								(uint8_t *)&gu8NewURL,gu32PageWriteSize) != HAL_OK)
						{
							assertError(enmTORERRORS_MEM_I2C,enmERRORSTATE_ACTIVE);
						}
						else
							assertError(enmTORERRORS_MEM_I2C,enmERRORSTATE_NOERROR);
					}
					else
					{
						/* Attempt Next Page Write Cycle */ //128
						if(HAL_I2C_Mem_Write_IT(&hi2c1, EEPROM_ADDRESS,(MEM_URL_BASE_ADD + (gu8PageWriteCounter * AT24XX_PAGESIZE)) ,I2C_MEMADD_SIZE_16BIT,
								(uint8_t *)&gu8NewURL[gu8PageWriteCounter * AT24XX_PAGESIZE],gu32PageWriteSize) != HAL_OK)
						{
							assertError(enmTORERRORS_MEM_I2C,enmERRORSTATE_ACTIVE);
						}
						else
							assertError(enmTORERRORS_MEM_I2C,enmERRORSTATE_NOERROR);
					}
				break;

				case I2C_MEM_ONFREQ:
					if(HAL_I2C_Mem_Write_IT(&hi2c1, EEPROM_ADDRESS, 22,I2C_MEMADD_SIZE_16BIT,
					(uint8_t *)&strI2cEeprom.pu8Wr8UploadOnFreq,sizeof(strI2cEeprom.pu8Wr8UploadOnFreq)) != HAL_OK)
					{
						assertError(enmTORERRORS_MEM_I2C,enmERRORSTATE_ACTIVE);
					}
					else
						assertError(enmTORERRORS_MEM_I2C,enmERRORSTATE_NOERROR);
				break;

				case I2C_MEM_OFFFREQ:
					if(HAL_I2C_Mem_Write_IT(&hi2c1, EEPROM_ADDRESS, 27,I2C_MEMADD_SIZE_16BIT,
					(uint8_t *)&strI2cEeprom.pu8WrUploadOffFreq,sizeof(strI2cEeprom.pu8WrUploadOffFreq)) != HAL_OK)
					{
						assertError(enmTORERRORS_MEM_I2C,enmERRORSTATE_ACTIVE);
					}
					else
						assertError(enmTORERRORS_MEM_I2C,enmERRORSTATE_NOERROR);
				break;

				/* Added on 19/7/2012 by 100133 for Config Storage */
				case I2C_MEM_CONFIGSIGN:
					if(HAL_I2C_Mem_Write_IT(&hi2c1, EEPROM_ADDRESS, CONFIG_MEMLOC_SIGN,I2C_MEMADD_SIZE_16BIT,
						(uint8_t *)&gau8ConfigurationSignature,strlen(gau8ConfigurationSignature)) != HAL_OK)
					{
						assertError(enmTORERRORS_MEM_I2C,enmERRORSTATE_ACTIVE);
					}
					else
						assertError(enmTORERRORS_MEM_I2C,enmERRORSTATE_NOERROR);
				break;

				case I2C_MEM_CONFIGNWLEN:
					if(HAL_I2C_Mem_Write_IT(&hi2c1, EEPROM_ADDRESS, CONFIG_MEMLOC_NWLEN,I2C_MEMADD_SIZE_16BIT,
					(uint8_t *)&ga8ConfigNWLength,sizeof(ga8ConfigNWLength)) != HAL_OK)
					{
						assertError(enmTORERRORS_MEM_I2C,enmERRORSTATE_ACTIVE);
					}
					else
						assertError(enmTORERRORS_MEM_I2C,enmERRORSTATE_NOERROR);
				break;

				case I2C_MEM_CONFIGMB485LEN:
					if(HAL_I2C_Mem_Write_IT(&hi2c1, EEPROM_ADDRESS, CONFIG_MEMLOC_MB485LEN,I2C_MEMADD_SIZE_16BIT,
					(uint8_t *)&ga8ConfigMB485Length,sizeof(ga8ConfigMB485Length)) != HAL_OK)
					{
						assertError(enmTORERRORS_MEM_I2C,enmERRORSTATE_ACTIVE);
					}
					else
						assertError(enmTORERRORS_MEM_I2C,enmERRORSTATE_NOERROR);
				break;

				case I2C_MEM_CONFIGMBTCPLEN:
					if(HAL_I2C_Mem_Write_IT(&hi2c1, EEPROM_ADDRESS, CONFIG_MEMLOC_MBTCPLEN,I2C_MEMADD_SIZE_16BIT,
					(uint8_t *)&ga8ConfigMBTCPLength,sizeof(ga8ConfigMB485Length)) != HAL_OK)
					{
						assertError(enmTORERRORS_MEM_I2C,enmERRORSTATE_ACTIVE);
					}
					else
						assertError(enmTORERRORS_MEM_I2C,enmERRORSTATE_NOERROR);
				break;

				case I2C_MEM_CONFIGNW:

					if(gu8PageWriteCounter == 0)
						gu8PageWriteCycle = FALSE;

					if(gu8PageWriteCycle == FALSE)
					{
						dataLength = strlen((char *)nwconfig);
						availableSize = getAvailableSpaceInMemoryPage(CONFIG_MEMLOC_NW); //CONFIG_MEMLOC_NW
						pageWriteSize = getFirstPageWriteSize(availableSize);
						lastPageWriteSize = getLastPageWriteSize(pageWriteSize ,dataLength);
						cycleRequired = getPageWriteCyclesRequired(pageWriteSize , dataLength);
						gu8PageWriteCycle = TRUE;
					}

					if(gu8PageWriteCounter == 0)
						gu32PageWriteSize = pageWriteSize;
					else if(gu8PageWriteCounter == (cycleRequired - 1))
						gu32PageWriteSize = lastPageWriteSize;
					else
						gu32PageWriteSize = AT24XX_PAGESIZE;

					if(gu8PageWriteCycle == TRUE && gu8PageWriteCounter == 0)
					{

						/* Attempting First Page Write Cycle */ //CONFIG_MEMLOC_NW
						if(HAL_I2C_Mem_Write_IT(&hi2c1, EEPROM_ADDRESS,CONFIG_MEMLOC_NW,I2C_MEMADD_SIZE_16BIT,
								(uint8_t *)&nwconfig,gu32PageWriteSize) != HAL_OK)
						{
							assertError(enmTORERRORS_MEM_I2C,enmERRORSTATE_ACTIVE);
						}
						else
							assertError(enmTORERRORS_MEM_I2C,enmERRORSTATE_NOERROR);
					}
					else
					{
						/* Attempt Next Page Write Cycle */ //CONFIG_MEMLOC_NW
						if(HAL_I2C_Mem_Write_IT(&hi2c1, EEPROM_ADDRESS,(CONFIG_MEMLOC_NW + (gu8PageWriteCounter * AT24XX_PAGESIZE)) ,I2C_MEMADD_SIZE_16BIT,
								(uint8_t *)&nwconfig[gu8PageWriteCounter * AT24XX_PAGESIZE],gu32PageWriteSize) != HAL_OK)
						{
							assertError(enmTORERRORS_MEM_I2C,enmERRORSTATE_ACTIVE);
						}
						else
							assertError(enmTORERRORS_MEM_I2C,enmERRORSTATE_NOERROR);
					}

				break;

				case I2C_MEM_CONFIGMB485:

					if(gu8PageWriteCounter == 0)
						gu8PageWriteCycle = FALSE;

					if(gu8PageWriteCycle == FALSE)
					{
						dataLength = strlen((char *)mb485config);
						availableSize = getAvailableSpaceInMemoryPage(CONFIG_MEMLOC_MB485); //CONFIG_MEMLOC_MB485
						pageWriteSize = getFirstPageWriteSize(availableSize);
						lastPageWriteSize = getLastPageWriteSize(pageWriteSize ,dataLength);
						cycleRequired = getPageWriteCyclesRequired(pageWriteSize , dataLength);
						gu8PageWriteCycle = TRUE;
					}

					if(gu8PageWriteCounter == 0)
						gu32PageWriteSize = pageWriteSize;
					else if(gu8PageWriteCounter == (cycleRequired - 1))
						gu32PageWriteSize = lastPageWriteSize;
					else
						gu32PageWriteSize = AT24XX_PAGESIZE;

					if(gu8PageWriteCycle == TRUE && gu8PageWriteCounter == 0)
					{

						/* Attempting First Page Write Cycle */ //CONFIG_MEMLOC_MB485
						if(HAL_I2C_Mem_Write_IT(&hi2c1, EEPROM_ADDRESS,CONFIG_MEMLOC_MB485,I2C_MEMADD_SIZE_16BIT,
								(uint8_t *)&mb485config,gu32PageWriteSize) != HAL_OK)
						{
							assertError(enmTORERRORS_MEM_I2C,enmERRORSTATE_ACTIVE);
						}
						else
							assertError(enmTORERRORS_MEM_I2C,enmERRORSTATE_NOERROR);
					}
					else
					{
						/* Attempt Next Page Write Cycle */ //CONFIG_MEMLOC_NW
						if(HAL_I2C_Mem_Write_IT(&hi2c1, EEPROM_ADDRESS,(CONFIG_MEMLOC_MB485 + (gu8PageWriteCounter * AT24XX_PAGESIZE)) ,I2C_MEMADD_SIZE_16BIT,
								(uint8_t *)&mb485config[gu8PageWriteCounter * AT24XX_PAGESIZE],gu32PageWriteSize) != HAL_OK)
						{
							assertError(enmTORERRORS_MEM_I2C,enmERRORSTATE_ACTIVE);
						}
						else
							assertError(enmTORERRORS_MEM_I2C,enmERRORSTATE_NOERROR);
					}

				break;

				case I2C_MEM_CONFIGMBTCP:

					if(gu8PageWriteCounter == 0)
						gu8PageWriteCycle = FALSE;

					if(gu8PageWriteCycle == FALSE)
					{
						dataLength = strlen((char *)mbTCPconfig);
						availableSize = getAvailableSpaceInMemoryPage(CONFIG_MEMLOC_MBTCP); //CONFIG_MEMLOC_NW
						pageWriteSize = getFirstPageWriteSize(availableSize);
						lastPageWriteSize = getLastPageWriteSize(pageWriteSize ,dataLength);
						cycleRequired = getPageWriteCyclesRequired(pageWriteSize , dataLength);
						gu8PageWriteCycle = TRUE;
					}

					if(gu8PageWriteCounter == 0)
						gu32PageWriteSize = pageWriteSize;
					else if(gu8PageWriteCounter == (cycleRequired - 1))
						gu32PageWriteSize = lastPageWriteSize;
					else
						gu32PageWriteSize = AT24XX_PAGESIZE;

					if(gu8PageWriteCycle == TRUE && gu8PageWriteCounter == 0)
					{

						/* Attempting First Page Write Cycle */ //CONFIG_MEMLOC_NW
						if(HAL_I2C_Mem_Write_IT(&hi2c1, EEPROM_ADDRESS,CONFIG_MEMLOC_MBTCP,I2C_MEMADD_SIZE_16BIT,
								(uint8_t *)&mbTCPconfig,gu32PageWriteSize) != HAL_OK)
						{
							assertError(enmTORERRORS_MEM_I2C,enmERRORSTATE_ACTIVE);
						}
						else
							assertError(enmTORERRORS_MEM_I2C,enmERRORSTATE_NOERROR);
					}
					else
					{
						/* Attempt Next Page Write Cycle */ //CONFIG_MEMLOC_NW
						if(HAL_I2C_Mem_Write_IT(&hi2c1, EEPROM_ADDRESS,(CONFIG_MEMLOC_MBTCP + (gu8PageWriteCounter * AT24XX_PAGESIZE)) ,I2C_MEMADD_SIZE_16BIT,
								(uint8_t *)&mbTCPconfig[gu8PageWriteCounter * AT24XX_PAGESIZE],gu32PageWriteSize) != HAL_OK)
						{
							assertError(enmTORERRORS_MEM_I2C,enmERRORSTATE_ACTIVE);
						}
						else
							assertError(enmTORERRORS_MEM_I2C,enmERRORSTATE_NOERROR);
					}
				break;

				default :
					/* Undefined Task */
				break;

				gu8MemoryOperationStatus = 1;
			}
		}
		else
		{
			u8WrMemLoopCounter++; 	/* Increment Loop Counter if operation is not to be performed .*/
			if(gu8MemoryOperationStatus)gu8MemoryOperationStatus = 0;
			if(u8WrMemLoopCounter >= I2CMEM_MAX_OPRATIONS)
			{
				u8WrMemLoopCounter = 0;
				if(gu8MemoryCycleComplete == FALSE)
					gu8MemoryCycleComplete = TRUE;
			}
		}
	}
}

/******************************************************************************
* Function : readFromMemory()
*//**
* \b Description:
*
* This function is used to  Initiate Memory Read Operation over I2C
*
* 	|  Loop Counter | Parameter Details    |
	|_______________|______________________|
	| 	 [1] 		| Mobile Number Length;|
	|	 [2]  		| Network APN Length;  |
	|	 [3]  	    | Server URL Length;   |
	|	 [4]  		| On Frequency Length; |
	|	 [5]        | Off Frequency Length;|
	|    [6]        | Mobile Number;       |
	|	 [7]        | Network APN;         |
	|	 [8]        | Server URL;          |
	|	 [9]        | Upload On Freq;      |
	|	 [10]       | Upload Off Freq;     |
	|______________________________________|
*
* PRE-CONDITION: Initilize used I2C peripheral , Enable interrupts
*
* POST-CONDITION: Write data to eeprom
*
*
* @return 		None.
*
* \b Example Example:
* @code
*   gu32MemoryOperation = EEPROM_OPR_READ;
*   gu8MemoryOperationStatus = 0;
*   gau8MemoryOperationRead[Parameter Counter] = TRUE;
* 	readFromMemory();
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
* <tr><td> 12/03/2020 </td><td> 0.0.1            </td><td> HL100133 </td><td> Mobile Number Read Problem .
 	 	 	 	 	 	 	 	 	 										  Incorrect Read length passed
 	 	 	 	 	 	 	 	 	 										  Solved.Tested </td></tr>
* </table><br><br>
* <hr>
*
*******************************************************************************/
void readFromMemory(void)
{
	if((gu32MemoryOperation != EEPROM_OPR_READ) || HAL_I2C_IsDeviceReady ( & hi2c1 ,EEPROM_ADDRESS , 3 ,HAL_MAX_DELAY))
		return;

	if((gu8MemoryOperationStatus == 1))
	{
		/*Unable to operate memory . Timeout */
		if(gu32I2CMemoryOperationTimeout == 0)
		{
			/* Log Error */
			assertError(enmTORERRORS_MEM_I2C,enmERRORSTATE_ACTIVE);
			u8RdMemLoopCounter++;
			gu8MemoryOperationStatus = 0;
		}
	}
	else
	{
		if(gau8MemoryOperationRead[u8RdMemLoopCounter] == TRUE)
		{
			gu8MemoryOperationStatus = 1;
			gu32I2CMemoryOperationTimeout = TEN_SEC;
			switch(u8RdMemLoopCounter)
			{
				case I2C_MEM_DEVSIGNATURE:
					if(HAL_I2C_Mem_Read_IT(&hi2c1, EEPROM_ADDRESS, 0,I2C_MEMADD_SIZE_16BIT,
							(uint8_t *)strI2cEeprom.pu8DeviceSignature,1) != HAL_OK)
					{
						assertError(enmTORERRORS_MEM_I2C,enmERRORSTATE_ACTIVE);
					}
					else
						assertError(enmTORERRORS_MEM_I2C,enmERRORSTATE_NOERROR);
				break;

				case I2C_MEM_MOBILELENGTH:
					if(HAL_I2C_Mem_Read_IT(&hi2c1, EEPROM_ADDRESS,1,
							I2C_MEMADD_SIZE_16BIT,(uint8_t *)&strI2cEeprom.u32RdMobileNumberLength, 4) != HAL_OK)
					{
						assertError(enmTORERRORS_MEM_I2C,enmERRORSTATE_ACTIVE);
					}
					else
						assertError(enmTORERRORS_MEM_I2C,enmERRORSTATE_NOERROR);
				break;

				case I2C_MEM_APNSTRLENGTH:
					if(HAL_I2C_Mem_Read_IT(&hi2c1, EEPROM_ADDRESS,5,
							I2C_MEMADD_SIZE_16BIT,(uint8_t *)&strI2cEeprom.u32RdNetworkAPNLength, 4) != HAL_OK)
					{
						assertError(enmTORERRORS_MEM_I2C,enmERRORSTATE_ACTIVE);
					}
					else
						assertError(enmTORERRORS_MEM_I2C,enmERRORSTATE_NOERROR);
				break;

				case I2C_MEM_URLSTRLENGTH:
					if(HAL_I2C_Mem_Read_IT(&hi2c1, EEPROM_ADDRESS,9,
							I2C_MEMADD_SIZE_16BIT,(uint8_t *)&strI2cEeprom.u32RdServerURLLength, 4) != HAL_OK)
					{
						assertError(enmTORERRORS_MEM_I2C,enmERRORSTATE_ACTIVE);
					}
					else
						assertError(enmTORERRORS_MEM_I2C,enmERRORSTATE_NOERROR);
				break;

				case I2C_MEM_ONFREQLENGTH:
					if(HAL_I2C_Mem_Read_IT(&hi2c1, EEPROM_ADDRESS,13,
							I2C_MEMADD_SIZE_16BIT,(uint8_t *)&strI2cEeprom.u32RdOnFrequencyLength, 4) != HAL_OK)
					{
						assertError(enmTORERRORS_MEM_I2C,enmERRORSTATE_ACTIVE);
					}
					else
						assertError(enmTORERRORS_MEM_I2C,enmERRORSTATE_NOERROR);
				break;

				case I2C_MEM_OFFFREQLENGTH:
					if(HAL_I2C_Mem_Read_IT(&hi2c1, EEPROM_ADDRESS,18,
							I2C_MEMADD_SIZE_16BIT,(uint8_t *)&strI2cEeprom.u32RdOffFrequencyLength, 4) != HAL_OK)
					{
						assertError(enmTORERRORS_MEM_I2C,enmERRORSTATE_ACTIVE);
					}
					else
						assertError(enmTORERRORS_MEM_I2C,enmERRORSTATE_NOERROR);
				break;

				case I2C_MEM_MOBILENOSTR:
					if((strI2cEeprom.u32RdMobileNumberLength != 0) && (strI2cEeprom.u32RdMobileNumberLength < 20))
					{
						if(HAL_I2C_Mem_Read_IT(&hi2c1, EEPROM_ADDRESS,33,
								I2C_MEMADD_SIZE_16BIT,(uint8_t *)strI2cEeprom.pu8MobileNumber,strI2cEeprom.u32RdMobileNumberLength) != HAL_OK)
						{
							assertError(enmTORERRORS_MEM_I2C,enmERRORSTATE_ACTIVE);
						}
						else
							assertError(enmTORERRORS_MEM_I2C,enmERRORSTATE_NOERROR);
					}
					else
						gau8MemoryOperationRead[u8RdMemLoopCounter++] = FALSE;
				break;

				case I2C_MEM_APNSTR:
					if(strI2cEeprom.u32RdNetworkAPNLength == 0 || (strI2cEeprom.u32RdNetworkAPNLength >= 151))
					{
						// Nothing to Read . Update Read State and Go to next state
						gau8MemoryOperationRead[u8RdMemLoopCounter++] = FALSE;

					}
					else
					{
						if(HAL_I2C_Mem_Read_IT(&hi2c1, EEPROM_ADDRESS,65,
								I2C_MEMADD_SIZE_16BIT,(uint8_t *)strI2cEeprom.pu8NetworkAPN,
								strI2cEeprom.u32RdNetworkAPNLength) != HAL_OK)
						{
							assertError(enmTORERRORS_MEM_I2C,enmERRORSTATE_ACTIVE);
						}
						else
							assertError(enmTORERRORS_MEM_I2C,enmERRORSTATE_NOERROR);
					}

				break;

				case I2C_MEM_URLSTR:
					/*Add strlen(strI2cEeprom.u32RdServerURLLength) == NULL check */
					if(strI2cEeprom.u32RdServerURLLength == 0 || (strI2cEeprom.u32RdServerURLLength >= 150))
					{
						// Nothing to Read . Update Read State and Go to next state
						gau8MemoryOperationRead[u8RdMemLoopCounter++] = FALSE;

					}
					else
					{
						if(HAL_I2C_Mem_Read_IT(&hi2c1, EEPROM_ADDRESS,MEM_URL_BASE_ADD,I2C_MEMADD_SIZE_16BIT,
								(uint8_t *)&strI2cEeprom.pu8RdServerURL,strI2cEeprom.u32RdServerURLLength) != HAL_OK)
						{
							assertError(enmTORERRORS_MEM_I2C,enmERRORSTATE_ACTIVE);
						}
						else
							assertError(enmTORERRORS_MEM_I2C,enmERRORSTATE_NOERROR);
					}
				break;

				case I2C_MEM_ONFREQ:
					if(HAL_I2C_Mem_Read_IT(&hi2c1, EEPROM_ADDRESS,22,
							I2C_MEMADD_SIZE_16BIT,(uint8_t *)strI2cEeprom.pu8RdUploadOnFreq,
								4) != HAL_OK)
					{
						assertError(enmTORERRORS_MEM_I2C,enmERRORSTATE_ACTIVE);
					}
					else
						assertError(enmTORERRORS_MEM_I2C,enmERRORSTATE_NOERROR);
				break;

				case I2C_MEM_OFFFREQ:
					if(HAL_I2C_Mem_Read_IT(&hi2c1, EEPROM_ADDRESS,27,
						I2C_MEMADD_SIZE_16BIT,(uint8_t *)strI2cEeprom.pu8RdUploadOffFreq,
							4) != HAL_OK)
					{
						assertError(enmTORERRORS_MEM_I2C,enmERRORSTATE_ACTIVE);
					}
					else
						assertError(enmTORERRORS_MEM_I2C,enmERRORSTATE_NOERROR);
				break;

				/* Added on 19/7/2012 by 100133 for Config Storage */
				case I2C_MEM_CONFIGSIGN:
					if(HAL_I2C_Mem_Read_IT(&hi2c1, EEPROM_ADDRESS, CONFIG_MEMLOC_SIGN,I2C_MEMADD_SIZE_16BIT,
						(uint8_t *)strI2cEeprom.pu8ConfigurationSignature,1) != HAL_OK)
					{
						assertError(enmTORERRORS_MEM_I2C,enmERRORSTATE_ACTIVE);
					}
					else
						assertError(enmTORERRORS_MEM_I2C,enmERRORSTATE_NOERROR);
				break;

				case I2C_MEM_CONFIGNWLEN:
					if(HAL_I2C_Mem_Read_IT(&hi2c1, EEPROM_ADDRESS, CONFIG_MEMLOC_NWLEN,I2C_MEMADD_SIZE_16BIT,
					(uint8_t *)&strI2cEeprom.u32RdNetworkConfigLength,5) != HAL_OK)
					{
						assertError(enmTORERRORS_MEM_I2C,enmERRORSTATE_ACTIVE);
					}
					else
						assertError(enmTORERRORS_MEM_I2C,enmERRORSTATE_NOERROR);

				break;

				case I2C_MEM_CONFIGMB485LEN:
					if(HAL_I2C_Mem_Read_IT(&hi2c1, EEPROM_ADDRESS, CONFIG_MEMLOC_MB485LEN,I2C_MEMADD_SIZE_16BIT,
					(uint8_t *)&strI2cEeprom.u32RdModbus485ConfigLength,5) != HAL_OK)
					{
						assertError(enmTORERRORS_MEM_I2C,enmERRORSTATE_ACTIVE);
					}
					else
						assertError(enmTORERRORS_MEM_I2C,enmERRORSTATE_NOERROR);
				break;

				case I2C_MEM_CONFIGMBTCPLEN:
					if(HAL_I2C_Mem_Read_IT(&hi2c1, EEPROM_ADDRESS, CONFIG_MEMLOC_MBTCPLEN,I2C_MEMADD_SIZE_16BIT,
					(uint8_t *)&strI2cEeprom.u32RdModbusTCPConfigLength,5) != HAL_OK)
					{
						assertError(enmTORERRORS_MEM_I2C,enmERRORSTATE_ACTIVE);
					}
					else
						assertError(enmTORERRORS_MEM_I2C,enmERRORSTATE_NOERROR);
				break;

				case I2C_MEM_CONFIGNW:
					if((strI2cEeprom.u32RdNetworkConfigLength == 0) || (strI2cEeprom.u32RdNetworkConfigLength > 500))
					{
						// Nothing to Read . Update Read State and Go to next state
						gau8MemoryOperationRead[u8RdMemLoopCounter++] = FALSE;
					}
					else
					{
						if(HAL_I2C_Mem_Read_IT(&hi2c1, EEPROM_ADDRESS, CONFIG_MEMLOC_NW,I2C_MEMADD_SIZE_16BIT,
						(uint8_t *)&strI2cEeprom.pu8ConfigurationNetwork,strI2cEeprom.u32RdNetworkConfigLength) != HAL_OK)
						{
							assertError(enmTORERRORS_MEM_I2C,enmERRORSTATE_ACTIVE);
						}
						else
							assertError(enmTORERRORS_MEM_I2C,enmERRORSTATE_NOERROR);
					}
				break;

				case I2C_MEM_CONFIGMB485:
					if((strI2cEeprom.u32RdModbus485ConfigLength == 0) || (strI2cEeprom.u32RdModbus485ConfigLength > 1000))
					{
						// Nothing to Read . Update Read State and Go to next state
						gau8MemoryOperationRead[u8RdMemLoopCounter++] = FALSE;
					}
					else
					{
						if(HAL_I2C_Mem_Read_IT(&hi2c1, EEPROM_ADDRESS, CONFIG_MEMLOC_MB485,I2C_MEMADD_SIZE_16BIT,
						(uint8_t *)&strI2cEeprom.pu8ConfigurationMB485,strI2cEeprom.u32RdModbus485ConfigLength) != HAL_OK)
						{
							assertError(enmTORERRORS_MEM_I2C,enmERRORSTATE_ACTIVE);
						}
						else
							assertError(enmTORERRORS_MEM_I2C,enmERRORSTATE_NOERROR);
					}

				break;

				case I2C_MEM_CONFIGMBTCP:
					if((strI2cEeprom.u32RdModbusTCPConfigLength == 0) || (strI2cEeprom.u32RdModbusTCPConfigLength > 1000))
					{
						// Nothing to Read . Update Read State and Go to next state
						gau8MemoryOperationRead[u8RdMemLoopCounter++] = FALSE;
					}
					else
					{
						if(HAL_I2C_Mem_Read_IT(&hi2c1, EEPROM_ADDRESS, CONFIG_MEMLOC_MBTCP,I2C_MEMADD_SIZE_16BIT,
						(uint8_t *)&strI2cEeprom.pu8ConfigurationMBTCP,strI2cEeprom.u32RdModbusTCPConfigLength) != HAL_OK)
						{
							assertError(enmTORERRORS_MEM_I2C,enmERRORSTATE_ACTIVE);
						}
						else
							assertError(enmTORERRORS_MEM_I2C,enmERRORSTATE_NOERROR);
					}
				break;

				default:
					/* Illegal State Error */
					//Error_Callback(enmERROR_MEM_I2C);
					break;
			}
		}
		else
		{
			u8RdMemLoopCounter++; 	/* Increment Loop Counter if operation is not to be performed .*/
			if(gu8MemoryOperationStatus)gu8MemoryOperationStatus = 0;
			if(u8RdMemLoopCounter >= I2CMEM_MAX_OPRATIONS)
			{
				if(gu8RestoreSystemSettings == TRUE)
				{
					initSystemDefaultsfromMemory();
					gu8RestoreSystemSettings = FALSE;
				}
				u8RdMemLoopCounter = 0;
				if(gu8MemoryCycleComplete == FALSE)
					gu8MemoryCycleComplete = TRUE;
			}
		}
	}
}

/******************************************************************************
* Function : HAL_I2C_MemTxCpltCallback()
*//**
* \b Description:
*
* This function is Memory Tx Transfer completed callback (Non-Blocking).
*
*
* PRE-CONDITION: Initilize used I2C peripheral , Enable interrupts
*
* POST-CONDITION: Operates Write cycle for eeprom logic
*
*
* @return 		None.
*
* \b Example Example:
* @code
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
* <tr><td> 23/03/2020 </td><td> 0.0.1            </td><td> HL100133 </td><td> Reset System if its first Write cycle.
  	  	  	  	  	  	  	  	  	  										  Default Param Write.
  	  	  	  	  	  	  	  	  	  										  IDWT will reset the system </td></tr>
*
* </table><br><br>
* <hr>
*
*******************************************************************************/
void HAL_I2C_MemTxCpltCallback (I2C_HandleTypeDef *hi2c)
{
	if((gu8MemoryOperationStatus == 1) && (gu32I2CMemoryOperationTimeout !=0))
	{
		if(gau8MemoryOperationWrite[u8WrMemLoopCounter] == TRUE)
		{
			if(gu8PageWriteCycle == TRUE)
			{
				if(gu8PageWriteCounter > cycleRequired)
				{
					gau8MemoryOperationWrite[u8WrMemLoopCounter] = FALSE;
					u8WrMemLoopCounter ++;
					gu8PageWriteCounter  = 0;
				}
				else
					gu8PageWriteCounter ++;
			}
			else
			{
				/* Normal Mem Write Opr */
				gau8MemoryOperationWrite[u8WrMemLoopCounter] = FALSE;
				u8WrMemLoopCounter ++;
			}

			/* Bug : Does not iterate after 2
			 * Solved : 25052019 */
			if(u8WrMemLoopCounter >= I2CMEM_MAX_OPRATIONS)
			{
				/*  */
				gu32MemoryOperation = EEPROM_OPR_READ;
				/* Reset System if its first Write cycle. Default Param Write */
				if(u32DefautParamWriteStatus == TRUE)
					systemReset();						/* IDWT will reset the system */
				else
					initMemoryRead();

				u8WrMemLoopCounter = 0;
			}
		}
		gu8MemoryOperationStatus = 0;
		gu32I2CMemoryOperationTimeout = 0;
	}
}

/******************************************************************************
* Function : getDeviceSignatureFromMemory()
*//**
* \b Description:
*
* This function is used to get Signature byte from EEPROM.
* If signature is available , Device has been used earlier and
  system parameters are available in the memory . If signature
  is not available use default parameters
*
*
* PRE-CONDITION: Initilize used I2C peripheral , Enable interrupts
*
* POST-CONDITION: Device Signature is fetched from eeprom
*
*
* @return 		None.
*
* \b Example Example:
* @code
*
* @endcode
*	getDeviceSignatureFromMemory();
* @see
*
* <br><b> - HISTORY OF CHANGES - </b>
*
* <table align="left" style="width:800px">
* <tr><td> Date       </td><td> Software Version </td><td> Initials </td><td> Description </td></tr>
* <tr><td> 01/06/2019 </td><td> 0.0.1            </td><td> HL100133 </td><td> Interface Created </td></tr>
*
* </table><br><br>
* <hr>
*
*******************************************************************************/
void getDeviceSignatureFromMemory(void)
{
	gu32I2CMemoryOperationTimeout = FIVE_SEC;
	gu8SignatureReadFlag = FALSE;
	if(HAL_I2C_Mem_Read_IT(&hi2c1, EEPROM_ADDRESS,0,I2C_MEMADD_SIZE_16BIT,
			(uint8_t *)strI2cEeprom.pu8DeviceSignature,1) != HAL_OK)
	{
		assertError(enmTORERRORS_MEM_I2C,enmERRORSTATE_ACTIVE);
	}
	else
		assertError(enmTORERRORS_MEM_I2C,enmERRORSTATE_NOERROR);

	/* Wait for EEPROM Signature Read Cycle to complete or Timeout */
	while((gu8SignatureReadFlag == FALSE ) && (gu32I2CMemoryOperationTimeout > 0));

	/* Reset Timeout id memory read is completed */
	if(gu32I2CMemoryOperationTimeout > 0)
		gu32I2CMemoryOperationTimeout = 0;

	/*Compare Device Signature */
	if(strcmp((char *)strI2cEeprom.pu8DeviceSignature,(const char *)EEPROM_SIGNATURE_BYTE) == 0)
	{
		gu8SignatureReadFlag = TRUE;
	}
	else
	{
		gu8SignatureReadFlag = FALSE;
	}
}

/******************************************************************************
* Function : HAL_I2C_MemRxCpltCallback()
*//**
* \b Description:
*
* This function is Memory Rx Transfer completed callback.(Non-Blocking)
*
*
* PRE-CONDITION: Initilize used I2C peripheral , Enable interrupts
*
* POST-CONDITION: Operates read cycle for eeprom logic
*
*
* @return 		None.
*
* \b Example Example:
* @code
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
*
* </table><br><br>
* <hr>
*
*******************************************************************************/
void HAL_I2C_MemRxCpltCallback (I2C_HandleTypeDef *hi2c)
{
	if((gu8MemoryOperationStatus == 1) && (gu32I2CMemoryOperationTimeout !=0)
			&& (gu8OperateSystemStatus != FALSE))
	{
		if(gau8MemoryOperationRead[u8RdMemLoopCounter] == TRUE)
		{
			gau8MemoryOperationRead[u8RdMemLoopCounter] = FALSE;
			u8RdMemLoopCounter ++;
			if(u8RdMemLoopCounter >= I2CMEM_MAX_OPRATIONS)
			{
				gu32MemoryOperation = EEPROM_OPR_IDLE;
				u8RdMemLoopCounter = 0;
				gu8MemoryRdOperationFlag = TRUE;
				gu8MemoryCycleComplete = TRUE;
				if(gu8RestoreSystemSettings == TRUE)
				{
					initSystemDefaultsfromMemory();
					gu8RestoreSystemSettings = FALSE;
				}
			}
		}
		gu8MemoryOperationStatus = 0;
		gu32I2CMemoryOperationTimeout = 0;
	}
	else
	{
		/* Signature Read complete */
		if(gu32EEPROMInitforSerialFlash == 0)
			gu8SignatureReadFlag = TRUE;
	}
}

///****************************************************************************
// Function HAL_I2C_MemRxCpltCallback
// Purpose: Memory Rx Transfer completed callback.
// Input:	  hi2c Pointer to a I2C_HandleTypeDef structure that contains
//          the configuration information for the specified I2C.
// Return value: None.
//
// Note(s)(if-any) :
// -> Server URL Length Limit is 250 bytes
// -> EEPROM Page Limit/ Boundary is 32 Bytes
// -> Will be used to store Server URL (byte by byte mechanism)
//
// Change History:
// Author           	Date                Remarks
// System Generated   25-05-18
//******************************************************************************/
//void HAL_I2C_MasterTxCpltCallback(I2C_HandleTypeDef *hi2c)
//{
//	/*To be used in case of Page boundary write*/
//}
/****************************************************************************
 Function HAL_I2C_ErrorCallback
 Purpose: I2C error callback.
 Input:	  hi2c Pointer to a I2C_HandleTypeDef structure that contains
		  the configuration information for the specified I2C.
 Return value: None.

 Note(s)(if-any) :

 Change History:
 Author           	Date                Remarks
 System Generated   11-04-18
******************************************************************************/

/******************************************************************************
* Function : HAL_I2C_ErrorCallback(I2C_HandleTypeDef *hi2c)
*//**
* \b Description:
*
* This function is I2C error callback.
*
*
* PRE-CONDITION: Initilize used I2C peripheral , Enable interrupts
*
* POST-CONDITION: Logs error
*
*
* @return 		None.
*
* \b Example Example:
* @code
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
*
* </table><br><br>
* <hr>
*
*******************************************************************************/
void HAL_I2C_ErrorCallback(I2C_HandleTypeDef *hi2c)
{
	assertError(enmTORERRORS_MEM_I2C,enmERRORSTATE_ACTIVE);
}


/******************************************************************************
* Function : updateMemoryOperationStatus(void)
*//**
* \b Description:
*
* This function is Timeout Handler for memory operations.
* Will break out from memory operation if timer is elapsed.
 -> Memory Operation Time is set to 5 Sec
 -> Not Used . Only for testing
*
*
* PRE-CONDITION: Initilize used I2C peripheral , Enable interrupts
*
* POST-CONDITION:
*
*
* @return 		None.
*
* \b Example Example:
* @code
* 	updateMemoryOperationStatus();
* @endcode
*
* @see
*
* <br><b> - HISTORY OF CHANGES - </b>
*
* <table align="left" style="width:800px">
* <tr><td> Date       </td><td> Software Version </td><td> Initials </td><td> Description </td></tr>
* <tr><td> 01/06/2019 </td><td> 0.0.1            </td><td> HL100133 </td><td> Interface Created </td></tr>
*
* </table><br><br>
* <hr>
*
*******************************************************************************/
void updateMemoryOperationStatus(void)
{
	if(gu32I2CMemoryOperationTimeout == 0)
	{
		gu32MemoryOperation = EEPROM_OPR_IDLE;
	}
}


/******************************************************************************
* Function : initDefaultParameters(void)
*//**
* \b Description:
*
* This function is used to Initialise System Operation Parameters
*
* PRE-CONDITION:
*
* POST-CONDITION:
*
*
* @return 		None.
*
* \b Example Example:
* @code
* 	initDefaultParameters();
* @endcode
*
* @see
*
* <br><b> - HISTORY OF CHANGES - </b>
*
* <table align="left" style="width:800px">
* <tr><td> Date       </td><td> Software Version </td><td> Initials </td><td> Description </td></tr>
* <tr><td> 01/06/2019 </td><td> 0.0.1            </td><td> HL100133 </td><td> Interface Created </td></tr>
*
* </table><br><br>
* <hr>
*
*******************************************************************************/
void initDefaultParameters(void)
{
	memset(gau8GSM4G_apn,0x00,sizeof(char) * 100);
	memcpy(gau8GSM4G_apn,strI2cEeprom.pu8NetworkAPN, strlen(strI2cEeprom.pu8NetworkAPN));
	memset(gau8GSM_url,0x00,sizeof(char) * 100);
	memcpy( gau8GSM_url, strI2cEeprom.pu8RdServerURL, strlen(strI2cEeprom.pu8RdServerURL));
	memset(gau8GSM_smsto,0x00,sizeof(char) * 15);
	memcpy(gau8GSM_smsto, strI2cEeprom.pu8MobileNumber,strlen(strI2cEeprom.pu8MobileNumber));
	gsmInstance.u32OFFPayloadUploadFreq = atoi(strI2cEeprom.pu8RdUploadOffFreq);
	gsmInstance.u32ONPayloadUploadFreq = atoi(strI2cEeprom.pu8RdUploadOnFreq);
	initGSMSIM868();
}

/******************************************************************************
* Function : initMemoryRead(void)
*//**
* \b Description:
*
* This function is used to Initialise Memory Read Array
*
* PRE-CONDITION:
*
* POST-CONDITION:
*
*
* @return 		None.
*
* \b Example Example:
* @code
* 	initMemoryRead();
* @endcode
*
* @see
*
* <br><b> - HISTORY OF CHANGES - </b>
*
* <table align="left" style="width:800px">
* <tr><td> Date       </td><td> Software Version </td><td> Initials </td><td> Description </td></tr>
* <tr><td> 01/06/2019 </td><td> 0.0.1            </td><td> HL100133 </td><td> Interface Created </td></tr>
*
* </table><br><br>
* <hr>
*
*******************************************************************************/
void initMemoryRead(void)
{
	gau8MemoryOperationRead[0] = FALSE;
	gau8MemoryOperationRead[1] = TRUE;
	gau8MemoryOperationRead[2] = TRUE;
	gau8MemoryOperationRead[3] = TRUE;
	gau8MemoryOperationRead[4] = TRUE;
	gau8MemoryOperationRead[5] = TRUE;
	gau8MemoryOperationRead[6] = TRUE;
	gau8MemoryOperationRead[7] = TRUE;
	gau8MemoryOperationRead[8] = TRUE;
	gau8MemoryOperationRead[9] = TRUE;
	gau8MemoryOperationRead[10] = TRUE;
	gau8MemoryOperationRead[11] = TRUE;
	gau8MemoryOperationRead[12] = TRUE;
	gau8MemoryOperationRead[13] = TRUE;
	gau8MemoryOperationRead[14] = TRUE;
	gau8MemoryOperationRead[15] = TRUE;
	gau8MemoryOperationRead[16] = TRUE;
	gau8MemoryOperationRead[17] = TRUE;
	gu32MemoryOperation = EEPROM_OPR_READ;
}

/******************************************************************************
* Function : initMemoryWrite(void)
*//**
* \b Description:
*
* This function is used to Initialise Memory Write Array
*
* PRE-CONDITION:
*
* POST-CONDITION:
*
*
* @return 		None.
*
* \b Example Example:
* @code
* 	initMemoryWrite();
* @endcode
*
* @see
*
* <br><b> - HISTORY OF CHANGES - </b>
*
* <table align="left" style="width:800px">
* <tr><td> Date       </td><td> Software Version </td><td> Initials </td><td> Description </td></tr>
* <tr><td> 01/06/2019 </td><td> 0.0.1            </td><td> HL100133 </td><td> Interface Created </td></tr>
*
* </table><br><br>
* <hr>
*
*******************************************************************************/
void initMemoryWrite(void)
{
	gau8MemoryOperationWrite[0] = FALSE;
	gau8MemoryOperationWrite[1] = TRUE;
	gau8MemoryOperationWrite[2] = TRUE;
	gau8MemoryOperationWrite[3] = TRUE;
	gau8MemoryOperationWrite[4] = TRUE;
	gau8MemoryOperationWrite[5] = TRUE;
	gau8MemoryOperationWrite[6] = TRUE;
	gau8MemoryOperationWrite[7] = TRUE;
	gau8MemoryOperationWrite[8] = TRUE;
	gau8MemoryOperationWrite[9] = TRUE;
	gau8MemoryOperationWrite[10] = TRUE;
	gau8MemoryOperationWrite[11] = TRUE;
	gu32MemoryOperation = EEPROM_OPR_WRITE;
}


/******************************************************************************
* Function : testEEPROM(void)
*//**
* \b Description:
*
* This function is used for Basic eeprom Wr/Rd testing Only . Not for Production Usage
*
* PRE-CONDITION:
*
* POST-CONDITION:
*
* @return 		None.
*
* \b Example Example:
* @code
* 	testEEPROM();
* @endcode
*
* @see
*
* <br><b> - HISTORY OF CHANGES - </b>
*
* <table align="left" style="width:800px">
* <tr><td> Date       </td><td> Software Version </td><td> Initials </td><td> Description </td></tr>
* <tr><td> 01/06/2019 </td><td> 0.0.1            </td><td> HL100133 </td><td> Interface Created </td></tr>
*
* </table><br><br>
* <hr>
*
*******************************************************************************/
void testEEPROM(void)
{
	gu8MemoryOperationStatus = 0;
	//uint32_t size = strlen(gau8GSM_apn);

	if(HAL_I2C_Mem_Write_IT(&hi2c1,EEPROM_ADDRESS,0x01,I2C_MEMADD_SIZE_16BIT,
			(uint8_t *)&gau8GSM_url,strlen(gau8GSM_url))== HAL_OK)
	{
		//testVar ++;
		gu8MemoryOperationStatus = 1;
	}
	else
	{
		//Error_Callback(enmERROR_MEM_I2C);
	}

	while (HAL_I2C_GetState(&hi2c1) != HAL_I2C_STATE_READY)
	for ( ;; )
	{
		gu32MemStatus = HAL_I2C_IsDeviceReady ( & hi2c1 ,EEPROM_ADDRESS , 1000 ,HAL_MAX_DELAY) ;
		if (gu32MemStatus == HAL_OK)
				break;
	}

	gu32MemStatus = HAL_I2C_Mem_Read_IT(&hi2c1,EEPROM_DEVICE_ID_READ,0x01,I2C_MEMADD_SIZE_16BIT,(uint8_t *)&strI2cEeprom.pu8RdServerURL,strlen(gau8GSM_url));
	//gu32MemStatus = HAL_I2C_Mem_Read_IT(&hi2c1, EEPROM_DEVICE_ID_READ,0x30,I2C_MEMADD_SIZE_16BIT,(uint8_t *)&strI2cEeprom.test,2);
	if(gu32MemStatus == HAL_OK)
	{
		//testVar --;
	}
	else
	{
		//Error_Callback(enmERROR_MEM_I2C);
	}
}

/******************************************************************************
* Function : enableMemoryOperation(uint32_t optType ,enmMemoryOperation oprNo)
*//**
* \b Description:
*
* This function is used for eeprom Wr/Rd
    optType :->
	EEPROM_OPR_WRITE
	EEPROM_OPR_READ
*
* PRE-CONDITION:
*
* POST-CONDITION:
*
* @return 		None.
*
* \b Example Example:
* @code
* 	enableMemoryOperation(EEPROM_OPR_WRITE ,1);
* @endcode
*
* @see
*
* <br><b> - HISTORY OF CHANGES - </b>
*
* <table align="left" style="width:800px">
* <tr><td> Date       </td><td> Software Version </td><td> Initials </td><td> Description </td></tr>
* <tr><td> 01/06/2019 </td><td> 0.0.1            </td><td> HL100133 </td><td> Interface Created </td></tr>
*
* </table><br><br>
* <hr>
*
*******************************************************************************/
void enableMemoryOperation(uint32_t optType ,enmMemoryOperation oprNo)
{
	if(optType == EEPROM_OPR_WRITE)
		gau8MemoryOperationWrite[oprNo] = TRUE;
	else if(optType == EEPROM_OPR_READ)
		gau8MemoryOperationRead[oprNo] = TRUE;
	else{}
}

/******************************************************************************
* Function : writeDefaultParamtoMemory()
*//**
* \b Description:
*
* This function is used to Write Default Parameters to Memory
*
* PRE-CONDITION:
*
* POST-CONDITION:
*
* @return 		None.
*
* \b Example Example:
* @code
* 	writeDefaultParamtoMemory();
* @endcode
*
* @see
*
* <br><b> - HISTORY OF CHANGES - </b>
*
* <table align="left" style="width:800px">
* <tr><td> Date       </td><td> Software Version </td><td> Initials </td><td> Description </td></tr>
* <tr><td> 01/06/2019 </td><td> 0.0.1            </td><td> HL100133 </td><td> Interface Created </td></tr>
*
* </table><br><br>
* <hr>
*
*******************************************************************************/
void writeDefaultParamtoMemory(void)
{
	initMemoryWrite();
	strI2cEeprom.u32WrMobileNumberLength = strlen(gau8GSM_smsto);
	strI2cEeprom.u32WrNetworkAPNLength = strlen(gau8GSM4G_apn);
	strI2cEeprom.u32WrServerURLLength = strlen(gu8NewURL);
	strI2cEeprom.u32WrOffFrequencyLength = 4;
	strI2cEeprom.u32WrOnFrequencyLength = 4;
 	memcpy(strI2cEeprom.pu8WrMobileNumber,gau8GSM_smsto, strlen(gau8GSM_smsto));
 	memcpy(strI2cEeprom.pu8WrUploadOffFreq,"30", 2);
 	memcpy(strI2cEeprom.pu8Wr8UploadOnFreq,"90",2);
	/* 27-12-2019 */
	gu32MemoryOperation = EEPROM_OPR_WRITE;
}

/******************************************************************************
* Function : initSignatureWrite()
*//**
* \b Description:
*
* This function is used to Initialize Signature Byte Write
*
* PRE-CONDITION:
*
* POST-CONDITION:
*
* @return 		None.
*
* \b Example Example:
* @code
* 	initSignatureWrite();
* @endcode
*
* @see
*
* <br><b> - HISTORY OF CHANGES - </b>
*
* <table align="left" style="width:800px">
* <tr><td> Date       </td><td> Software Version </td><td> Initials </td><td> Description </td></tr>
* <tr><td> 01/06/2019 </td><td> 0.0.1            </td><td> HL100133 </td><td> Interface Created </td></tr>
*
* </table><br><br>
* <hr>
*
*******************************************************************************/
void initSignatureWrite(void)
{
	gau8MemoryOperationWrite[0] = TRUE;
	gu32MemoryOperation = EEPROM_OPR_WRITE;
}


/******************************************************************************
* Function : writeParametertoMemory(enmMemoryOperation memOpr)
*//**
* \b Description:
*
* This function is used to write param data to memory
*
* PRE-CONDITION:
*
* POST-CONDITION:
*
* @return 		None.
*
* \b Example Example:
* @code
* 	writeParametertoMemory(I2C_MEM_DEVSIGNATURE);
* @endcode
*
* @see
*
* <br><b> - HISTORY OF CHANGES - </b>
*
* <table align="left" style="width:800px">
* <tr><td> Date       </td><td> Software Version </td><td> Initials </td><td> Description </td></tr>
* <tr><td> 01/06/2019 </td><td> 0.0.1            </td><td> HL100133 </td><td> Interface Created </td></tr>
*
* </table><br><br>
* <hr>
*
*******************************************************************************/
void writeParametertoMemory(enmMemoryOperation memOpr)
{
	switch(memOpr)
	{
		case I2C_MEM_DEVSIGNATURE:
			gau8MemoryOperationWrite[I2C_MEM_DEVSIGNATURE] = TRUE;
			break;
		case I2C_MEM_MOBILELENGTH:
			gau8MemoryOperationWrite[I2C_MEM_MOBILELENGTH] = TRUE;
			break;
		case I2C_MEM_APNSTRLENGTH:
			gau8MemoryOperationWrite[I2C_MEM_APNSTRLENGTH] = TRUE;
			break;
		case I2C_MEM_URLSTRLENGTH:
			gau8MemoryOperationWrite[I2C_MEM_URLSTRLENGTH] = TRUE;
			break;
		case I2C_MEM_ONFREQLENGTH:
			gau8MemoryOperationWrite[I2C_MEM_ONFREQLENGTH] = TRUE;
			break;
		case I2C_MEM_OFFFREQLENGTH:
			gau8MemoryOperationWrite[I2C_MEM_OFFFREQLENGTH] = TRUE;
			break;
		case I2C_MEM_MOBILENOSTR:
			gau8MemoryOperationWrite[I2C_MEM_MOBILENOSTR] = TRUE;
			break;
		case I2C_MEM_APNSTR:
			gau8MemoryOperationWrite[I2C_MEM_APNSTR] = TRUE;
			break;
		case I2C_MEM_URLSTR:
			gau8MemoryOperationWrite[I2C_MEM_URLSTR] = TRUE;
			break;
		case I2C_MEM_ONFREQ:
			gau8MemoryOperationWrite[I2C_MEM_ONFREQ] = TRUE;
			break;
		case I2C_MEM_OFFFREQ:
			gau8MemoryOperationWrite[I2C_MEM_OFFFREQ] = TRUE;
			break;

		default:
			/* Invalid Request */
			break;
	}
	gu32MemoryOperation = EEPROM_OPR_WRITE;
}

/******************************************************************************
* Function : initSystemDefaultsfromMemory()
*//**
* \b Description:
*
* This function is used to Initialize System Parameters with Stored Params
*
* PRE-CONDITION:
*
* POST-CONDITION:
*
* @return 		None.
*
* \b Example Example:
* @code
* 	initSystemDefaultsfromMemory();
* @endcode
*
* @see
*
* <br><b> - HISTORY OF CHANGES - </b>
*
* <table align="left" style="width:800px">
* <tr><td> Date       </td><td> Software Version </td><td> Initials </td><td> Description </td></tr>
* <tr><td> 01/06/2019 </td><td> 0.0.1            </td><td> HL100133 </td><td> Interface Created </td></tr>
*
* </table><br><br>
* <hr>
*
*******************************************************************************/
void initSystemDefaultsfromMemory(void)
{
	/* */
	if(strlen(strI2cEeprom.pu8NetworkAPN) != 0)
	{
		memset(gau8GSM4G_apn,0x00,sizeof(char) * 100);
		memcpy(gau8GSM4G_apn,strI2cEeprom.pu8NetworkAPN, strlen(strI2cEeprom.pu8NetworkAPN));
	}
	if(strlen(strI2cEeprom.pu8RdServerURL) != 0)
	{
		memset(gau8GSM_url,0x00,sizeof(char) * 100);
		memcpy( gau8GSM_url, strI2cEeprom.pu8RdServerURL, strlen(strI2cEeprom.pu8RdServerURL));
	}
	if(strlen(strI2cEeprom.pu8MobileNumber) != 0)
	{
		memset(gau8GSM_smsto,0x00,sizeof(char) * 15);
		memcpy(gau8GSM_smsto, strI2cEeprom.pu8MobileNumber,strlen(strI2cEeprom.pu8MobileNumber));
	}
	gsmInstance.u32OFFPayloadUploadFreq = atoi(strI2cEeprom.pu8RdUploadOffFreq);
	gsmInstance.u32ONPayloadUploadFreq = atoi(strI2cEeprom.pu8RdUploadOnFreq);
	initGSMSIM868();
}


/******************************************************************************
* Function : getAvailableSpaceInMemoryPage(uint32_t MemAddress)
*//**
* \b Description:
*
* This function is used to  Calculate Available Space in Memory Page
*
* PRE-CONDITION:
*
* POST-CONDITION:
*
* @return 		uint32_t spaceSize.
*
* \b Example Example:
* @code
* 	uint32_t spaceSize = getAvailableSpaceInMemoryPage(30);
* @endcode
*
* @see
*
* <br><b> - HISTORY OF CHANGES - </b>
*
* <table align="left" style="width:800px">
* <tr><td> Date       </td><td> Software Version </td><td> Initials </td><td> Description </td></tr>
* <tr><td> 01/06/2019 </td><td> 0.0.1            </td><td> HL100133 </td><td> Interface Created </td></tr>
*
* </table><br><br>
* <hr>
*
*******************************************************************************/
uint32_t getAvailableSpaceInMemoryPage(uint32_t MemAddress)
{
	return (((MemAddress/AT24XX_PAGESIZE)+1) * AT24XX_PAGESIZE) - MemAddress ;
}

/******************************************************************************
* Function : getFirstPageWriteSize(uint32_t pageSpace)
*//**
* \b Description:
*
* This function is used to  Calculate First Page Write Size.
* pageSpace - As calculated with getAvailableSpaceInMemoryPage(uint32_t MemAddress)
*
* PRE-CONDITION:
*
* POST-CONDITION:
*
* @return 		uint32_t pageSize.
*
* \b Example Example:
* @code
* 	uint32_t pageSize = getFirstPageWriteSize(30);
* @endcode
*
* @see
*
* <br><b> - HISTORY OF CHANGES - </b>
*
* <table align="left" style="width:800px">
* <tr><td> Date       </td><td> Software Version </td><td> Initials </td><td> Description </td></tr>
* <tr><td> 01/06/2019 </td><td> 0.0.1            </td><td> HL100133 </td><td> Interface Created </td></tr>
*
* </table><br><br>
* <hr>
*
*******************************************************************************/
uint32_t getFirstPageWriteSize(uint32_t pageSpace)
{
	uint32_t u32firstPageWriteSize = 0;
	if(pageSpace >AT24XX_PAGESIZE)
	{
		u32firstPageWriteSize = pageSpace - ((pageSpace/AT24XX_PAGESIZE) * AT24XX_PAGESIZE);
	}
	else if(u32firstPageWriteSize == 0)
	{
		u32firstPageWriteSize = AT24XX_PAGESIZE;
	}
	else
		u32firstPageWriteSize = AT24XX_PAGESIZE;

	return u32firstPageWriteSize;
}

/******************************************************************************
* Function : getLastPageWriteSize(uint32_t firstPageWriteSize , uint32_t dataLength)
*//**
* \b Description:
*
* This function is used to  Calculate Last Page Write Size.
*
*
* PRE-CONDITION:
*
* POST-CONDITION:
*
* @return 		uint32_t pageSize.
*
* \b Example Example:
* @code
* 	uint32_t pageSize = getLastPageWriteSize(30,20);
* @endcode
*
* @see
*
* <br><b> - HISTORY OF CHANGES - </b>
*
* <table align="left" style="width:800px">
* <tr><td> Date       </td><td> Software Version </td><td> Initials </td><td> Description </td></tr>
* <tr><td> 01/06/2019 </td><td> 0.0.1            </td><td> HL100133 </td><td> Interface Created </td></tr>
*
* </table><br><br>
* <hr>
*
*******************************************************************************/
uint32_t getLastPageWriteSize(uint32_t firstPageWriteSize , uint32_t dataLength)
{
	uint32_t u32LastPageWriteSize = 0;

	if(dataLength > firstPageWriteSize)
		u32LastPageWriteSize = (dataLength - firstPageWriteSize) % AT24XX_PAGESIZE;

	return u32LastPageWriteSize;
}

/******************************************************************************
* Function : getPageWriteCyclesRequired(uint32_t firstPageWriteSize , uint32_t dataLength)
*//**
* \b Description:
*
* This function is used to  Calculate Page Write Cycles required.
*
*
* PRE-CONDITION:
*
* POST-CONDITION:
*
* @return 		uint32_t pageCycles.
*
* \b Example Example:
* @code
* 	uint32_t pageCycles = getPageWriteCyclesRequired(30 , 20);
* @endcode
*
* @see
*
* <br><b> - HISTORY OF CHANGES - </b>
*
* <table align="left" style="width:800px">
* <tr><td> Date       </td><td> Software Version </td><td> Initials </td><td> Description </td></tr>
* <tr><td> 01/06/2019 </td><td> 0.0.1            </td><td> HL100133 </td><td> Interface Created </td></tr>
*
* </table><br><br>
* <hr>
*
*******************************************************************************/
uint32_t getPageWriteCyclesRequired(uint32_t firstPageWriteSize , uint32_t dataLength)
{
	uint32_t u32PageWriteCycles = 0;

	if(dataLength > firstPageWriteSize)
		u32PageWriteCycles = ((dataLength - firstPageWriteSize) / AT24XX_PAGESIZE) + 2 ;
	else
		u32PageWriteCycles = 1;

	return u32PageWriteCycles;
}


void systemReset(void)
{
	while(1);
}
//******************************* End of File *******************************************************************
