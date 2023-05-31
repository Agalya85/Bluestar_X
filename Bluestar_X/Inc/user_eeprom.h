/*******************************************************************************
* Title                 :   EEPROM Over I2C interface
* Filename              :   user_eeprom.h
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

/*************** FILE REVISION LOG *****************************************
*
*    Date    Version   Author         	  Description
*  01/06/19   1.0.0    Hrishikesh Limaye   Initial Release.
*
*******************************************************************************/

/** @file  user_eeprom.h
 *  @brief Utilities for EEPROM interfaced over I2C
 */

#ifndef INC_USER_EEPROM_H_
#define INC_USER_EEPROM_H_

/******************************************************************************
* Includes
*******************************************************************************/
/******************************************************************************
* Module Preprocessor Constants
*******************************************************************************/
/**
 * \brief Defines used memory Size in KB \a .
   \param None.
   \returns \a .
 */
#define CONFIG_AT24XX_SIZE 		(32)			/* Configure Used EEPROM memory */
/**
 * \brief Defines device signature byte \a .
   \param None.
   \returns \a .
 */
#define EEPROM_SIGNATURE_BYTE 					"@"  // Make sure gu8DeviceSignature[1] = EEPROM_SIGNATURE_BYTE
/**
 * \brief Defines memory address size in bit  \a .
   \param None.
   \returns \a .
 */
#define EEPROM_MEMORY_ADDRESS_SIZE				(I2C_MEMADD_SIZE_16BIT)
/**
 * \brief Defines device read ID \a .
   \param None.
   \returns \a .
 */
#define EEPROM_DEVICE_ID_READ					(0b10100001)
/**
 * \brief Defines device write ID \a .
   \param None.
   \returns \a .
 */
#define EEPROM_DEVICE_ID_WRITE					(0b10100000)
/**
 * \brief Defines signature address in memory \a .
   \param None.
   \returns \a .
 */
#define EEPROM_SIGNATURE_ADDRESS				(0x00)
/**
 * \brief Defines memory address \a .
   \param None.
   \returns \a .
 */
#define EEPROM_ADDRESS 							(0xA0)
/**
 * \brief Defines eeprom Read Operation \a .
   \param None.
   \returns \a .
 */
#define EEPROM_OPR_READ 						(0x00)
/**
 * \brief Defines eeprom Write Operation \a .
   \param None.
   \returns \a .
 */
#define EEPROM_OPR_WRITE						(0x01)
/**
 * \brief Defines eeprom Idle Operation \a .
   \param None.
   \returns \a .
 */
#define EEPROM_OPR_IDLE							(0x02)
/**
 * \brief Defines max eeprom Operations \a .
   \param None.
   \returns \a .
 */
#define I2CMEM_MAX_OPRATIONS 					(15)	//(11) - Without Configuration Data
/**
 * \brief Defines Mobile Number length start add in eeprom \a .
   \param None.
   \returns \a .
 */
#define MEM_MOBNUMLEN_ADD 						(0x02)
/**
 * \brief Defines URL length start add in eeprom \a .
   \param None.
   \returns \a .
 */
#define MEM_SERURLLEN_ADD 						(0x04)
/**
 * \brief Defines APN length start add in eeprom \a .
   \param None.
   \returns \a .
 */
#define MEM_NWAPNLEN_ADD 						(0x06)
/**
 * \brief Defines On Freq length start add in eeprom \a .
   \param None.
   \returns \a .
 */
#define MEM_ONFRQLEN_ADD 						(0x08)
/**
 * \brief Defines Off freq length start add in eeprom \a .
   \param None.
   \returns \a .
 */
#define MEM_OFFFRQLEN_ADD 						(0x0A)
/**
 * \brief Defines Mobile Number start add in eeprom \a .
   \param None.
   \returns \a .
 */
#define MEM_MOBNUM_ADD 							(0x20)
/**
 * \brief Defines Server URL start add in eeprom \a .
   \param None.
   \returns \a .
 */
#define MEM_SERURL_ADD 							(0x40)
/**
 * \brief Defines APN start add in eeprom \a .
   \param None.
   \returns \a .
 */
