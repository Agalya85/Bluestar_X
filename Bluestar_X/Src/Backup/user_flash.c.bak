/*******************************************************************************
* Title                 :   Internal Flash Interface
* Filename              :   user_flash.c
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

/** @file  user_flash.c
 *  @brief Internal Flash Implementation
 *
 */
/******************************************************************************
* Includes
*******************************************************************************/
#include "main.h"
#include "user_flash.h"
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
uint32_t u32TempDataFromMemory = 0;
uint32_t gu3ConfigLength = 0;
uint32_t u32DatatoWrite = 0;
uint32_t u32LastMemoryWriteLocation = 0;
uint32_t u32MemoryDestination = 0; 			/* Flash Write Start Location for Flash Write */

/******************************************************************************
* Function Prototypes
*******************************************************************************/
static uint32_t GetSector(uint32_t Address);
/******************************************************************************
* Function Definitions
*******************************************************************************/
/**
  * @brief  Unlocks Flash for write access
  * @param  None
  * @retval None
  */
void FLASH_If_Init(void)
{
  HAL_FLASH_Unlock();

  /* Clear pending flags (if any) */
  __HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_EOP | FLASH_FLAG_OPERR | FLASH_FLAG_WRPERR |
                         FLASH_FLAG_PGAERR | FLASH_FLAG_PGPERR | FLASH_FLAG_PGSERR);
}

/**
  * @brief  This function does an erase of all user flash area
  * @param  StartSector: start of user flash area
  * @retval 0: user flash area successfully erased
  *         1: error occurred
  */
uint32_t FLASH_If_Erase(uint32_t StartSector)
{
  uint32_t UserStartSector;
  uint32_t SectorError;
  FLASH_EraseInitTypeDef pEraseInit;

  /* Unlock the Flash to enable the flash control register access *************/
  FLASH_If_Init();

  /* Get the sector where start the user flash area */
  UserStartSector = GetSector(StartSector);

  pEraseInit.TypeErase = TYPEERASE_SECTORS;
  pEraseInit.Sector = UserStartSector;
  pEraseInit.NbSectors = 1;
  pEraseInit.VoltageRange = VOLTAGE_RANGE_3;

  if (HAL_FLASHEx_Erase(&pEraseInit, &SectorError) != HAL_OK)
  {
     /* Error occurred while page erase */
     return (1);
  }

  return (0);
}

/**
  * @brief  This function writes a data buffer in flash (data are 32-bit aligned).
  * @note   After writing data buffer, the flash content is checked.
  * @param  FlashAddress: start address for writing data buffer
  * @param  Data: pointer on data buffer
  * @param  DataLength: length of data buffer (unit is 32-bit word)
  * @retval 0: Data successfully written to Flash memory
  *         1: Error occurred while writing data in Flash memory
  *         2: Written Data in flash memory is different from expected one
  */
uint32_t FLASH_If_Write(uint32_t FlashAddress, uint32_t* Data ,uint32_t DataLength)
{
  uint32_t i = 0;
  /* actual data is uint8_t so we devide datalength / 4*/
  for (i = 0; (i < (DataLength / 4)) && (FlashAddress <= (USER_FLASH_END_ADDRESS - 4)); i++)//USER_FLASH_END_ADDRESS
  {
    /* Device voltage range supposed to be [2.7V to 3.6V], the operation will
       be done by word */
    if (HAL_FLASH_Program(TYPEPROGRAM_WORD, FlashAddress, *(uint32_t*)(Data+i)) == HAL_OK)
    {
     /* Check the written value */
      if (*(uint32_t*)FlashAddress != *(uint32_t*)(Data+i))
      {
        /* Flash content doesn't match SRAM content */
        return(FLASHIF_WRITINGCTRL_ERROR);
      }
      /* Increment FLASH destination address */
      FlashAddress += 4;
      u32LastMemoryWriteLocation = FlashAddress;
    }
    else
    {
      /* Error occurred while writing data in Flash memory */
      return (FLASHIF_WRITING_ERROR);
    }
  }
  u32LastMemoryWriteLocation = FlashAddress;
  return (FLASHIF_OK);
}

/**
  * @brief  Returns the write protection status of user flash area.
  * @param  None
  * @retval 0: No write protected sectors inside the user flash area
  *         1: Some sectors inside the user flash area are write protected
  */
uint16_t FLASH_If_GetWriteProtectionStatus(void)
{
  uint32_t ProtectedSECTOR = 0xFFF;
  FLASH_OBProgramInitTypeDef OptionsBytesStruct;

  /* Unlock the Flash to enable the flash control register access *************/
  HAL_FLASH_Unlock();

  /* Check if there are write protected sectors inside the user flash area ****/
  HAL_FLASHEx_OBGetConfig(&OptionsBytesStruct);

  /* Lock the Flash to disable the flash control register access (recommended
     to protect the FLASH memory against possible unwanted operation) *********/
  HAL_FLASH_Lock();

  /* Get pages already write protected ****************************************/
  ProtectedSECTOR = ~(OptionsBytesStruct.WRPSector) & FLASH_SECTOR_TO_BE_PROTECTED;

  /* Check if desired pages are already write protected ***********************/
  if(ProtectedSECTOR != 0)
  {
    /* Some sectors inside the user flash area are write protected */
    return FLASHIF_PROTECTION_WRPENABLED;
  }
  else
  {
    /* No write protected sectors inside the user flash area */
    return FLASHIF_PROTECTION_NONE;
  }
}

