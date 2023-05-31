/*******************************************************************************
* Title                 :   GSM SIM7600 EI interface
* Filename              :   gsmSIM7600.c
* Author                :   Hrishikesh Limaye
* Origin Date           :   1/06/2021
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
*  01/06/21   1.0.0    Hrishikesh Limaye   Initial Release.
*
*******************************************************************************/

/** @file  gsmSIM7600.c
 *  @brief Utilities for interfacing GSM 4G Module SIM7600-EI
 */

/******************************************************************************
* Includes
*******************************************************************************/
#include "main.h"
#include "externs.h"
#include "applicationdefines.h"
#include <string.h>
#include <stdlib.h>
#include "user_timer.h"
#include "gsmSim7600.h"
#include "queue.h"
#include "payload.h"
#include "user_rtc.h"
#include "deviceinfo.h"
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

strctQUEUE gsmPayload;
strctGSM gsmInstance;
uint8_t gu8HttpPayloadFlag = 0;												/* Update HTTP Payload Flag */
uint8_t gu8CheckSMS = FALSE;

static char gau8GPS_POWERON[]="AT+CGPS?\r\n";//=1,1\r\n";  					/* Start/Stop GPS session */
/* Used for send data gsm command */
static char * gu8GSMDataTimeout=",10000\r\n"; 							/* Data input : Max time in ms */
/* GPRS/GSM Commands */
static char gau8GSM_AT[4]="AT\r\n";												/* Module Attention */
static char gau8GSM_ATE0[6]="ATE0\r\n";    										/* Echo Off */
/*LTE N/W selection */
static char gau8GSM_ATCNMP[13] = "AT+CNMP=38\r\n";								/*Preferred Mode 2- Automatic , 38 - LTE*/
static char gau8GSM_ATCPIN[10]="AT+CPIN?\r\n";									/* Is SIM Ready */
static char gau8GSM_CSMINS[5]="AT\r\n";//+CSMINS?\r\n";    						/* Check if SIMCARD is present */
static char gau8GSM_CSQ[9]="AT+CSQ\r\n";										/* Query Signal Strength */
static char gau8GSM_ATCREG[12]="AT+CREG=1\r\n"; 								/* Registration */
static char gau8GSM_ATCMEE[12]="AT+CMEE=1\r\n"; 								/* Enable numeric error codes  */
static char gau8GSM_ATCGACT[13]="AT+CGACT=?\r\n";								/* Context Activate or Deactivate */
static char gau8GSM_ATCTZU[12] = "AT+CTZU=1\r\n";								/* Automatic time and time zone update */
/*GPS*/
static char gau8GSM_ATCGPSURL[36] = "AT+CGPS=1,1\r\n";							/* Set AGPS default server URL*/
static char gau8GSM_ATCGPSINFOCFG[23] = "AT+CGPSINFO\r\n";			    		/* Report GPS NMEA-0183 sentence , 2 - GPRMC*/
static char gau8GSM_ATCGPSAUTO[]="AT+CGPSAUTO=0\r\n";							/* Start GPS automatic */
/*"AT+CGDCONT=1,\"IP\",\"airteliot.com\"\r\n";
 * jionet*/
static char gau8GSM_ATSETAPN[]="AT+CGDCONT=1,\"IP\",";//\"airteliot.com\"\r\n";		/* Set APN */

/* Copy gau8GSM_ATSETAPN and append APN from configuration */
char gau8GSM_NewSetAPN[50] = "";

static char gau8GSM_ATCMGF[12]="AT+CMGF=1\r\n";									/* SMS Text Mode */
static char gau8GSM_ATHTTPINIT[14]="AT+HTTPINIT\r\n"; 							/* Init HTTP */
static char gau8GSM_ATHTTPPARAURL[150]="AT+HTTPPARA=\"URL\",";					/* HTTP Parameter : URL */
static char gau8GSM_ATHTTPDATA[31]="AT+HTTPDATA=";								/* Set HTTP Data Length and UART Timeout */
static char gau8GSM_ATHTTPDATACOMMAND[30];										/* HTTP Data Command */
char gau8GSM_ATAPN[180] = {'0'};												/* Network APN */
char gau8GSM_ATURL[180] = {'0'};												/* Server URL */
static char gau8GSM_SMSRecepient[180] = {'0'};									/* User Phone Number */
static char gau8GSM_ATHTTPACTION[18]="AT+HTTPACTION=1\r\n";	 					/* Send Data over HTTP  0:GET method 1:POST 2:HEAD method */
static char gau8GSM_ATHTTPACTIONFOTA[18]="AT+HTTPACTION=0\r\n";	 				/* Send Data over HTTP  0:GET method 1:POST 2:HEAD method */
static char gau8GSM_ATHTTPTERN[14]="AT+HTTPTERM\r\n";							/* Terminiate HTTP */
static char gau8GSM_ATCMGL[23]="AT+CMGL=\"REC UNREAD\"\r\n";					/* Display All Unread SMS */
static char gau8GSM_ATCMGD[21]="AT+CMGDA=\"DEL ALL\"\r\n";						/* Delete All Messages */
static char gau8GSM_ATCMGS[27]="AT+CMGS=";										/* Send SMS */
static char gau8GSM_ATCSCS[16]="AT+CSCS=\"GSM\"\r\n";							/* TE Character Set */
static char gau8GSM_SMS_EOM[2]={0x1A};											/* End of SMS Character (ctrl+Z) */
static char gau8GSM_ATCCLK[11]="AT+CCLK?\r\n";									/* Query Time */
static char gau8GSM_ATHTTPREAD[26]={'0'};										/* @Fota : Read File / Server URL Response . TODO: Array Length according to data */

char gau8GSM_smsto[15]="+918669666703";
char gau8GSM_url[150]="https://bluestardevapi.remotemonitor.in/api/values/PostStringData";//"http://59.163.219.179:8025/api/Values/PostStringData";	/* Holds Server URL */
char gau8GSM_apn[100]="AT+CGDCONT=1,\"IP\",\"airteliot.com\"\r\n";//"ioturl.com";//"iot.com"//"airteliot.com";										/* Holds Network APN */
char gau8GSM_DataArray[500]= {'0'};												/* Holds Modbus payload for GSM Payload */
char gau8GSM_TimeStamp[25]={'0'};												/* Stores Network Time Stamp */
char gu8NewURL[150] = "http://59.163.219.179:8025/api/Values/PostStringData";
char gau8GSM4G_apn[100] = "airteliot.com";//"fast.t-mobile.com";//"airteliot.com";


/* Remote Configuration */	//100
char gau8RemoteConfigurationURL[150] = "https://bluestarutility.remotemonitor.in/Values/GetData";//"http://192.168.16.17:8025/Values/GetData";//"http://59.163.219.178:81/modbustwowaycomm/api/insertData/getData";
//char gau8FotaURL[150]="http://20.198.65.195/fota/Terex/Bluestar_Y.hex";
char gau8FotaURL[150]="http://20.198.65.195/fota/Terex/Bluestar_Y_300523.bin";

/* Entire Config */
//"(,2,9430766648761060083866678,A,1,2,1,0,0,0,0,0,0,1,2,1,0,0,0,0,0,0,1,2,1,0,0,0,0,0,0,1,2,1,0,0,0,0,0,0,1,2,1,0,0,0,0,0,0,I,1,1,2000,0,0,0,0,1,1,2000,0,0,0,0,1,1,2000,0,0,0,0,1,1,2000,0,0,0,0,T,1,2000,1,0,0,0,2000,1,0,0,0,2000,1,0,0,0,2000,1,0,0,0,R,1,1,2000,4000,0,0,0,0,0,0,NS,1,http://59.163.219.179:8025/api/Values/PostStringData,http://59.163.219.178:81/modbustwowaycomm/api/insertData/getData,airteliot.com,2000,100000000,2,1,0,100000000,NE,M,1,1,75,115200,1,E,2,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,45,46,47,48,49,50,51,52,53,54,55,56,57,58,59,60,61,62,63,64,65,66,67,68,69,70,71,72,73,74,75,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,4,3,3,3,3,3,3,3,3,9,3,3,3,3,3,3,3,3,30,3,3,3,3,3,3,3,3,3,3,3,23,3,3,3,3,3,3,19,1,5,3,3,3,3,3,3,3,3,3,3,14,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,1000,1000,m,75,192.168.0.255,255.255.255.255,192.168.0.1,502,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,45,46,47,48,49,50,51,52,53,54,55,56,57,58,59,60,61,62,63,64,65,66,67,68,69,70,71,72,73,74,75,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,4,3,3,3,3,3,3,3,3,9,3,3,3,3,3,3,3,3,30,3,3,3,3,3,3,3,3,3,3,3,23,3,3,3,3,3,3,19,1,5,3,3,3,3,3,3,3,3,3,3,14,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,1000,1000,D,^,*,!,L,C,1,500,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,45,46,47,48,49,50,51,52,53,54,55,56,57,58,59,60,61,62,63,64,65,66,67,68,69,70,71,72,73,74,75,c,1,500,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,45,46,47,48,49,50,51,52,53,54,55,56,57,58,59,60,61,62,63,64,65,66,67,68,69,70,71,72,73,74,75,D,0,115200,1,E,2,4000,)";

/* Network Config */
//"(,2,9430766648761060083866678,NS,6,http://59.163.219.179:8025/api/Values/PostStringData,http://59.163.219.178:81/modbustwowaycomm/api/insertData/getData,airteliot.com,2000,100000000889,7,1,0,100000000,NE,)"

/* Modbus 485 Config */
//"(,2,ME,1,1,75,9600,2,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,32,33,34,35,36,37,3338,3339,3340,3341,3342,3343,3344,3345,	3346,3347,65500,65501,65502,65503,65504,65505,65506,65507,65508,65509,65510,65511,65512,65513,65514,65515,65516,65517,65518,65519,65520,65521,65522,65523,65524,65525,65526,65527,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,45,46,47,48,49,50,51,52,53,54,55,56,57,58,59,60,61,62,63,64,65,66,67,68,69,70,71,72,73,74,75,1000,2000,ME,)";

