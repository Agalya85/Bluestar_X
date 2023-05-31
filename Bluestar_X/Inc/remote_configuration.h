/*******************************************************************************
* Title                 :   Remote Configuration - Over the Air
* Filename              :   remote_configuration.h
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
*    Date    Version   Author         	  Description
*  08/03/22   1.0.0    Hrishikesh Limaye   Initial Release.
*
*******************************************************************************/

/** @file  remote_configuration.h
 *  @brief Remote Configuration - Over the Air utilities for the application
 */
#ifndef REMOTE_CONFIGURATION_H_
#define REMOTE_CONFIGURATION_H_
/******************************************************************************
* Includes
*******************************************************************************/
/******************************************************************************
* Module Preprocessor Constants
*******************************************************************************/

/******************************************************************************
* Module Preprocessor Macros
*******************************************************************************/

/******************************************************************************
* Module Typedefs
*******************************************************************************/

/******************************************************************************
* Module Variables
*******************************************************************************/

/******************************************************************************
* Module Preprocessor Macros
*******************************************************************************/

/******************************************************************************
* Module Typedefs
*******************************************************************************/
/******************************************************************************
* Module Variable Definitions
*******************************************************************************/

/**
* This enumeration is a list of device types supporting remote config
*/
typedef enum
{
	enmDEVICETYPE_TORMINI = 0,
	enmDEVICETYPE_TOR2G,
	enmDEVICETYPE_TOR4G
}enmMyDevice;
/**
* This enumeration is a list of peripheral(s) supported by configuration stack
*/
typedef enum
{
	enmCONFIG_DEVTYPE = 0,
	enmCONFIG_ADC,
	enmCONFIG_DI,
	enmCONFIG_DO,
	enmCONFIG_RPM,
	enmCONFIG_NETWORK,
	enmCONFIG_MODBUS485,
	enmCONFIG_MODBUSTCP,
	enmCONFIG_DATAMGMT,
	enmCONFIG_CAN_1,
	enmCONFIG_CAN_2,
	enmCONFIG_DEBUG,
	enmCONFIG_MQTT,
	enmCONFIG_WRITETOMEMORY
}enmRemoteConfig;
/**
* This enumeration is a list of N/W config errors
*/
typedef enum
{
	enmCONFIG_NWISENABLED = 1,
	enmCONFIG_NWSERURLLEN,
	enmCONFIG_NWCONFURLLEN,
	enmCONFIG_NWAPN,
	enmCONFIG_NWUPFREQSUPPLY,
	enmCONFIG_NWUPFREQBATT,
	enmCONFIG_NWTYPE,
	enmCONFIG_NWGPSEN,
	enmCONFIG_NWSENDDATAEN,
	enmCONFIG_NWSENDDATADISABLE,
	enmCONFIG_NWCONFIGNOTFOUND
}enmNetworkConfigErrors;
/**
* This enumeration is a list of MB485 config errors
*/
typedef enum
{
	enmCONFIG_MB485ISENABLED = 1,
	enmCONFIG_MB485SLAVEID,
	enmCONFIG_MB485TERMINATE,
	enmCONFIG_MB485DATAPOINTS,
	enmCONFIG_MB485UARTBR,
	enmCONFIG_MB485UARTSTARTBIT,
	enmCONFIG_MB485UARTSTOPBIT,
	enmCONFIG_MB485UARTPARITY,
	enmCONFIG_MB485ADDRESS,
	enmCONFIG_MB485FUCNTIONCODE,
	enmCONFIG_MB485DATALEN,
	enmCONFIG_MB485POLLTIME,
	enmCONFIG_MB485RESPTIME,
	enmCONFIG_MBTCPIPDEVICE,
	enmCONFIG_MBTCPIPSUBNET,
	enmCONFIG_MBTCPIPSERVER,
	enmCONFIG_MBTCPIPGATEWAY,
	enmCONFIG_MBTCPIPPORT,
	enmCONFIG_MB485CONFIGNOTFOUND
}enmModbus485ConfigErrors;
/**
* This enumeration is a list of MBTCP config errors
*/
typedef enum
{
	enmCONFIG_MBTCPISENABLED = 1,
	enmCONFIG_MBTCPDATAPOINTS,
	enmCONFIG_MBTCPIPDEVICE1,
	enmCONFIG_MBTCPIPSUBNET1,
	enmCONFIG_MBTCPIPSERVER1,
	enmCONFIG_MBTCPIPGATEWAY1,
	enmCONFIG_MBTCPIPPORT1,
	enmCONFIG_MBTCPADDRESS,
	enmCONFIG_MBTCPFUNCTIONCODE,
	enmCONFIG_MBTCPDATALEN,
	enmCONFIG_MBTCPPOLLTIME,
	enmCONFIG_MBTCPRESPTIME,
	enmCONFIG_MBTCPCONFIGNOTFOUND
}enmModbusTCPConfigErrors;


/******************************************************************************
* Function Prototypes
*******************************************************************************/

#ifdef __cplusplus
extern "C"{
#endif

void verifyRemoteConfiguration(void);
uint32_t checkRemoteDeviceID (void);
void parseRemoteconfig(void);
void extractNewtorkConfiguration(void);
void extractModbus485Configuration(void);
void extractModbusTCPConfiguration(void);
uint32_t getTokenNumberfromConfigString(char * str);
uint32_t getTokenNumberfromIPConfigString(char * str);
uint32_t validateIPParams(char * ipStr);
void getLastKnownConfiguration(void);

#ifdef __cplusplus
} // extern "C"
#endif

#endif /* REMOTE_CONFIGURATION_H_ */

//***************************************** End of File ********************************************************