/**
  * @brief  Gets the sector of a given address
  * @param  Address: Flash address
  * @retval The sector of a given address
  */
static uint32_t GetSector(uint32_t Address)
{
  uint32_t sector = 0;

  if((Address < ADDR_FLASH_SECTOR_1) && (Address >= ADDR_FLASH_SECTOR_0))
  {
    sector = FLASH_SECTOR_0;
  }
  else if((Address < ADDR_FLASH_SECTOR_2) && (Address >= ADDR_FLASH_SECTOR_1))
  {
    sector = FLASH_SECTOR_1;
  }
  else if((Address < ADDR_FLASH_SECTOR_3) && (Address >= ADDR_FLASH_SECTOR_2))
  {
    sector = FLASH_SECTOR_2;
  }
  else if((Address < ADDR_FLASH_SECTOR_4) && (Address >= ADDR_FLASH_SECTOR_3))
  {
    sector = FLASH_SECTOR_3;
  }
  else if((Address < ADDR_FLASH_SECTOR_5) && (Address >= ADDR_FLASH_SECTOR_4))
  {
    sector = FLASH_SECTOR_4;
  }
  else if((Address < ADDR_FLASH_SECTOR_6) && (Address >= ADDR_FLASH_SECTOR_5))
  {
    sector = FLASH_SECTOR_5;
  }
  else if((Address < ADDR_FLASH_SECTOR_7) && (Address >= ADDR_FLASH_SECTOR_6))
  {
    sector = FLASH_SECTOR_6;
  }
  else if((Address < ADDR_FLASH_SECTOR_8) && (Address >= ADDR_FLASH_SECTOR_7))
  {
    sector = FLASH_SECTOR_7;
  }
  else if((Address < ADDR_FLASH_SECTOR_9) && (Address >= ADDR_FLASH_SECTOR_8))
  {
    sector = FLASH_SECTOR_8;
  }
  else if((Address < ADDR_FLASH_SECTOR_10) && (Address >= ADDR_FLASH_SECTOR_9))
  {
    sector = FLASH_SECTOR_9;
  }
  else if((Address < ADDR_FLASH_SECTOR_11) && (Address >= ADDR_FLASH_SECTOR_10))
  {
    sector = FLASH_SECTOR_10;
  }
  else if((Address < ADDR_FLASH_SECTOR_12) && (Address >= ADDR_FLASH_SECTOR_11))
  {
    sector = FLASH_SECTOR_11;
  }
  else if((Address < ADDR_FLASH_SECTOR_13) && (Address >= ADDR_FLASH_SECTOR_12))
  {
    sector = FLASH_SECTOR_12;
  }
  else if((Address < ADDR_FLASH_SECTOR_14) && (Address >= ADDR_FLASH_SECTOR_13))
  {
    sector = FLASH_SECTOR_13;
  }
  else if((Address < ADDR_FLASH_SECTOR_15) && (Address >= ADDR_FLASH_SECTOR_14))
  {
    sector = FLASH_SECTOR_14;
  }
  else if((Address < ADDR_FLASH_SECTOR_16) && (Address >= ADDR_FLASH_SECTOR_15))
  {
    sector = FLASH_SECTOR_15;
  }
  else if((Address < ADDR_FLASH_SECTOR_17) && (Address >= ADDR_FLASH_SECTOR_16))
  {
    sector = FLASH_SECTOR_16;
  }
  else if((Address < ADDR_FLASH_SECTOR_18) && (Address >= ADDR_FLASH_SECTOR_17))
  {
    sector = FLASH_SECTOR_17;
  }
  else if((Address < ADDR_FLASH_SECTOR_19) && (Address >= ADDR_FLASH_SECTOR_18))
  {
    sector = FLASH_SECTOR_18;
  }
  else if((Address < ADDR_FLASH_SECTOR_20) && (Address >= ADDR_FLASH_SECTOR_19))
  {
    sector = FLASH_SECTOR_19;
  }
  else if((Address < ADDR_FLASH_SECTOR_21) && (Address >= ADDR_FLASH_SECTOR_20))
  {
    sector = FLASH_SECTOR_20;
  }
  else if((Address < ADDR_FLASH_SECTOR_22) && (Address >= ADDR_FLASH_SECTOR_21))
  {
    sector = FLASH_SECTOR_21;
  }
  else if((Address < ADDR_FLASH_SECTOR_23) && (Address >= ADDR_FLASH_SECTOR_22))
  {
    sector = FLASH_SECTOR_22;
  }
  else/*(Address < FLASH_END_ADDR) && (Address >= ADDR_FLASH_SECTOR_23))*/
  {
    sector = FLASH_SECTOR_23;
  }
  return sector;
}