/* Modbus TCP Config */
//"ms,1,1,192.168.0.100,255.255.255.0,192.168.0.200,192.168.0.1,502,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,32,33,34,35,36,37,3338,3339,3340,3341,3342,3343,3344,3345,3346,3347,65500,65501,65502,65503,65504,65505,65506,65507,65508,65509,65510,65511,65512,65513,65514,65515,65516,65517,65518,65519,65520,65521,65522,65523,65524,65525,65526,65527,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,45,46,47,48,49,50,51,52,53,54,55,56,57,58,59,60,61,62,63,64,65,66,67,68,69,70,71,72,73,74,75,1000,2000,me";
char gau8ConfigData[2500];//="(,2,9430766648761060083866678,MS,1,1,75,9600,2,2,3,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,32,33,34,35,36,37,3338,3339,3340,3341,3342,3343,3344,3345,3346,3347,65500,65501,65502,65503,65504,65505,65506,65507,65508,65509,65510,65511,65512,65513,65514,65515,65516,65517,65518,65519,65520,65521,65522,65523,65524,65525,65526,65527,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,45,46,47,48,49,50,51,52,53,54,55,56,57,58,59,60,61,62,63,64,65,66,67,68,69,70,71,72,73,74,75,1000,2000,ME,NS,1,http://59.163.219.179:8025/api/Values/PostStringData,http://59.163.219.178:81/modbustwowaycomm/api/insertData/getData,airteliot.com,2000,1000000,1,1,0,100000000,NE,ms,1,75,192.168.0.100,255.255.255.0,192.168.0.200,192.168.0.1,502,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,32,33,34,35,36,37,3338,3339,3340,3341,3342,3343,3344,3345,3346,3347,65500,65501,65502,65503,65504,65505,65506,65507,65508,65509,65510,65511,65512,65513,65514,65515,65516,65517,65518,65519,65520,65521,65522,65523,65524,65525,65526,65527,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,45,46,47,48,49,50,51,52,53,54,55,56,57,58,59,60,61,62,63,64,65,66,67,68,69,70,71,72,73,74,75,1000,2000,me,)";
char gau32RemoteConfigSizeinBytes[8] = {'0'};
uint32_t gu32RemoteConfigSizeinBytes = 0;
char buffer[6] = {'0'};
char bufferRemBytes[6] = {'0'};
char ConfigbufferChunkBytes[6] = {'0'};
uint32_t gu32NewConfigAvailable = FALSE;

uint32_t u32ConfigFileBaseAddress = 0;
uint32_t u32ConfigFileChunkCounter = 0;
uint32_t u32ConfigFileRemainingBytes = 0;
uint32_t u32ConfigFileStatus = 0;
uint32_t u32ConfigFileReadComplete = 0;
uint32_t u32ConfigChunckLength = 0;
uint32_t u32StartofFotaWrite = 0;

uint8_t gu8SendSMS = FALSE;	 													/* Send SMS Flag */
uint8_t gu8ReadSMS = FALSE;														/* Read SMS Flag */
uint32_t u8LastHttpResponseCode = 0;											/* Last Http Response Code */

uint32_t gu32NWRTCYear = 0;
uint32_t gu32NWRTCMnt = 0;
uint32_t gu32NWRTCDay = 0;

uint32_t gu32NWRTCHH = 0;
uint32_t gu32NWRTCYMM = 0;
uint32_t gu32NWRTCSS = 0;

/* 14-12-2020 . GPS time Sync */
uint32_t u32GPSTimeSyncFlag = FALSE;
uint32_t u32GPSRefTimeHH = 0;
uint32_t u32GPSRefTimeMM = 0;
uint32_t u32GPSRefTimeSS = 0;

uint32_t u32GPSRefDateDD = 0;
uint32_t u32GPSRefDateMM = 0;
uint32_t u32GPSRefDateYY = 0;

extern volatile uint32_t gu32GSMRestartTimer;
uint8_t gu8FlagNoTerminate = 1;

uint32_t gu32AttemptFota = FALSE;

uint32_t u32FOTAFileBaseAddress = 0;
uint32_t u32FotaFileChunkCounter = 0;
uint32_t u32FotaFileRemainingBytes = 0;
uint32_t u32FotaFileStatus = 0;
uint32_t u32FotaFileReadComplete = 0;

uint32_t u32MemoryWriteCycle = FALSE;

/* Identifier for Configuration or FOTA file . added on 16/6/22 by @HL */
uint32_t gu32OTATaskNumber = 1; 	/* 0 - configuration , 1 - Fota file */
uint32_t u32FlashMemoryWriteStatus = 0;
uint32_t u32MemoryEraseStatus = 0;



char * tempStrCongif = NULL;
char * tempStrEndConfig = NULL;

///* Simcom Operation States */
//strctGSMStateTable gsmStateTableArray[45]=
//{
//	{gau8GSM_AT,"OK\r\n",ONE_SEC,&gsmInstance},
//	{gau8GSM_ATE0,"OK\r\n",ONE_SEC,&gsmInstance},
//	{gau8GSM_ATCTZU,"OK\r\n",ONE_SEC,&gsmInstance}, // Tested
//	{gau8GSM_ATCCLK,"+CCLK:",ONE_SEC,&gsmInstance},
//	{gau8GSM_NewSetAPN,"OK\r\n",ONE_SEC,&gsmInstance},//gau8GSM_ATSETAPN
//	{gau8GSM_ATCNMP,"OK\r\n",ONE_SEC,&gsmInstance},
//	{gau8GSM_ATCPIN,"+CPIN: READY",ONE_SEC,&gsmInstance},
//	{gau8GSM_CSMINS,"OK\r\n",ONE_SEC,&gsmInstance},
//	{gau8GSM_CSQ,"+CSQ:",ONE_SEC,&gsmInstance},
//	{gau8GSM_ATCREG,"OK\r\n",ONE_SEC,&gsmInstance},
//	{gau8GSM_ATCMEE,"OK\r\n",ONE_SEC,&gsmInstance},
//	{gau8GSM_ATCGACT,"+CGACT:",ONE_SEC,&gsmInstance},
//	/* SMS Init */
//	{gau8GSM_ATCSCS,"OK\r\n",ONE_SEC,&gsmInstance},
//	/* GPS Init */
//	{gau8GPS_POWERON,"\r\n",ONE_SEC,&gsmInstance},
//	{gau8GSM_ATCGPSURL,"OK\r\n",ONE_SEC,&gsmInstance},
//	{gau8GSM_ATCGPSAUTO,"OK\r\n",ONE_SEC,&gsmInstance},
//	{gau8GSM_ATCGPSINFOCFG,"OK\r\n",ONE_SEC,&gsmInstance},
//	/* HTTP Commands : */
//	{gau8GSM_ATHTTPTERN,"OK\r\n",TEN_SEC,&gsmInstance},
//	{gau8GSM_ATHTTPINIT,"OK\r\n",TWO_MIN,&gsmInstance},
//	{gau8GSM_ATURL,"OK\r\n",ONE_SEC,&gsmInstance},
//	{gau8GSM_ATHTTPDATACOMMAND,"DOWNLOAD",FIVE_SEC,&gsmInstance},
//	{"","OK\r\n",FIVE_SEC,&gsmInstance},
//	{gau8GSM_ATHTTPACTION,"+HTTPACTION:",ONE_MIN,&gsmInstance},
//
//	/* GPS Location Commands:e */
//	{gau8GSM_ATCGPSINFOCFG,"OK\r\n",TEN_SEC,&gsmInstance},
//	/* Send SMS */
//	{gau8GSM_ATCMGF,"OK\r\n",ONE_SEC,&gsmInstance},
//	{gau8GSM_SMSRecepient,">",TEN_SEC,&gsmInstance},
//	{gsmInstance.strSystemSMS.agsmSMSMessageBody,"",TWO_SEC,&gsmInstance},
//	{gau8GSM_SMS_EOM,"+CMGS:",TWENTY_SEC,&gsmInstance},
//	/* Read All Unread SMS */
//	{gau8GSM_ATCMGF,"OK\r\n",ONE_SEC,&gsmInstance},
//	{gau8GSM_ATCMGL,"OK\r\n",TWO_SEC,&gsmInstance},
//	{gau8GSM_ATCMGD,"OK",THIRTY_SEC,&gsmInstance},
//	/* Fota Commands */
//	{gau8GSM_ATHTTPTERN,"OK\r\n",TEN_SEC,&gsmInstance},
//	{gau8GSM_ATHTTPINIT,"OK\r\n",TWO_MIN,&gsmInstance},
//	{gau8GSM_ATURL,"OK",ONE_SEC,&gsmInstance}, //gau8GSM_ATHTTPPARAURL
//	{gau8GSM_ATHTTPACTIONFOTA,"+HTTPACTION:",TWENTY_SEC,&gsmInstance},//+HTTPACTION:
//	{gau8GSM_ATHTTPREAD,"+HTTPREAD:",TWENTY_SEC,&gsmInstance},
//
//};

