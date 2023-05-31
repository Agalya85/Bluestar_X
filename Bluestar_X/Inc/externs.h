/*******************************************************************************
* Title                 :   Application Extern Variables
* Filename              :   externs.h
* Author                :   Hrishikesh Limaye
* Origin Date           :   28/02/2022
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
*  28/02/22   1.0.0    Hrishikesh Limaye   Initial Release.
*
*******************************************************************************/

/** @file  externs.h
 *  @brief externs variables for application
 */
#ifndef __EXTERNS_H
#define __EXTERNS_H
/******************************************************************************
* Includes
*******************************************************************************/
#include "user_rtc.h"
#include "modbus_rs485.h"
#include "user_eeprom.h"
#include "applicationdefines.h"
#include "modbus_tcp.h"
#include "gsmSim7600.h"
#include "queue.h"
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
/* Network Config */
extern uint32_t gu32UploadTimeSupply;
extern uint32_t gu32UploadTimeBatt;
/* 485 Config */
extern uint32_t gu32OperateModbus485;
extern uint32_t gu32Modbus485UARTBR;
extern uint32_t gu32Modbus485PollingTime;
extern uint32_t gu32Modbus485RegisterFetch;
extern uint32_t gu32Modbus485SlaveID;
extern uint32_t gu32Modbus485ResponseTime ;

extern uint32_t gu32MBRTUClientFuncCode[75];
extern uint32_t gu32MBRTUClientAddress[75];
extern uint32_t gu32MBRTUClientNoofPoints[75];

/* TCP Config */
extern uint32_t gu32OperateModbusTCP;
extern uint32_t gu32ModbusTCPRegisterFetch;
extern uint32_t gu32ModbusTCPPollingTime;
extern uint32_t gu32ModbusTCPResponseTime;
extern uint32_t gu32LoopCounter;

extern uint32_t gu32MBTCPClientFuncCode[75];
extern uint32_t gu32MBTCPClientAddress[75];
extern uint32_t gu32MBTCPClientNoofPoints[75];

extern uint8_t IP_ADDRESS[4];
extern uint8_t NETMASK_ADDRESS[4];
extern uint8_t GATEWAY_ADDRESS[4];

extern char IP_ADDRESS_DESTI[4];
extern uint32_t gu32ModbusTCPPort;

extern RTC_HandleTypeDef hrtc;
extern char dinfo[100];
extern ETH_HandleTypeDef heth;

extern strctModbusTCPMaster strMbClient;

/* ADC */
extern uint32_t gu32ADCOperationTimer;
extern uint32_t gu32ADCPollTimer;
extern float gfInputSupplyVoltage;
extern float gfBatteryVoltage;
extern uint32_t gu32InputSupplySwitchedAlert;

/* RTC */
extern RTC_DateTypeDef SDate1;
extern RTC_TimeTypeDef STime1;
extern strTimeElapsedAftersync strTimeUpdate;
extern char gau8Year[5];
extern char gau8Month[5];
extern char gau8Date[5];
extern char gau8Hour[5];
extern char gau8Minutes[5];
extern char gau8Seconds[5];

extern uint8_t aShowTime[50], aShowTimeStamp[50];
extern uint8_t aShowDate[50], aShowDateStamp[50];

extern RTC_DateTypeDef sTimeStampDateget_backup;
RTC_TimeTypeDef sTimeStampget_backup;

/*MB*/
extern strctModbusMaster master;
extern enmMODBUSFSMState modbusState;
extern uint32_t gu32MBTCPTimer;
extern uint32_t gu32MBPOLLTimer;
extern uint32_t gu32MBDataByteCounterLimit;
extern uint32_t gu32MBDataByteCounterLimitRTU;
extern uint32_t gu32MBDataByteCounterLimit;
extern volatile uint32_t gu32MBRTUClientConnectedFlag;
extern volatile uint32_t gu32MBClientConnectedFlag;
extern char gau8TempMBPayloadString[1100];
extern char gau8TempMBRTUPayloadString[1100];
extern uint8_t gu8MBFrameTransmittedFlag;

/*Eth*/
extern uint32_t gu32EthLinkStatus;
extern uint32_t gu32EthLinkAlert;

/* Timer */
extern volatile uint32_t gu32GSMHangTimer;
extern uint32_t gu32GSMCharacterTimeout;
extern uint32_t gu32TimeSyncFlag;
extern uint32_t gu32MBCharacterDelay ;
extern uint32_t gu32ModbusFrameEndTimer;
extern uint32_t gu32ModbusResponseTimeout;
extern uint32_t gu32I2CMemoryOperationTimeout;
extern volatile uint32_t gu32TempPayloadQueueEnqueue;
extern volatile uint32_t gu32GSMRestartTimer;
extern uint32_t gu32RTCUpdateTimer;
extern volatile uint32_t gu32TimeSyncVariable;
extern volatile uint32_t gu32LedTimer;
extern volatile uint32_t gu32LinkDisconnectTimer;

extern volatile uint32_t gu32LinkDownTimer;
extern volatile uint32_t gu32SystemResetTimer;

/*GSM*/
extern volatile uint32_t u8GSMCharRcv;
extern char gu8NewURL[150];
extern char gau8GSM_url[150];
extern char gau8GSM_smsto[15];
extern char gau8GSM4G_apn[100];
extern char gau8RemoteConfigurationURL[150];
extern uint32_t u32GPSTimeSyncFlag;
extern volatile uint8_t u8MBQueryCharacterCounter;
extern volatile uint32_t gu32FotaFileReadTimer;
extern char gau8ConfigData[2500];

extern uint32_t gu32NewConfigAvailable;

extern strctQUEUE gsmPayload;
/*dinfo*/
extern char buffuuid0[32];
extern char buffuuid1[32];
extern char buffuuid2[32];
extern char buffflash[32];

extern uint32_t u32ConfigFileReadComplete ;
/*Memory*/
extern volatile uint32_t u32DefautParamWriteStatus;
extern uint8_t gu8SignatureReadFlag;
extern uint8_t gu8RestoreSystemSettings;
extern volatile uint8_t gu8MemoryCycleComplete;
extern uint8_t gu32MemoryOperation;
extern uint8_t gu8OperateSystemStatus;

extern uint8_t gau8MemoryOperationWrite[I2CMEM_MAX_OPRATIONS];//{FALSE}; // 12 for non config operations
extern uint8_t gau8MemoryOperationRead[I2CMEM_MAX_OPRATIONS];

/* Remote Config */
extern uint32_t ga8ConfigNWLength;
extern uint32_t ga8ConfigMB485Length;
extern uint32_t ga8ConfigMBTCPLength;

extern uint32_t gu3ConfigLength;
extern uint32_t u32LastMemoryWriteLocation;

extern  char * nwconfig ;
extern  char * mb485config ;
extern  char * mbTCPconfig ;

extern char gau8LastKnownConfiguration[2000];
extern char gau8ConfigurationCopy[2000];

extern uint32_t gu32ConfigNetworkErrorDatabase;
extern uint32_t gu32ConfigModbus485ErrorDatabase;
extern uint32_t gu32ConfigModbusTCPErrorDatabase;

extern char gau8ConfigModbus485UARTBR[8];

extern volatile uint32_t gu32GSMConfigCheckTimer;
extern volatile uint32_t gu32UploadFailTimer;

/******************************************************************************
* Function Prototypes
*******************************************************************************/
#ifdef __cplusplus
extern "C"{
#endif

#ifdef __cplusplus
} // extern "C"
#endif

#endif

//***************************************** End of File ********************************************************