#define MEM_NWAPN_ADD 							(0x70)
/**
 * \brief Defines On freq start add in eeprom \a .
   \param None.
   \returns \a .
 */
#define MEM_ONFRQ_ADD 							(0x2C0)
/**
 * \brief Defines off freq start add in eeprom \a .
   \param None.
   \returns \a .
 */
#define MEM_OFFFRQ_ADD 							(0x2C5)
/**
 * \brief Defines URL base add in eeprom \a .
   \param None.
   \returns \a .
 */
#define MEM_URL_BASE_ADD  						(128)

/**
 * \brief Defines Memory Page , page size , address size according to memory used \a .
   \param None.
   \returns \a .
 */
/* Memory Chip Used - Select Accordingly */
/* Get the part configuration based on the size configuration */
#if CONFIG_AT24XX_SIZE == 2       /* AT24C02: 2Kbits = 256; 16 * 16 =  256 */
#  define AT24XX_NPAGES     16
#  define AT24XX_PAGESIZE   16
#  define AT24XX_ADDRSIZE   1
#elif CONFIG_AT24XX_SIZE == 4     /* AT24C04: 4Kbits = 512B; 32 * 16 = 512 */
#  define AT24XX_NPAGES     32
#  define AT24XX_PAGESIZE   16
#  define AT24XX_ADDRSIZE   1
#elif CONFIG_AT24XX_SIZE == 8     /* AT24C08: 8Kbits = 1KiB; 64 * 16 = 1024 */
#  define AT24XX_NPAGES     64
#  define AT24XX_PAGESIZE   16
#  define AT24XX_ADDRSIZE   1
#elif CONFIG_AT24XX_SIZE == 16    /* AT24C16: 16Kbits = 2KiB; 128 * 16 = 2048 */
#  define AT24XX_NPAGES     128
#  define AT24XX_PAGESIZE   16
#  define AT24XX_ADDRSIZE   1
#elif CONFIG_AT24XX_SIZE == 32    /* AT24C32: 32Kbits = 4KiB; 128 * 32 =  4096 * 8*/
#  define AT24XX_NPAGES     128
#  define AT24XX_PAGESIZE   32
#  define AT24XX_ADDRSIZE   2
#elif CONFIG_AT24XX_SIZE == 48    /* AT24C48: 48Kbits = 6KiB; 192 * 32 =  6144 */
#  define AT24XX_NPAGES     192
#  define AT24XX_PAGESIZE   32
#  define AT24XX_ADDRSIZE   2
#elif CONFIG_AT24XX_SIZE == 64    /* AT24C64: 64Kbits = 8KiB; 256 * 32 = 8192 */
#  define AT24XX_NPAGES     256
#  define AT24XX_PAGESIZE   32
#  define AT24XX_ADDRSIZE   2
#elif CONFIG_AT24XX_SIZE == 128   /* AT24C128: 128Kbits = 16KiB; 256 * 64 = 16384 */
#  define AT24XX_NPAGES     256
#  define AT24XX_PAGESIZE   64
#  define AT24XX_ADDRSIZE   2
#elif CONFIG_AT24XX_SIZE == 256   /* AT24C256: 256Kbits = 32KiB; 512 * 64 = 32768 */
#  define AT24XX_NPAGES     512
#  define AT24XX_PAGESIZE   64
#  define AT24XX_ADDRSIZE   2
#elif CONFIG_AT24XX_SIZE == 512   /* AT24C512: 512Kbits = 64KiB; 512 * 128 = 65536 */
#  define AT24XX_NPAGES     512
#  define AT24XX_PAGESIZE   128
#  define AT24XX_ADDRSIZE   2
#endif

/******************************************************************************
* Module Preprocessor Macros
*******************************************************************************/

/******************************************************************************
* Module Typedefs
*******************************************************************************/
/**
 * This Structure stores Memory OperationsS
 */
typedef enum
{
	enmMEMORY_WRITE,
	enmMEMORY_READ
}enmI2CMemoryOperation;

/**
 * This Structure stores Memory Layout
 */