/* SIM7600 - GH global module tested */
strctGSMStateTable gsmStateTableArray[45]=
{
	{gau8GSM_AT,"OK\r\n",THREE_SEC,&gsmInstance},
	{gau8GSM_ATE0,"OK\r\n",THREE_SEC,&gsmInstance},
	{gau8GSM_ATCTZU,"OK\r\n",THREE_SEC,&gsmInstance}, // Tested
	{gau8GSM_ATCCLK,"+CCLK:",THREE_SEC,&gsmInstance},
	{gau8GSM_NewSetAPN,"OK\r\n",THREE_SEC,&gsmInstance},//gau8GSM_ATSETAPN
	{gau8GSM_ATCNMP,"OK\r\n",THREE_SEC,&gsmInstance},
	{gau8GSM_ATCPIN,"+CPIN: READY",THREE_SEC,&gsmInstance},
	{gau8GSM_CSMINS,"OK\r\n",THREE_SEC,&gsmInstance},
	{gau8GSM_CSQ,"+CSQ:",THREE_SEC,&gsmInstance},
	{gau8GSM_ATCREG,"OK\r\n",THREE_SEC,&gsmInstance},
	{gau8GSM_ATCMEE,"OK\r\n",THREE_SEC,&gsmInstance},
	{gau8GSM_ATCGACT,"+CGACT:",THREE_SEC,&gsmInstance},
	/* SMS Init */
	{gau8GSM_ATCSCS,"OK\r\n",THREE_SEC,&gsmInstance},
	/* GPS Init */
	{gau8GPS_POWERON,"\r\n",THREE_SEC,&gsmInstance},
	{gau8GSM_ATCGPSURL,"OK\r\n",THREE_SEC,&gsmInstance},
	{gau8GSM_ATCGPSAUTO,"OK\r\n",THREE_SEC,&gsmInstance},
	{gau8GSM_ATCGPSINFOCFG,"OK\r\n",THREE_SEC,&gsmInstance},
	/* HTTP Commands : */
	{gau8GSM_ATHTTPTERN,"OK\r\n",THREE_SEC,&gsmInstance},
	{gau8GSM_ATHTTPINIT,"OK\r\n",TWO_MIN,&gsmInstance},
	{gau8GSM_ATURL,"OK\r\n",THREE_SEC,&gsmInstance},
	{gau8GSM_ATHTTPDATACOMMAND,"DOWNLOAD",FIVE_SEC,&gsmInstance},
	{"","OK\r\n",FIVE_SEC,&gsmInstance},
	{gau8GSM_ATHTTPACTION,"+HTTPACTION:",ONE_MIN,&gsmInstance},

	/* GPS Location Commands:e */
	{gau8GSM_ATCGPSINFOCFG,"OK\r\n",THREE_SEC,&gsmInstance},
	/* Send SMS */
	{gau8GSM_ATCMGF,"OK\r\n",ONE_SEC,&gsmInstance},
	{gau8GSM_SMSRecepient,">",TEN_SEC,&gsmInstance},
	{gsmInstance.strSystemSMS.agsmSMSMessageBody,"",TWO_SEC,&gsmInstance},
	{gau8GSM_SMS_EOM,"+CMGS:",TWENTY_SEC,&gsmInstance},
	/* Read All Unread SMS */
	{gau8GSM_ATCMGF,"OK\r\n",ONE_SEC,&gsmInstance},
	{gau8GSM_ATCMGL,"OK\r\n",TWO_SEC,&gsmInstance},
	{gau8GSM_ATCMGD,"OK",THIRTY_SEC,&gsmInstance},
	/* Fota Commands */
	{gau8GSM_ATHTTPTERN,"OK\r\n",TEN_SEC,&gsmInstance},
	{gau8GSM_ATHTTPINIT,"OK\r\n",TWO_MIN,&gsmInstance},
	{gau8GSM_ATURL,"OK",THREE_SEC,&gsmInstance}, //gau8GSM_ATHTTPPARAURL
	{gau8GSM_ATHTTPACTIONFOTA,"+HTTPACTION:",TWENTY_SEC,&gsmInstance},//+HTTPACTION:
	{gau8GSM_ATHTTPREAD,"+HTTPREAD:",TWENTY_SEC,&gsmInstance},

};

/******************************************************************************
* Function Prototypes
*******************************************************************************/

/******************************************************************************
* Function Definitions
*******************************************************************************/
/******************************************************************************
* Function : initGSMSIM868()
*//**
* \b Description:
*
* This function is use to Initialise GSM Structure used for SIM7600 Module
*
* PRE-CONDITION: Enable Relevant UART and its interrupts(Rx)
*
*
* POST-CONDITION:
*
*
* @return 		None.
*
* \b Example Example:
* @code
*
* 	initGSMSIM868();
*
* @endcode
*
* @see
*
* <br><b> - HISTORY OF CHANGES - </b>
*
* <table align="left" style="width:800px">
* <tr><td> Date       </td><td> Software Version </td><td> Initials </td><td> Description </td></tr>
* <tr><td> 01/06/2021 </td><td> 0.0.1            </td><td> HL100133 </td><td> Interface Created </td></tr>
* </table><br><br>
* <hr>
*
*******************************************************************************/
void initGSMSIM868(void)
{
	/* Init Buffers */

	memset(gsmInstance.agpsLocationData, GSM_ARRAY_INIT_CHAR, (GPS_LOCATION_ARRAY_SIZE * sizeof(char)));
	memset(gsmInstance.agsmNetworkIP, GSM_ARRAY_INIT_CHAR, (GPRS_NETWORKIP_ARRAY_SIZE * sizeof(char)));
	memset(gsmInstance.agsmSignalStrength, GSM_ARRAY_INIT_CHAR, (GSM_SIGSTRGTH_ARRAY_SIZE * sizeof(char)));
	memset((char *)gsmInstance.as8GSM_Response_Buff,GSM_ARRAY_INIT_CHAR,(GSM_RESPONSE_ARRAY_SIZE * sizeof(char)));
	memset(gsmInstance.agsmCommandResponse, DATA_UNKNOWN, sizeof(gsmInstance.agsmCommandResponse[0][0])
		   * GSM_TOTAL_COMMANDS * GSM_TOTAL_COMMANDS);
	memset(gsmInstance.agsmHTTPRequestStatus,DATA_UNKNOWN,GSM_HTTP_RESPONSE_CODE_SIZE * sizeof(char));
	memset(gsmInstance.strSystemSMS.agsmSMSRecipient,DATA_UNKNOWN,SMS_MOB_NO_LENGTH * sizeof(char));
	memset(gsmInstance.strSystemSMS.agsmSMSMessageBody,0x00,SMS_MAX_MSG_LENGTH * sizeof(char));
	memset(gsmInstance.u32GSMHttpResponseCode,0x00,sizeof(char) * 3);
	memset(gau8GSM_ATAPN, 0x00, (180 * sizeof(char)));
	memset(gau8GSM_ATURL, 0x00, (180 * sizeof(char)));
	memset(gau8GSM_SMSRecepient, 0x00, ( 180 * sizeof(char)));

	strcat((char *)gau8GSM_SMSRecepient,(char *)gau8GSM_ATCMGS);
	strcat((char *)gau8GSM_SMSRecepient,(char *)"\"");
	strcat((char *)gau8GSM_SMSRecepient,(char *)gau8GSM_smsto);
	strcat((char *)gau8GSM_SMSRecepient,(char *)"\"");
	//strcat((char *)gau8GSM_ATAPN,(char *)gau8GSM_ATSAPRBAPN);
	strcat((char *)gau8GSM_ATAPN,(char *)"\"");
	strcat((char *)gau8GSM_ATAPN,(char *)gau8GSM_apn);
	strcat((char *)gau8GSM_ATAPN,(char *)"\"");
	strcat((char *)gau8GSM_ATURL,(char *)gau8GSM_ATHTTPPARAURL);
	strcat((char *)gau8GSM_ATURL,(char *)"\"");
	strcat((char *)gau8GSM_ATURL,(char *)gau8GSM_url);
	strcat((char *)gau8GSM_ATURL,(char *)"\"");
	strcat(gau8GSM_ATAPN,"\r\n");
	strcat(gau8GSM_ATURL,"\r\n");
	strcat(gau8GSM_SMSRecepient,"\r\n");

	gsmInstance.strSystemSMS.u8NewMessage = FALSE;
	gsmInstance.enmcurrentTask = enmGSMTASK_RESET;
	gsmInstance.enmGSMPwrState = enmGSM_PWRNOTSTARTED;
	gsmInstance.u8isConnected = FALSE;
	gsmInstance.u8GSM_Response_Character_Counter = 0;
	gsmInstance.u8gsmRegistrationStatus = FALSE;
	gsmInstance.u8gsmSIMReadyStatus = FALSE;
	gsmInstance.u8gsmRetryCount = GSM_MAX_RETRY;
	gsmInstance.u8AttemptFota = FALSE;
	gsmInstance.u32GSMTimer = ONE_SEC;
	gu32GSMHangTimer = THREE_MIN;
	gsmInstance.u32GSMHeartbeatTimer = 0;
	gsmInstance.u8IllegalHttpResponseCounter = 0;
	gsmInstance.enmGSMCommandResponseState = enmGSM_SENDCMD;
	gsmInstance.enmGSMCommand = enmGSMSTATE_ATE0;
	gsmInstance.enmGSMCommandState = enmGSM_CMDSEND;
	gu8FlagNoTerminate = 1;
}