/**
  * @brief  Configure the write protection status of user flash area.
  * @param  modifier DISABLE or ENABLE the protection
  * @retval HAL_StatusTypeDef HAL_OK if change is applied.
  */
HAL_StatusTypeDef FLASH_If_WriteProtectionConfig(uint32_t modifier)
{
  uint32_t ProtectedSECTOR = 0xFFF;
  FLASH_OBProgramInitTypeDef config_new, config_old;
  HAL_StatusTypeDef result = HAL_OK;

  /* Get pages write protection status ****************************************/
  HAL_FLASHEx_OBGetConfig(&config_old);

  /* The parameter says whether we turn the protection on or off */
  config_new.WRPState = modifier;

  /* We want to modify only the Write protection */
  config_new.OptionType = OPTIONBYTE_WRP;

  /* No read protection, keep BOR and reset settings */
  config_new.RDPLevel = OB_RDP_LEVEL_0;
  config_new.USERConfig = config_old.USERConfig;
  /* Get pages already write protected ****************************************/
  ProtectedSECTOR = config_old.WRPSector | FLASH_SECTOR_TO_BE_PROTECTED;

  /* Unlock the Flash to enable the flash control register access *************/
  HAL_FLASH_Unlock();

  /* Unlock the Options Bytes *************************************************/
  HAL_FLASH_OB_Unlock();

  config_new.WRPSector    = ProtectedSECTOR;
  result = HAL_FLASHEx_OBProgram(&config_new);

  return result;
}

/******************************************************************************
* Function : ConfigFLASHIfWrite(uint8_t *p_source, uint32_t length,uint32_t memorySector ,uint32_t writeCycle)
*//**
* \b Description:
*
* This function is used to write date to internal flash.*
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
*	//
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
uint32_t ConfigFLASHIfWrite(uint8_t *p_source, uint32_t length,uint32_t memorySector ,uint32_t writeCycle)
{
  uint32_t LoopCounter = 0;
  uint32_t LoopIterator = 0;

  /* Unlock the Flash to enable the flash control register access *************/
  	  HAL_FLASH_Unlock();

	  if(writeCycle == 1)
		  u32MemoryDestination = ADDR_FLASH_SECTOR_22; // Second Last Sector
	  else
		  u32MemoryDestination = u32LastMemoryWriteLocation;

	  for (LoopCounter = 0; (LoopCounter < length) && (u32MemoryDestination <= (ADDR_FLASH_SECTOR_23 - 4));)
	  {
			if(LoopIterator <= (length/4))
			{
				if(WritetoFlashMemory(p_source[LoopCounter],p_source[LoopCounter + 1],p_source[LoopCounter + 2],p_source[LoopCounter + 3],u32MemoryDestination) == SUCCESS)
				{
					/* Increment FLASH destination address */
					u32MemoryDestination += 4;
					LoopCounter += 4;
					LoopIterator++;
				}
				else
				{
					return(FLASHIF_WRITINGCTRL_ERROR);
				}
			}
			else
				break;
	  	}

  /* Lock the Flash to disable the flash control register access (recommended
     to protect the FLASH memory against possible unwanted operation) *********/
  HAL_FLASH_Lock();
  u32LastMemoryWriteLocation = u32MemoryDestination;

  return (FLASHIF_OK);
}


/******************************************************************************
* Function : uint32_t WriteDatatoFlash(uint32_t Sector,uint8_t *data,uint32_t configLen,uint32_t writeCycle)
*//**
* \b Description:
*
* This function is used to write date to internal flash.*
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
*	if(WriteDatatoFlash(u32LastMemoryWriteLocation,(uint8_t *)tempconfig,(nwmlen+mb485mlen+mbtcpmlen+4),1) == SUCCESS)
*	{
*		// Reset the system
*	}
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
/****************************************************************************
 Function WriteDatatoFlash
 Purpose: Write Data to Internal Flash
 Input:	uint8_t *data,uint32_t writeCycle
 Return value: uint32_t status SUCCESS/ERROR

 Note(s)(if-any) :

 	 // Continuation data ...start after last known config
 u32FlashMemoryWriteStatus = WriteDatatoFlash((uint8_t *)gau8FotaData,0);
 	 //Fresh Data ..Start from Sector Start
 u32FlashMemoryWriteStatus = WriteDatatoFlash((uint8_t *)gau8FotaData,1);


 Change History:
 Author           	Date                Remarks
 KloudQ Team      26-07-2021			Initial Definition
******************************************************************************/
uint32_t WriteDatatoFlash(uint32_t Sector,uint8_t *data,uint32_t configLen,uint32_t writeCycle)
{
	if(FLASH_If_Write(Sector,(uint32_t*)data,configLen) == FLASHIF_OK)
		return SUCCESS;
	else
		return ERROR;
}


/****************************** End of File **************************************************************/