typedef struct
{
	uint32_t u32WrMobileNumberLength;
	uint32_t u32WrNetworkAPNLength;
	uint32_t u32WrServerURLLength;
	uint32_t u32WrOnFrequencyLength;    // in Sec 1 to 10000 Sec
	uint32_t u32WrOffFrequencyLength;   // in Sec 1 to 10000 Sec

	uint32_t u32RdMobileNumberLength;
	uint32_t u32RdNetworkAPNLength;
	uint32_t u32RdServerURLLength;
	uint32_t u32RdOnFrequencyLength;    // in Sec 1 to 10000 Sec
	uint32_t u32RdOffFrequencyLength;   // in Sec 1 to 10000 Sec

	/* Remote Configuration Variables*/
	uint32_t u32RdNetworkConfigLength;
	uint32_t u32RdModbus485ConfigLength;
	uint32_t u32RdModbusTCPConfigLength;

	/* Remote Configuration Variables*/
	char pu8ConfigurationSignature[1];		// Remote Configuration Signature
	char pu8ConfigurationNetwork[1000];	    // Remote Configuration Network
	char pu8ConfigurationMB485[1000];	    // Remote Configuration MB 485
	char pu8ConfigurationMBTCP[1000];	    // Remote Configuration MB TCP

	char pu8Wr8UploadOnFreq[5];	    	// Upload On Freq
	char pu8WrUploadOffFreq[5];	   		// Upload On Freq
	char pu8WrMobileNumber[15];	    	// SMS Send to : 10 or 13 Digit

	char pu8DeviceSignature[1];	    	// Device Signature
	char pu8MobileNumber[15];	    	// SMS Send to : 10 or 13 Digit
	char pu8NetworkAPN[20];		    	// APN : Max 20 characters
	char pu8RdServerURL[150];		    // URL : Max 150 Characters
	char pu8RdUploadOnFreq[5];		    // Upload On Freq
	char pu8RdUploadOffFreq[5];	    	// Upload Off Freq
	uint32_t test;
	uint8_t SerialFlashWrittenTill[4];
	uint8_t SerialFlashReadTill[4];
	/* Flash memory variables */
}strctMemoryLayout;

/**
 * This Enumeration stores Data
 */
typedef enum
{
	I2C_MEM_DEVSIGNATURE = 0,
	I2C_MEM_MOBILELENGTH,
	I2C_MEM_APNSTRLENGTH,
	I2C_MEM_URLSTRLENGTH,
	I2C_MEM_ONFREQLENGTH,
	I2C_MEM_OFFFREQLENGTH,
	I2C_MEM_MOBILENOSTR,
	I2C_MEM_APNSTR,
	I2C_MEM_URLSTR,
	I2C_MEM_ONFREQ,
	I2C_MEM_OFFFREQ,
	/* Added on 19/7/2021 - by 100133 for Config data storage */
	I2C_MEM_CONFIGSIGN,
	I2C_MEM_CONFIGNWLEN,
	I2C_MEM_CONFIGMB485LEN,
	I2C_MEM_CONFIGMBTCPLEN,
	I2C_MEM_CONFIGNW,
	I2C_MEM_CONFIGMB485,
	I2C_MEM_CONFIGMBTCP,

}enmMemoryOperation;

/******************************************************************************
* Module Variable Definitions
*******************************************************************************/
extern strctMemoryLayout strI2cEeprom;
/******************************************************************************
* Function Prototypes
*******************************************************************************/
#ifdef __cplusplus
extern "C"{
#endif

void initMemoryRead(void);
void initDefaultParameters(void);
void initMemoryWrite(void);
void getDeviceSignatureFromMemory(void);
void writeToMemory(void);
void readFromMemory(void);
void initSignatureWrite(void);
void writeDefaultParamtoMemory(void);
void writeParametertoMemory(enmMemoryOperation memOpr);
void initSystemDefaultsfromMemory(void);
uint32_t getAvailableSpaceInMemoryPage(uint32_t MemAddress);
uint32_t getFirstPageWriteSize(uint32_t pageSpace);
uint32_t getLastPageWriteSize(uint32_t firstPageWriteSize , uint32_t dataLength);
uint32_t getPageWriteCyclesRequired(uint32_t firstPageWriteSize , uint32_t dataLength);
void systemReset(void);

#ifdef __cplusplus
} // extern "C"
#endif


#endif /* INC_USER_EEPROM_H_ */

/*** End of File **************************************************************/