/******************************************************************************
* Function : operateGSMSIM868()
*//**
* \b Description:
*
* This function is use to operate GSM Structure used for SIM7600 Module
*
* PRE-CONDITION: Enable Relevant UART and its interrupts(Rx)
*
*
* POST-CONDITION:
*
*
* @return 		None.
*
* \b Example Example:
* @code
*
* 	operateGSMSIM868();
*
* @endcode
*
* @see
*
* <br><b> - HISTORY OF CHANGES - </b>
*
* <table align="left" style="width:800px">
* <tr><td> Date       </td><td> Software Version </td><td> Initials </td><td> Description </td></tr>
* <tr><td> 01/06/2021 </td><td> 0.0.1            </td><td> HL100133 </td><td> Interface Created </td></tr>
* <tr><td> 25/12/2021 </td><td> 0.0.1            </td><td> HL100133 </td><td> Timeout added for EI module </td></tr>
* </table><br><br>
* <hr>
*
*******************************************************************************/
void operateGSMSIM868()
{
	if(gsmInstance.u32GSMTimer != 0)
		return;

	/* Operate Current Task */
	switch(gsmInstance.enmcurrentTask)
	{
		case enmGSMTASK_RESET:
			if(gsmInstance.enmGSMPwrState == enmGSM_PWRNOTSTARTED)
			{
				HAL_GPIO_WritePin(GSM_PWR_KEY_GPIO_Port,GSM_PWR_KEY_Pin,GPIO_PIN_SET);
				gsmInstance.u32GSMTimer = FIVE_SEC;
				gsmInstance.enmGSMPwrState = enmGSM_PWRSTARTED;
			}
			else if(gsmInstance.enmGSMPwrState == enmGSM_PWRSTARTED)
			{
				HAL_GPIO_WritePin(GSM_PWR_KEY_GPIO_Port,GSM_PWR_KEY_Pin,GPIO_PIN_RESET);
				gsmInstance.u32GSMTimer = TWO_SEC;
				gsmInstance.enmcurrentTask = enmGSMTASK_INITMODULE;
				gsmInstance.enmGSMPwrState = enmGSM_PWRCOMPLETED;
			}
			break;

		case enmGSMTASK_INITMODULE:
			/* Initialize Module : ATE0 to HTTP URL */
			if(gsmInstance.enmGSMCommandState == enmGSM_CMDTIMEOUT)
			{
				/* Reset Module */
				initGSMSIM868();
			}
			else
			{
				/* Send Command and Check Response */
				if(gsmInstance.enmGSMCommand == enmGSMSTATE_ATHTTPDATACOMMAND)
				{
					if(gu8CheckSMS == TRUE)
					{
						gsmInstance.enmcurrentTask = enmGSMTASK_UPLOADDATA;
						gsmInstance.enmGSMCommand = enmGSMSTATE_ATHTTPDATACOMMAND;
						gu8CheckSMS = FALSE;
					}
					else
					{
						/* Module is initialized ,Check Received SMS */
						/* 31-3-2020 : Altered for testing FOTA File Read */
						gsmInstance.enmGSMCommand = enmGSMSTATE_GPSINFO;
						gsmInstance.enmcurrentTask = enmGSMTASK_UPDATELOCATION;
						/* Only For Testing */

					}
				}
				else
				{
					sendGSMCommand(gsmInstance);
				}
			}
			break;

		case enmGSMTASK_ISALIVE:
			if(gsmInstance.u32GSMHeartbeatTimer == 0)
			{
				sendGSMCommand(gsmInstance);
			}
			break;

		case enmGSMTASK_READSMS:
			if(gu8ReadSMS == TRUE)
				sendGSMCommand(gsmInstance);
			else
			{
				gsmInstance.enmGSMCommand = enmGSMSTATE_GPSINFO;
				gsmInstance.enmcurrentTask = enmGSMTASK_UPDATELOCATION;
				gsmInstance.u32GSMTimer = TWO_SEC;
			}

			break;

		case enmGSMTASK_SENDSMS:
			if(gu8SendSMS == TRUE)
				sendGSMCommand(gsmInstance);
			else
			{
				gsmInstance.enmGSMCommand = enmGSMSTATE_GPSINFO;
				gsmInstance.enmcurrentTask = enmGSMTASK_UPDATELOCATION;
				gsmInstance.u32GSMTimer = TWO_SEC;
			}
			break;

		case enmGSMTASK_UPDATELOCATION:
			/*Every 30 Sec */
			sendGSMCommand(gsmInstance);
			break;

		case enmGSMTASK_UPLOADDATA:
		{

			/* Send Data from the Queue with upload Time Interval */
			if((isQueueEmpty(&gsmPayload) == FALSE )&& (gsmPayload.data[gsmPayload.tail] != NULL))
			{
				if((gu8HttpPayloadFlag == 0) && (gsmInstance.enmGSMCommand == enmGSMSTATE_ATHTTPDATACOMMAND))
				{
					/* Update Payload Length once Every Cycle */
					updateHttpDataLength();
					gu8HttpPayloadFlag = 1;
				}
				else if((gu8HttpPayloadFlag == 1) && (gsmInstance.enmGSMCommand == enmGSMSTATE_SENDDATA))
				{
					/* Flag to update new data length */
					gu8HttpPayloadFlag = 0;
				}
				else
				{
					sendGSMCommand(gsmInstance);
				}
			}
			else
			{
				gsmInstance.enmGSMCommand = enmGSMSTATE_GPSINFO;
				gsmInstance.enmcurrentTask = enmGSMTASK_UPDATELOCATION;
			}
		}
			break;

		case enmGSMTASK_GETDATA:
			/* Provision for Two Way communication with Server : ex Modbus . Not Implemented
			 * In this Firmware */
//				sendGSMCommand(gsmInstance);
			initGSMSIM868();
			break;

		case enmGSMTASK_DOWNLOADFOTAFILE:
				sendGSMCommand(gsmInstance);
			break;

		case enmGSMTASK_IDLE:
			gsmInstance.enmcurrentTask = enmGSMTASK_UPLOADDATA;
			gsmInstance.enmGSMCommand = enmGSMSTATE_ATHTTPDATACOMMAND;
			break;

		default:
			/* Log illegal State Error */
			//Error_Callback(enmERROR_GSM_ILLSTATE);
			//gsmInstance.enmcurrentTask = enmGSMTASK_UPLOADDATA;
			//gsmInstance.enmGSMCommand = enmGSMSTATE_ATHTTPDATACOMMAND;
			initGSMSIM868();
			break;
	}
	gu32GSMHangTimer = THREE_MIN;
}

/******************************************************************************
* Function : sendGSMCommand()
*//**
* \b Description:
*
* This function is use to send commands to GSM module
*
* PRE-CONDITION: Enable Relevant UART and its interrupts(Rx)
*
*
* POST-CONDITION:
*
*
* @return 		None.
*
* \b Example Example:
* @code
*
* 	sendGSMCommand();
*
* @endcode
*
* @see
*
* <br><b> - HISTORY OF CHANGES - </b>
*
* <table align="left" style="width:800px">
* <tr><td> Date       </td><td> Software Version </td><td> Initials </td><td> Description </td></tr>
* <tr><td> 01/06/2021 </td><td> 0.0.1            </td><td> HL100133 </td><td> Interface Created </td></tr>
* <tr><td> 25/12/2021 </td><td> 0.0.1            </td><td> HL100133 </td><td> Timeout added for EI module </td></tr>
* </table><br><br>
* <hr>
*
*******************************************************************************/

void sendGSMCommand()
{
	static char * command;
	static uint32_t u8CharacterCounter;
	static uint32_t u8LoopCounter ;

	switch (gsmInstance.enmGSMCommandResponseState)
	{
		case enmGSM_SENDCMD:
			if(gsmInstance.enmGSMCommandState == enmGSM_CMDSEND)
			{
				u8LoopCounter = 0;
				command = NULL;
				if(gsmInstance.enmGSMCommand == enmGSMSTATE_SENDDATA)
				{
#if (DATA_PROCESSING_METHOD == FIFO )
					command = gsmPayload.data[gsmPayload.tail];
#elif  (DATA_PROCESSING_METHOD == LIFO)
					command = gstrGMSPayloadLIFO.au8PayloadStack[gstrGMSPayloadLIFO.u32Stacktop];
#endif
				}
				else
					command = gsmStateTableArray[gsmInstance.enmGSMCommand].atCommand;

				u8CharacterCounter = strlen((const char *)command);
				LL_USART_TransmitData8(UART4,command[u8LoopCounter++]);
				gsmInstance.enmGSMCommandState = enmGSM_CMDINPROCESS;

			}
			else if(gsmInstance.enmGSMCommandState == enmGSM_CMDINPROCESS)
			{
				if(!LL_USART_IsActiveFlag_TXE(UART4))
				{
					/*Do Nothing . Wait For Previous Character Transmission */
				}
				else
				{
					if(u8LoopCounter < (u8CharacterCounter))
					{
						LL_USART_TransmitData8(UART4,command[u8LoopCounter++]);
					}
					else
					{
						u8LoopCounter = 0;
						gu32GSMCharacterTimeout = FIVEHUNDRED_MS;
						u8CharacterCounter = 0;
						gsmInstance.enmGSMCommandResponseState = enmGSM_CHKRESPONSE;
						gsmInstance.u32GSMResponseTimer = gsmStateTableArray[gsmInstance.enmGSMCommand].msTimeOut;

						if((gsmInstance.enmGSMCommand == enmGSMSTATE_FOTAHTTPACTION) || (gsmInstance.enmGSMCommand == enmGSMSTATE_HTTPACTION))
							gu32FotaFileReadTimer = FIFTEEN_SEC;
						else
							gu32FotaFileReadTimer = 0;
					}
				}
			}
			else
			{
				initGSMSIM868();
			}

			break;

			/*if(gsmInstance.enmGSMCommand == enmGSMSTATE_FOTAHTTPACTION )*/
		case enmGSM_CHKRESPONSE:
			if((gu32GSMCharacterTimeout == 0) && (gsmInstance.u32GSMResponseTimer != 0) && (u8GSMCharRcv == 1) && (gu32FotaFileReadTimer == 0))
			{
				/* Parse Response */
				if(strstr((const char *)gsmInstance.as8GSM_Response_Buff
						,(const char *)gsmStateTableArray[gsmInstance.enmGSMCommand].atCommandResponse) != NULL)
				{
					/*Required Response Received */
					switch(gsmInstance.enmGSMCommand)
					{

						case enmGSMSTATE_AT:
							gsmInstance.u8IncrementGsmState = TRUE;
							break;

						case enmGSMSTATE_ATE0:
							gsmInstance.u8IncrementGsmState = TRUE;
							break;

						case enmGSMSTATE_GETTIMESTAMP:
							/* Parse and update local RTC . Add Flag check for update
							 * +CCLK: "18/06/21,12:00:21+22" */
							{
								char * strLoc = strstr((const char *)gsmInstance.as8GSM_Response_Buff
												,(const char *)"+CCLK:");
								memset(gau8GSM_TimeStamp,0x00,(sizeof(char ) * strlen(gau8GSM_TimeStamp)));
								memcpy( gau8GSM_TimeStamp, &strLoc[8], strlen(strtok(&strLoc[8],"\r")));
								syncrtcwithNetworkTime();
								gsmInstance.u8IncrementGsmState = TRUE;
							}
							break;

						case enmGSMSTATE_SETAPN:
							gsmInstance.u8IncrementGsmState = TRUE;
							break;

						case enmGSMSTATE_ATCNMP:
							gsmInstance.u8IncrementGsmState = TRUE;
							break;

						case enmGSMSTATE_ATCPIN:
							gsmInstance.u8IncrementGsmState = TRUE;
							break;

						case enmGSMSTATE_ATCSMINS:
							gsmInstance.u8IncrementGsmState = TRUE;
							break;

						case enmGSMSTATE_ATCSQ:
							{
							/* Store Signal Strength . Bug 19012019 Cut Copy-Solved	 */
								char * strLoc = strstr((const char *)gsmInstance.as8GSM_Response_Buff
									,(const char *)"+CSQ");
								memcpy(gsmInstance.agsmSignalStrength, &strLoc[6], strlen(strtok(strLoc,"\r")));
								if(gsmInstance.enmcurrentTask == enmGSMTASK_ISALIVE)
								{
									gsmInstance.u32GSMHeartbeatTimer = ONE_MIN;
									gsmInstance.u32GSMTimer = FIVE_SEC;
									gsmInstance.enmcurrentTask = enmGSMTASK_UPLOADDATA;
									gsmInstance.enmGSMCommand = enmGSMSTATE_ATHTTPDATACOMMAND;
									gsmInstance.u8IncrementGsmState = FALSE;
								}
								else
									gsmInstance.u8IncrementGsmState = TRUE;
							}
							break;

						case enmGSMSTATE_CREG:
							gsmInstance.u8IncrementGsmState = TRUE;
							break;

						case enmGSMSTATE_CMEE:
							gsmInstance.u8IncrementGsmState = TRUE;
							break;

						case enmGSMSTATE_CGACT:
							gsmInstance.u8IncrementGsmState = TRUE;
							break;

						case enmGSMSTATE_CTZU:
							gsmInstance.u8IncrementGsmState = TRUE;
							break;

						case enmGSMSTATE_CMGF:
							gsmInstance.u8IncrementGsmState = TRUE;
							break;

						case enmGSMSTATE_CSCS:
							gsmInstance.u8IncrementGsmState = TRUE;
							break;

						case enmGSMSTATE_GPSPOWERON:
							if(strstr((const char *)gsmInstance.as8GSM_Response_Buff
							,(const char *)"+CGPS: 1,1") != NULL)
							{
								// GPS is enabled . No need of Power ON command
								gsmInstance.u8IncrementGsmState = FALSE;
								gsmInstance.enmGSMCommand += 2;
							}
							else
								gsmInstance.u8IncrementGsmState = TRUE;

							break;

						case enmGSMSTATE_AGPSURL:
							gsmInstance.u8IncrementGsmState = TRUE;
							break;

						case enmGSMSTATE_GPSAUTOEN:
							gsmInstance.u8IncrementGsmState = TRUE;
							break;

						case enmGSMSTATE_GPSINFOCFG:
							if(gu8FlagNoTerminate == 1)
							{
								gu8FlagNoTerminate = 0;
								gsmInstance.enmGSMCommand++;
							}
							gsmInstance.u8IncrementGsmState = TRUE;
							break;

						case enmGSMSTATE_HTTPINIT:
							gsmInstance.u8IncrementGsmState = TRUE;
							break;

						case enmGSMSTATE_HTTPPARAURL:
							gsmInstance.u8HTTPInitStatus = TRUE;
							gsmInstance.u8IncrementGsmState = TRUE;
							break;

						case enmGSMSTATE_ATHTTPDATACOMMAND:
							memcpy(gsmInstance.agsmCommandResponse[gsmInstance.enmGSMCommand],
									(char *)&gsmInstance.as8GSM_Response_Buff, sizeof(gsmInstance.as8GSM_Response_Buff));
							gsmInstance.u8IncrementGsmState = TRUE;
							break;

						case enmGSMSTATE_SENDDATA:
							gsmInstance.u8IncrementGsmState = TRUE;
							break;

						case enmGSMSTATE_HTTPACTION:

							/* Data Uploaded Successfully . Upload Next data packet */
							/* Process HTTP Response code */
							{
							char * strLoc = strstr((const char *)gsmInstance.as8GSM_Response_Buff
																			,(const char *)": ");
							memcpy(gsmInstance.u32GSMHttpResponseCode, (char *)&strLoc[4], 3);
							/* Parse HTTP Response Code */
							switch(atoi(gsmInstance.u32GSMHttpResponseCode))
							{
								case 200:
								/* HTTP Request Successful . Send Next Packet */
								dequeue(&gsmPayload);
								gsmInstance.u32GSMTimer = TWO_SEC; // Replace with Upload Frequency
								gu8HttpPayloadFlag = 0;
								gsmInstance.enmGSMCommand = enmGSMSTATE_ATHTTPDATACOMMAND;
								gsmInstance.u8IncrementGsmState = FALSE;
								u8LastHttpResponseCode = atoi(gsmInstance.u32GSMHttpResponseCode);
								HAL_GPIO_TogglePin(LED_3_GPIO_Port, LED_3_Pin);
								gu32GSMRestartTimer = FIVE_MIN;
								break;


								default:
									/* Log and Change State : Tested CSQ and DATA Upload in loop
									 * When Service is not responding with 200 maintain counter and reset*/
									u8LastHttpResponseCode = atoi(gsmInstance.u32GSMHttpResponseCode);
									gsmInstance.u8IllegalHttpResponseCounter++;
									if(gsmInstance.u8IllegalHttpResponseCounter >= MAX_HTTP_ATTEMPTS)
									{
										/* Log and Reset the modem */
										initGSMSIM868();
										gu8CheckSMS = TRUE;
										break;
									}
									u8LastHttpResponseCode = atoi(gsmInstance.u32GSMHttpResponseCode);
									gsmInstance.enmGSMCommand = enmGSMSTATE_ATHTTPDATACOMMAND;
									gsmInstance.u32GSMResponseTimer = 0;
									gsmInstance.u8IncrementGsmState = FALSE;

								break;
								}
							}
							break;

						case enmGSMSTATE_HTTPTERM:
							gsmInstance.u8IncrementGsmState = TRUE;
							break;

						case enmGSMSTATE_GPSINFO:
						{
							/*Changed on 11 02 2018 . Garbage Location Logged . Change - [12] -> [11]
							 * 1830.616769,N,07351.175558,E,080721,072645.0,559.2,0.0,0.0\0\n\r\nOK\r\n */
							char * strLoc = strstr((const char *)gsmInstance.as8GSM_Response_Buff
																	,(const char *)"+CGPSINFO:");
							memset(gsmInstance.agpsLocationData,0x00, strlen(gsmInstance.agpsLocationData));
							memcpy(gsmInstance.agpsLocationData, (char *)&strLoc[11], strlen(strtok(strLoc,"\r")));//strlen(strtok(strLoc,"\r\n")));
							//gsmInstance.enmcurrentTask = enmGSMTASK_UPLOADDATA;
							//gsmInstance.enmGSMCommand = enmGSMSTATE_ATHTTPDATACOMMAND;
							gsmInstance.u8IncrementGsmState = FALSE;
							if(strlen(gsmInstance.agpsLocationData) < 10)
							{
								/* GPS Data not Available */
								u32GPSTimeSyncFlag = FALSE;
							}
							else
								u32GPSTimeSyncFlag = TRUE;
							if(gu32GSMConfigCheckTimer == 0)
							{
								/* Backup generated payload strings before reset */
								gu32GSMConfigCheckTimer = TWENTY_MIN;
								initHTTPURLforRemoteConfig();
								gsmInstance.enmcurrentTask = enmGSMTASK_DOWNLOADFOTAFILE;
								gsmInstance.enmGSMCommand = enmGSMSTATE_HTTPTERMCONFIG;
							}
							else
							{
								gsmInstance.enmcurrentTask = enmGSMTASK_UPLOADDATA;
								gsmInstance.enmGSMCommand = enmGSMSTATE_ATHTTPDATACOMMAND;
							}
						}
							break;

						case enmGSMSTATE_SENDSMS:
							gsmInstance.u8IncrementGsmState = TRUE;
							break;

						case enmGSMSTATE_SMSCMGS:
							gsmInstance.u8IncrementGsmState = TRUE;
							break;

						case enmGSMSTATE_SMSEOM:
							gsmInstance.u32GSMHeartbeatTimer = FIVE_SEC;
							gsmInstance.u32GSMTimer = ONE_SEC;
							gsmInstance.enmGSMCommand = enmGSMSTATE_ATCSQ;
							gsmInstance.enmcurrentTask = enmGSMTASK_ISALIVE;
							gsmInstance.u8IncrementGsmState = FALSE;
							gu8SendSMS = FALSE;
							break;

						case enmGSMSTATE_READMODE:
							gsmInstance.u8IncrementGsmState = TRUE;
							break;

						case enmGSMSTATE_READSMS:
							/* Check for all applicable commands */

							break;

						case enmGSMSTATE_DELETESMS:
							sendSystemConfigurationSMS();
							gsmInstance.enmcurrentTask = enmGSMTASK_SENDSMS;
							gsmInstance.enmGSMCommand = enmGSMSTATE_CMGF;
							memset((char *)gsmInstance.as8GSM_Response_Buff, GSM_ARRAY_INIT_CHAR, (GSM_RESPONSE_ARRAY_SIZE));
							gsmInstance.u8IncrementGsmState = FALSE;
							break;

						case enmGSMSTATE_HTTPTERMCONFIG:
							/* Not Implemented */
							gsmInstance.u8IncrementGsmState = TRUE;
							break;

						case enmGSMSTATE_HTTPTINITCONFIG:
							gsmInstance.u8IncrementGsmState = TRUE;
							break;
							/* Fota */
						case enmGSMSTATE_SETFOTAURL:
							gsmInstance.u8IncrementGsmState = TRUE;
							break;

						case enmGSMSTATE_FOTAHTTPACTION:

							{
								char * strLoc = strstr((const char *)gsmInstance.as8GSM_Response_Buff
																	,(const char *)"200,");
								if(strLoc != NULL)
								{
									gu32GSMHangTimer = THREE_MIN;
									memset(gau32RemoteConfigSizeinBytes,0x00, strlen(gau32RemoteConfigSizeinBytes));
									memcpy(gau32RemoteConfigSizeinBytes, &strLoc[4], strlen(strtok(&strLoc[4],"\r\n")));

									/*Extract Size bytes */
									gu32RemoteConfigSizeinBytes = strlen(gau32RemoteConfigSizeinBytes);
									gsmInstance.gu32RemoteConfigSizeinBytes = atoi(gau32RemoteConfigSizeinBytes);
									/*
									 *
									 */
									if((gsmInstance.gu32RemoteConfigSizeinBytes) <= ((STM32_FLASHSIZE /2) * 1000))//(MAX_RMT_CONFIG_SIZE_BYTES))
									{
										/* File Size is Valid */
										gu32AttemptFota = TRUE;
										if(gsmInstance.gu32RemoteConfigSizeinBytes != 0)
											updateHTTPReadLength(gsmInstance.gu32RemoteConfigSizeinBytes);
										else
										{
											/*Error File Size is not valid */
										}
									}
									else
									{
										/* Abort Fota / Raise Error / Continue with regular operation */
										gsmInstance.u8IncrementGsmState = FALSE;
										/* Raise size error and change state back to HTTP Upload */
										gsmInstance.enmcurrentTask = enmGSMTASK_UPLOADDATA;
										gsmInstance.enmGSMCommand = enmGSMSTATE_ATHTTPDATACOMMAND;
									}
									gsmInstance.u8IncrementGsmState = TRUE;
								}
								else
								{
									/* Response Does not contain 200 . Read Failed . Issue Alart / Failure */
									gsmInstance.u8IncrementGsmState = FALSE;
									//gu32AttemptFota = FALSE;
									gsmInstance.enmcurrentTask = enmGSMTASK_UPLOADDATA;
									gsmInstance.enmGSMCommand = enmGSMSTATE_ATHTTPDATACOMMAND;
								}
							}
							break;

							case enmGSMSTATE_READFILE:
								{
								/* Read and Parse Received response and file
								 * +HTTPREAD: 800\r\n\r
								 *
								 *
									gsmInstance.enmcurrentTask = enmGSMTASK_UPLOADDATA;
									gsmInstance.enmGSMCommand = enmGSMSTATE_ATHTTPDATACOMMAND;
								  */
									if(gu32OTATaskNumber == 0)
									{
										char *strloc = strstr((const char *)gsmInstance.as8GSM_Response_Buff,(const char *)"(");
										memset(gau8ConfigData,0x00,sizeof(gau8ConfigData));//
										memcpy(gau8ConfigData, (char *)strloc, gsmInstance.gu32RemoteConfigSizeinBytes);
										restoreHTTPURLforData();
										gsmInstance.enmcurrentTask = enmGSMTASK_UPLOADDATA;
										gsmInstance.enmGSMCommand = enmGSMSTATE_HTTPTERM;
										HAL_GPIO_WritePin(LED_1_GPIO_Port, LED_1_Pin, GPIO_PIN_RESET);	// LED ON
										gsmInstance.u8IncrementGsmState = FALSE;
										gu32NewConfigAvailable = TRUE;
									}
									else if(gu32OTATaskNumber == 1)
									{
										/* Download .bin FOTA file */
										if(u32ConfigFileReadComplete == 0)//u32FotaFileReadComplete
										{
											char *ptr = strstr((const char *)&gsmInstance.as8GSM_Response_Buff,"+HTTPREAD:");
											char *ptr2 = strstr(ptr,"\n");
											memset(gau8ConfigData,0x00,sizeof(gau8ConfigData));
											memcpy(gau8ConfigData,(ptr2+1),gsmInstance.gu32RemoteConfigSizeinBytes);
//											memset(gau8ConfigData,0x00,sizeof(gau8ConfigData));
//											memcpy(gau8ConfigData,(const char *)&gsmInstance.as8GSM_Response_Buff[30],1024);
											/* More chuncks available */
											if(u32MemoryWriteCycle == FALSE)
											{
												/* Erase the Sector */
												if(u32StartofFotaWrite == 0)
												{
													FLASH_If_Init();
													if(FLASH_If_Erase_User(0,getNewFirmwareStorageLocation()) != FLASHIF_OK)
													{
														/* Error Erasing Flash */
														restoreHTTPURLforData();
														gsmInstance.enmcurrentTask = enmGSMTASK_UPLOADDATA;
														gsmInstance.enmGSMCommand = enmGSMSTATE_ATHTTPDATACOMMAND;
													}
													u32StartofFotaWrite = 1;
												}

												/* Write Received Chucnk to memory
												 * Need to update u32LastMemoryWriteLocation according to the sector 'X' or 'Y' */
												u32FlashMemoryWriteStatus = WriteDatatoFlash(u32LastMemoryWriteLocation,(uint8_t *)gau8ConfigData,1024,1);
												if(u32FlashMemoryWriteStatus == SUCCESS)
												{
													/* Memory Block Write Complete */
													u32MemoryWriteCycle = TRUE;
													updateHTTPReadLength(gsmInstance.gu32RemoteConfigSizeinBytes);
													memset((char *)gsmInstance.as8GSM_Response_Buff, GSM_ARRAY_INIT_CHAR,(GSM_RESPONSE_ARRAY_SIZE * sizeof(uint8_t)));
												}
												else
												{
													/*Memory Write Failed . Raise Error and Back to HTTP Upload */
													restoreHTTPURLforData();
													gsmInstance.enmcurrentTask = enmGSMTASK_UPLOADDATA;
													gsmInstance.enmGSMCommand = enmGSMSTATE_ATHTTPDATACOMMAND;
												}
											}
											else
											{
												/* Write Next Chunck to Internal Flash */
//												FLASH_If_Init();
												u32FlashMemoryWriteStatus = WriteDatatoFlash(u32LastMemoryWriteLocation,(uint8_t *)gau8ConfigData,1024,1);
												if(u32FlashMemoryWriteStatus == SUCCESS)
												{
													/* Memory Block Write Complete */
													updateHTTPReadLength(gsmInstance.gu32RemoteConfigSizeinBytes);
												}
												else
												{
													/*Memory Write Failed . Raise Error and Back to HTTP Upload */
													restoreHTTPURLforData();
													gsmInstance.enmcurrentTask = enmGSMTASK_UPLOADDATA;
													gsmInstance.enmGSMCommand = enmGSMSTATE_ATHTTPDATACOMMAND;
												}
											}
											/* Read the file */
											gsmInstance.u8IncrementGsmState = FALSE;
										}
										else if(u32ConfigFileReadComplete == 1)
										{
											char *ptr = strstr((const char *)&gsmInstance.as8GSM_Response_Buff,"+HTTPREAD:");
											char *ptr2 = strstr(ptr,"\n");
//											memset(gau8FotaData,0x00,sizeof(gau8FotaData));
	//										memcpy(gau8FotaData,(const char *)&gsmInstance.as8GSM_Response_Buff[18],gsmInstance.u32FotaFileSizeBytes);
//											memcpy(gau8FotaData,(ptr2+1),gsmInstance.ConfigFileSizeBytes);
											memset(gau8ConfigData,0x00,sizeof(gau8ConfigData));
											memcpy(gau8ConfigData,(ptr2+1),gsmInstance.gu32RemoteConfigSizeinBytes);

											if(u32MemoryWriteCycle == FALSE)
											{
												/*
												 * USE CASE : File Size is Less than CHUNK SIZE = 2k
												 * */
												/* Initialise Flash Opr Flags . Erase required sector and write new FW */
												FLASH_If_Init();
												u32MemoryEraseStatus = FLASH_If_Erase_User(0,getNewFirmwareStorageLocation());
												if(u32MemoryEraseStatus != FLASHIF_OK)
												{
													/* Error Erasing Flash .Terminate FW Upgrade */
													restoreHTTPURLforData();
													gsmInstance.enmcurrentTask = enmGSMTASK_UPLOADDATA;
													gsmInstance.enmGSMCommand = enmGSMSTATE_ATHTTPDATACOMMAND;
												}
											}//
											u32FlashMemoryWriteStatus = WriteDatatoFlash(u32LastMemoryWriteLocation,(uint8_t *)gau8ConfigData,1024,1);

											/* Memory Write Successful */
											if(flashWriteBootSection(getNewFirmwareStorageLocation()) == 0)
											{
												/* Boot Info Updated Successfully*/
												gu32AttemptFota =  FALSE;
												if(u32MemoryWriteCycle == TRUE)
													u32MemoryWriteCycle = FALSE;

												/* Jump to New User Application */
												/* Reset system to boot with new FW */
												HAL_Delay(50000);
											}
											else
											{
												/* Boot Info Updation Failed . FW Upadte Cant be initiated
												 * Risky to Jump.
												 *  */
												restoreHTTPURLforData();
												gsmInstance.enmcurrentTask = enmGSMTASK_UPLOADDATA;
												gsmInstance.enmGSMCommand = enmGSMSTATE_ATHTTPDATACOMMAND;
											}

											/* Write Chunk to memory . Checksum check and Application Jump.*/
											u32MemoryWriteCycle = TRUE;

										}
										else
										{
											/* Http Upload */
											restoreHTTPURLforData();
											gsmInstance.enmcurrentTask = enmGSMTASK_UPLOADDATA;
											gsmInstance.enmGSMCommand = enmGSMSTATE_ATHTTPDATACOMMAND;
										}
									}
									else
									{
										/*Continue HTTP Upload */
										gsmInstance.enmcurrentTask = enmGSMTASK_UPLOADDATA;
										gsmInstance.enmGSMCommand = enmGSMSTATE_ATHTTPDATACOMMAND;
									}
								}

							break;

						default:
							memcpy(gsmInstance.agsmCommandResponse[gsmInstance.enmGSMCommand], (char *)&gsmInstance.as8GSM_Response_Buff, sizeof(gsmInstance.as8GSM_Response_Buff));
							break;
						}

					gsmInstance.u8GSM_Response_Character_Counter = 0;
					gsmInstance.enmGSMCommandState = enmGSM_CMDSEND;
					gsmInstance.enmGSMCommandResponseState = enmGSM_SENDCMD;
					gsmInstance.u8gsmRetryCount = GSM_MAX_RETRY;
					gsmInstance.u32GSMResponseTimer = 0;
					u8GSMCharRcv = 0;
					gsmInstance.u8GSM_Response_Character_Counter = 0;
					if(gsmInstance.enmGSMCommand != enmGSMSTATE_READFILE)
						memset((char *)gsmInstance.as8GSM_Response_Buff, GSM_ARRAY_INIT_CHAR,(GSM_RESPONSE_ARRAY_SIZE * sizeof(uint8_t))); /* Clear Response Buffer */

					if(gsmInstance.u8IncrementGsmState == TRUE)
					{
						gsmInstance.enmGSMCommand++;
						gsmInstance.u32GSMTimer = ONE_SEC;
					}
				}
				else
				{
					/* Response not found : Try Again */
					gsmInstance.u8GSM_Response_Character_Counter = 0;
					gsmInstance.enmGSMCommandResponseState = enmGSM_CMDSEND;
					memcpy(gsmInstance.agsmCommandResponse[gsmInstance.enmGSMCommand], (char *)&gsmInstance.as8GSM_Response_Buff, sizeof(gsmInstance.as8GSM_Response_Buff));
					memset((char *)gsmInstance.as8GSM_Response_Buff, GSM_ARRAY_INIT_CHAR,(GSM_RESPONSE_ARRAY_SIZE * sizeof(uint8_t))); /* Clear Response Buffer */
					u8GSMCharRcv = 0;
				}
			}// end of if((gu32GSMCharacterTimeout == 0) && (gsmInstance.u32GSMResponseTimer != 0) && (u8GSMCharRcv == 1))
			else if(gsmInstance.u32GSMResponseTimer == 0)
			{
				/* Time Out */
				gsmInstance.u8gsmRetryCount--;
				if(gsmInstance.u8gsmRetryCount == 0)
				{
					/* Max Retry Attempt Reached Yet No Response . Reset the modem */
					/* Clear Response Buffer */
					memset((char *)gsmInstance.as8GSM_Response_Buff, GSM_ARRAY_INIT_CHAR, (GSM_RESPONSE_ARRAY_SIZE));
					if(gsmInstance.enmcurrentTask == enmGSMTASK_UPLOADDATA)
					{
						/* Check SMS even if upload data was not successful */
						gu8CheckSMS = TRUE;
					}
					initGSMSIM868();
				}
				else
				{
					/*Send Same Command Again */
					gsmInstance.enmGSMCommandState = enmGSM_CMDSEND;
					gsmInstance.enmGSMCommandResponseState = enmGSM_SENDCMD;
					/* Clear Response Buffer */
					memset((char *)gsmInstance.as8GSM_Response_Buff, GSM_ARRAY_INIT_CHAR, (GSM_RESPONSE_ARRAY_SIZE));
					gsmInstance.u32GSMTimer = ONE_SEC;
				}
				gsmInstance.u32GSMResponseTimer = 0;
				u8GSMCharRcv = 0;
			}

			break;
	}
}

/******************************************************************************
* Function : updateHttpDataLength()
*//**
* \b Description:
*
* This function is use to Configure data length and timeout for gsm data
* gau8GSM_ATHTTPDATA = datalength,timeout\r\n;
*
* PRE-CONDITION:
*
*
* POST-CONDITION:
*
*
* @return 		None.
*
* \b Example Example:
* @code
*
* 	updateHttpDataLength();
*
* @endcode
*
* @see
*
* <br><b> - HISTORY OF CHANGES - </b>
*
* <table align="left" style="width:800px">
* <tr><td> Date       </td><td> Software Version </td><td> Initials </td><td> Description </td></tr>
* <tr><td> 01/09/2021 </td><td> 0.0.1            </td><td> HL100133 </td><td> Interface Created </td></tr>
*
* </table><br><br>
* <hr>
*
*******************************************************************************/
void updateHttpDataLength()
{
	char * tempdata = "";
	tempdata = gsmPayload.data[gsmPayload.tail];

	uint32_t payloadLength = strlen(tempdata);
	char buffer[payloadLength];
	memset(buffer, 0x00, (payloadLength * sizeof(char))); /* Clear Response Buffer */

	/* Convert Integer to ASCII ( Decimal) */
	memset(gau8GSM_ATHTTPDATACOMMAND, 0x00, ( 30 * sizeof(char)));
	itoa(payloadLength,buffer,PAYLOAD_DATA_STRING_RADIX);
	strcat(buffer,gu8GSMDataTimeout);
	strcat((char *)gau8GSM_ATHTTPDATACOMMAND,(char *)gau8GSM_ATHTTPDATA);
	strcat((char *)gau8GSM_ATHTTPDATACOMMAND,buffer);
}


/******************************************************************************
* Function : sendSystemConfigurationSMS()
*//**
* \b Description:
*
* This function is use to update SMS information
* NOT USED
*
* PRE-CONDITION:
*
*
* POST-CONDITION:
*
*
* @return 		None.
*
* \b Example Example:
* @code
*
* 	sendSystemConfigurationSMS();
*
* @endcode
*
* @see
*
* <br><b> - HISTORY OF CHANGES - </b>
*
* <table align="left" style="width:800px">
* <tr><td> Date       </td><td> Software Version </td><td> Initials </td><td> Description </td></tr>
* <tr><td> 01/09/2021 </td><td> 0.0.1            </td><td> HL100133 </td><td> Interface Created </td></tr>
*
* </table><br><br>
* <hr>
*
*******************************************************************************/
void sendSystemConfigurationSMS(void)
{
	/* Tor Signature */
	strcpy(gsmInstance.strSystemSMS.agsmSMSMessageBody,(char *)KLOUDQ_SIGNATURE);
	strcat(gsmInstance.strSystemSMS.agsmSMSMessageBody,(char *)"Config: \r\n");
	/*Tor Version */
	strcat(gsmInstance.strSystemSMS.agsmSMSMessageBody,(char *)"Version: \r\n");
	strcat(gsmInstance.strSystemSMS.agsmSMSMessageBody,(char *)MODEL_NUMBER);
	strcat(gsmInstance.strSystemSMS.agsmSMSMessageBody,"\r\n");
	/* Tor Device Id */
	strcat(gsmInstance.strSystemSMS.agsmSMSMessageBody,(char *)"Id: ");
	strcat(gsmInstance.strSystemSMS.agsmSMSMessageBody,buffuuid2);
	strcat(gsmInstance.strSystemSMS.agsmSMSMessageBody,buffuuid1);
	strcat(gsmInstance.strSystemSMS.agsmSMSMessageBody,buffuuid0);
	strcat(gsmInstance.strSystemSMS.agsmSMSMessageBody,"\r\n");
	/* Tor Signal Strength in RSSI */
	strcat(gsmInstance.strSystemSMS.agsmSMSMessageBody,(char *)"RSSI: ");
	strcat(gsmInstance.strSystemSMS.agsmSMSMessageBody,gsmInstance.agsmSignalStrength);
	strcat(gsmInstance.strSystemSMS.agsmSMSMessageBody,"\r\n");
	/* Tor Network IP , if connected */
	strcat(gsmInstance.strSystemSMS.agsmSMSMessageBody,(char *)"IP: ");
	strcat(gsmInstance.strSystemSMS.agsmSMSMessageBody,gsmInstance.agsmNetworkIP);
	strcat(gsmInstance.strSystemSMS.agsmSMSMessageBody,"\r\n");
	/* Tor Network APN */
	strcat(gsmInstance.strSystemSMS.agsmSMSMessageBody,(char *)"APN: ");
	strcat(gsmInstance.strSystemSMS.agsmSMSMessageBody,gau8GSM_apn);
	strcat(gsmInstance.strSystemSMS.agsmSMSMessageBody,"\r\n");
	/* Tor Server URL */
	strcat(gsmInstance.strSystemSMS.agsmSMSMessageBody,(char *)"URL: ");
	strcat(gsmInstance.strSystemSMS.agsmSMSMessageBody,gau8GSM_url);
	strcat(gsmInstance.strSystemSMS.agsmSMSMessageBody,"\r\n");

	/* Tor Upload Frequency */
	char ontime[10];
	char offtime[10];
	itoa(gsmInstance.u32ONPayloadUploadFreq,ontime,10);
	itoa(gsmInstance.u32OFFPayloadUploadFreq,offtime,10);
	strcat(gsmInstance.strSystemSMS.agsmSMSMessageBody,(char *)"Up Freq ON: ");
	strcat(gsmInstance.strSystemSMS.agsmSMSMessageBody,ontime);
	strcat(gsmInstance.strSystemSMS.agsmSMSMessageBody,"\r\n");
	strcat(gsmInstance.strSystemSMS.agsmSMSMessageBody,(char *)"Up Freq OFF: ");
	strcat(gsmInstance.strSystemSMS.agsmSMSMessageBody,offtime);
	strcat(gsmInstance.strSystemSMS.agsmSMSMessageBody,"\r\n");

	/* Last Known Location and time */
	strcat(gsmInstance.strSystemSMS.agsmSMSMessageBody,(char *)"Location: ");
	strcat(gsmInstance.strSystemSMS.agsmSMSMessageBody,gsmInstance.agpsLocationData);
	strcat(gsmInstance.strSystemSMS.agsmSMSMessageBody,"\r\n");

	/*Last HTTP Status Code */
	char httpresp[5];
	itoa(u8LastHttpResponseCode,httpresp,10);
	strcat(gsmInstance.strSystemSMS.agsmSMSMessageBody,(char *)"HTTP Code : ");
	strcat(gsmInstance.strSystemSMS.agsmSMSMessageBody,httpresp);
	strcat(gsmInstance.strSystemSMS.agsmSMSMessageBody,"\n");
}

/******************************************************************************
* Function : updatePhoneNumber()
*//**
* \b Description:
*
* This function is use to Update Phone number (SMS to)
* NOT USED
*
* PRE-CONDITION:
*
*
* POST-CONDITION:
*
*
* @return 		None.
*
* \b Example Example:
* @code
*
* 	updatePhoneNumber();
*
* @endcode
*
* @see
*
* <br><b> - HISTORY OF CHANGES - </b>
*
* <table align="left" style="width:800px">
* <tr><td> Date       </td><td> Software Version </td><td> Initials </td><td> Description </td></tr>
* <tr><td> 01/09/2021 </td><td> 0.0.1            </td><td> HL100133 </td><td> Interface Created </td></tr>
*
* </table><br><br>
* <hr>
*
*******************************************************************************/
void updatePhoneNumber(void)
{
	memset(gau8GSM_smsto,0x00,sizeof(char) * 15);
	memcpy(gau8GSM_smsto, (char *)&gsmInstance.as8GSM_Response_Buff[25]
		,(strlen(strtok((char *)&gsmInstance.as8GSM_Response_Buff[26],","))));
	memset(gau8GSM_SMSRecepient, 0x00, ( 180 * sizeof(char)));
	strcat((char *)gau8GSM_SMSRecepient,(char *)gau8GSM_ATCMGS);
	strcat((char *)gau8GSM_SMSRecepient,(char *)"\"");
	strcat((char *)gau8GSM_SMSRecepient,(char *)gau8GSM_smsto);
	strcat((char *)gau8GSM_SMSRecepient,(char *)"\"");
	strcat(gau8GSM_SMSRecepient,"\r\n");
	sendSystemConfigurationSMS();
	gu8SendSMS = TRUE;
}

/******************************************************************************
* Function : syncrtcwithNetworkTime()
*//**
* \b Description:
*
* This function is use to Sync Local RTC with N/W Time
* Updates RTC Structure values with network time
	Network Time format : "yy/MM/dd,hh:mm:ss � zz"
	zz - time zone
	(indicates the difference, expressed in quarters of an hour, between the
	local time and GMT; range -47...+48)

	 E.g. 6th of May 2010, 00:01:52
	 GMT+2 hours equals to "10/05/06,00:01:52+08".
*
*
* PRE-CONDITION:
*
*
* POST-CONDITION:
*
*
* @return 		None.
*
* \b Example Example:
* @code
*
* 	syncrtcwithNetworkTime();
*
* @endcode
*
* @see
*
* <br><b> - HISTORY OF CHANGES - </b>
*
* <table align="left" style="width:800px">
* <tr><td> Date       </td><td> Software Version </td><td> Initials </td><td> Description </td></tr>
* <tr><td> 01/09/2021 </td><td> 0.0.1            </td><td> HL100133 </td><td> Interface Created </td></tr>
*
* </table><br><br>
* <hr>
*
*******************************************************************************/
uint32_t gu32Year = 0;
uint32_t gu32Month = 0;
uint32_t gu32Date = 0;
uint32_t gu32Hours = 0;
uint32_t gu32Minutes = 0;
uint32_t gu32Seconds = 0;
void syncrtcwithNetworkTime(void)
{

//	if(gu32TimeSyncFlag == 1)
//		return;

	gu32Year = (((gau8GSM_TimeStamp[0]-'0') * 10) + (gau8GSM_TimeStamp[1]-'0'));
	gu32Month = (((gau8GSM_TimeStamp[3]-'0') * 10) + (gau8GSM_TimeStamp[4]-'0'));
	gu32Date = (((gau8GSM_TimeStamp[6]-'0') * 10) + (gau8GSM_TimeStamp[7]-'0'));

	gu32Hours = (((gau8GSM_TimeStamp[9]-'0') * 10) + (gau8GSM_TimeStamp[10]-'0'));
	gu32Minutes = (((gau8GSM_TimeStamp[12]-'0') * 10) + (gau8GSM_TimeStamp[13]-'0'));
	gu32Seconds = (((gau8GSM_TimeStamp[15]-'0') * 10) + (gau8GSM_TimeStamp[16]-'0'));

	SDate1.Year = DecimalToBCD(gu32Year);
	SDate1.Month = DecimalToBCD(gu32Month);
	SDate1.Date = DecimalToBCD(gu32Date);
	STime1.Hours = DecimalToBCD(gu32Hours);
	STime1.Minutes = DecimalToBCD(gu32Minutes);
	STime1.Seconds = DecimalToBCD(gu32Seconds);

	strTimeUpdate.u32RefTimeHH = gu32Hours;
	strTimeUpdate.u32RefTimeMin = gu32Minutes;
	strTimeUpdate.u32RefTimeSec = gu32Seconds;

	/* Update/ Set RTC Structure */
	HAL_RTC_SetTime(&hrtc,&STime1,RTC_FORMAT_BCD);
	HAL_RTC_SetDate(&hrtc,&SDate1,RTC_FORMAT_BCD);

	/* Network Time Sync complete (Indicator) */
	gu32TimeSyncFlag = 1;
}

/******************************************************************************
* Function : initHTTPURLforRemoteConfig()
*//**
* \b Description:
*
* This function is use to Update HTTP URL for Remote Config Update
*
*
* PRE-CONDITION:
*
*
* POST-CONDITION:
*
*
* @return 		None.
*
* \b Example Example:
* @code
*
* 	initHTTPURLforRemoteConfig();
*
* @endcode
*
* @see
*
* <br><b> - HISTORY OF CHANGES - </b>
*
* <table align="left" style="width:800px">
* <tr><td> Date       </td><td> Software Version </td><td> Initials </td><td> Description </td></tr>
* <tr><td> 01/09/2021 </td><td> 0.0.1            </td><td> HL100133 </td><td> Interface Created </td></tr>
*
* </table><br><br>
* <hr>
*
*******************************************************************************/
void initHTTPURLforRemoteConfig(void )
{
	if(gu32OTATaskNumber == 0)
	{
		/* Update Configuration URL */
		memset(gau8GSM_ATURL, 0, sizeof(gau8GSM_ATURL) * sizeof(char));
		strcpy((char *)gau8GSM_ATURL,(char *)gau8GSM_ATHTTPPARAURL);
		strcat((char *)gau8GSM_ATURL,(char *)"\"");
		strcat((char *)gau8GSM_ATURL,(char *)gau8RemoteConfigurationURL);
		strcat((char *)gau8GSM_ATURL,(char *)"?id=");
		strcat((char *)gau8GSM_ATURL,(char *)dinfo);
		strcat((char *)gau8GSM_ATURL,(char *)"\"");
		strcat((char *)gau8GSM_ATURL,"\r\n");
	}
	else if(gu32OTATaskNumber == 1)
	{
		/* Update FOTA URL */
		memset(gau8GSM_ATURL, 0, sizeof(gau8GSM_ATURL) * sizeof(char));
		strcpy((char *)gau8GSM_ATURL,(char *)gau8GSM_ATHTTPPARAURL);
		strcat((char *)gau8GSM_ATURL,(char *)"\"");
		strcat((char *)gau8GSM_ATURL,(char *)gau8FotaURL);
		strcat((char *)gau8GSM_ATURL,(char *)"?id=");
		strcat((char *)gau8GSM_ATURL,(char *)dinfo);
		strcat((char *)gau8GSM_ATURL,(char *)"&fota=1");
		strcat((char *)gau8GSM_ATURL,(char *)"\"");
		strcat((char *)gau8GSM_ATURL,"\r\n");
	}
}

/******************************************************************************
* Function : restoreHTTPURLforData()
*//**
* \b Description:
*
* This function is use to Restore HTTP Data URL .URL for Data and Fota will always be different .
	FW has to handle proper selection .
*
*
* PRE-CONDITION:
*
*
* POST-CONDITION:
*
*
* @return 		None.
*
* \b Example Example:
* @code
*
* 	restoreHTTPURLforData();
*
* @endcode
*
* @see
*
* <br><b> - HISTORY OF CHANGES - </b>
*
* <table align="left" style="width:800px">
* <tr><td> Date       </td><td> Software Version </td><td> Initials </td><td> Description </td></tr>
* <tr><td> 01/09/2021 </td><td> 0.0.1            </td><td> HL100133 </td><td> Interface Created </td></tr>
*
* </table><br><br>
* <hr>
*
*******************************************************************************/
void restoreHTTPURLforData(void)
{
	strcpy((char *)gau8GSM_ATURL,(char *)gau8GSM_ATHTTPPARAURL);
	strcat((char *)gau8GSM_ATURL,(char *)"\"");
	strcat((char *)gau8GSM_ATURL,(char *)gau8GSM_url);
	strcat((char *)gau8GSM_ATURL,(char *)"\"");
	strcat((char *)gau8GSM_ATURL,"\r\n");
}

/******************************************************************************
* Function : updateNetworkAPN()
*//**
* \b Description:
*
* This function is use to update APN from configuration
*
* PRE-CONDITION:
*
*
* POST-CONDITION:
*
*
* @return 		None.
*
* \b Example Example:
* @code
*
* 	updateHTTPReadLength();
*
* @endcode
*
* @see
*
* <br><b> - HISTORY OF CHANGES - </b>
*
* <table align="left" style="width:800px">
* <tr><td> Date       </td><td> Software Version </td><td> Initials </td><td> Description </td></tr>
* <tr><td> 01/09/2021 </td><td> 0.0.1            </td><td> HL100133 </td><td> Interface Created </td></tr>
*
* </table><br><br>
* <hr>
*
*******************************************************************************/
void updateNetworkAPN(void)
{
	memset(gau8GSM_NewSetAPN, 0x00, sizeof(gau8GSM_NewSetAPN) * sizeof(char));
	//gau8GSM_NewSetAPN
	strcpy((char *)gau8GSM_NewSetAPN,(char *)gau8GSM_ATSETAPN);
	strcat((char *)gau8GSM_NewSetAPN,(char *)"\"");
	strcat((char *)gau8GSM_NewSetAPN,(char *)gau8GSM4G_apn);
	strcat((char *)gau8GSM_NewSetAPN,(char *)"\"");
	strcat((char *)gau8GSM_NewSetAPN,"\r\n");
}

/******************************************************************************
* Function : updateHTTPReadLength()
*//**
* \b Description:
*
* This function is use to Read Next chunk of FOTAFILECHUNKSIZEBYTES Bytes of Config File
* 	AT+HTTPREAD = <start_address> , <byte_size> \r\n
    Test Fota File size is 29630 Bytes .
	(fotaFileSizeBytes / 2000) + 1 = (29630/2000) + 1 = (14.8) + 1 = 15
	interations/read to get complete file
*
* PRE-CONDITION:
*
*
* POST-CONDITION:
*
*
* @return 		None.
*
* \b Example Example:
* @code
*
* 	updateHTTPReadLength();
*
* @endcode
*
* @see
*
* <br><b> - HISTORY OF CHANGES - </b>
*
* <table align="left" style="width:800px">
* <tr><td> Date       </td><td> Software Version </td><td> Initials </td><td> Description </td></tr>
* <tr><td> 01/09/2021 </td><td> 0.0.1            </td><td> HL100133 </td><td> Interface Created </td></tr>
*
* </table><br><br>
* <hr>
*
*******************************************************************************/
#define FOTAFILECHUNKSIZEBYTES	(1024)
uint32_t updateHTTPReadLength(uint32_t ConfigFileSizeBytes)
{

	if(ConfigFileSizeBytes != 0)
	{
		/* Start of File */
		if(u32ConfigFileBaseAddress == 0)
		{
			u32ConfigFileChunkCounter = (ConfigFileSizeBytes / FOTAFILECHUNKSIZEBYTES);
			/* Calculates Remaining data bytes after multiples of 2000  */
			u32ConfigFileRemainingBytes = (ConfigFileSizeBytes % FOTAFILECHUNKSIZEBYTES);
			itoa(FOTAFILECHUNKSIZEBYTES,ConfigbufferChunkBytes,PAYLOAD_DATA_STRING_RADIX);
			itoa(u32ConfigFileRemainingBytes,bufferRemBytes,PAYLOAD_DATA_STRING_RADIX);
		}

		strcpy(gau8GSM_ATHTTPREAD,(char *)"AT+HTTPREAD=");
		memset(buffer,0x00,sizeof(char) * sizeof(buffer));

		if(u32ConfigFileBaseAddress < u32ConfigFileChunkCounter)
		{
			itoa((u32ConfigFileBaseAddress * FOTAFILECHUNKSIZEBYTES),buffer,PAYLOAD_DATA_STRING_RADIX);
			strcat(gau8GSM_ATHTTPREAD,(char *)buffer);
			strcat(gau8GSM_ATHTTPREAD,(char *)",");
			strcat(gau8GSM_ATHTTPREAD,(char *)ConfigbufferChunkBytes); /* Byte(s) Chunk to read*/
			strcat(gau8GSM_ATHTTPREAD,(char *)"\r\n");
			u32ConfigFileBaseAddress++;
			gsmInstance.gu32RemoteConfigSizeinBytes = FOTAFILECHUNKSIZEBYTES;
			return 2;
		}
		else
		{
//			itoa(u32ConfigFileChunkCounter * 2000,buffer,PAYLOAD_DATA_STRING_RADIX);
			itoa(u32ConfigFileChunkCounter * FOTAFILECHUNKSIZEBYTES,buffer,PAYLOAD_DATA_STRING_RADIX);
			strcat(gau8GSM_ATHTTPREAD,(char *)buffer);
			strcat(gau8GSM_ATHTTPREAD,(char *)",");
			strcat(gau8GSM_ATHTTPREAD,(char *)bufferRemBytes); /* Byte(s) Chunk to read*/
			strcat(gau8GSM_ATHTTPREAD,(char *)"\r\n");
			gsmInstance.gu32RemoteConfigSizeinBytes = u32ConfigFileRemainingBytes;
			u32ConfigFileBaseAddress = 0;
			u32ConfigFileChunkCounter = 0;
			u32ConfigFileRemainingBytes = 0;
			u32ConfigFileReadComplete = 1;
			return 1;
		}
	}
	else
		return 0;
}

//***************************************** End of File ********************************************************

