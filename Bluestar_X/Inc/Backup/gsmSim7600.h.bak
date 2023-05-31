/*******************************************************************************
* Title                 :   2G Cellular Module Interfacing
* Filename              :   gsmSim7600.h
* Author                :   Hrishikesh Limaye
* Origin Date           :   24/11/2021
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
*  24/11/21   1.0.0    Hrishikesh Limaye   Initial Release.
*******************************************************************************/

/** @file  gsmSim868.h
*  @brief GSM HTTP / SMS / GPS and utility Functions
*
*  This is the header file for the definition(s) related to 4G Cellular module
*/
#ifndef GSMSIM7600_H_
#define GSMSIM7600_H_
/******************************************************************************
* Includes
*******************************************************************************/
/******************************************************************************
* Module Preprocessor Constants
*******************************************************************************/
/**
* \brief Defines GSM Max Retries \a .
  \param None.
  \returns None \a .
*/
#define GSM_MAX_RETRY 				(5)
/**
* \brief Defines GPS Max Retries \a .
  \param None.
  \returns None \a .
*/
#define GPS_MAX_RETRY 				(5)
/**
* \brief Defines Payload data string radix \a .
  \param None.
  \returns None \a .
*/
#define PAYLOAD_DATA_STRING_RADIX   (10) 		/* 10-> Decimal 2-> Binary 16-> Hex */
/**
* \brief Defines GSM Response Array Size \a .
  \param None.
  \returns None \a .
*/
#define GSM_RESPONSE_ARRAY_SIZE     (2500)
/**
* \brief Defines GPS Response Array Size \a .
  \param None.
  \returns None \a .
*/
#define GPS_LOCATION_ARRAY_SIZE     (150)
/**
* \brief Defines GSM Signal Strength array size \a .
  \param None.
  \returns None \a .
*/
#define GSM_SIGSTRGTH_ARRAY_SIZE    (10)
/**
* \brief Defines GSM Network IP array size \a .
  \param None.
  \returns None \a .
*/
#define GPRS_NETWORKIP_ARRAY_SIZE   (35)
/**
* \brief Defines GSM Total Commands \a .
  \param None.
  \returns None \a .
*/
#define GSM_TOTAL_COMMANDS		    (30)
/**
* \brief Defines GSM Response code size \a .
  \param None.
  \returns None \a .
*/
#define GSM_HTTP_RESPONSE_CODE_SIZE (8)
/**
* \brief Defines HTTP max attempts \a .
  \param None.
  \returns None \a .
*/
#define MAX_HTTP_ATTEMPTS			(5)		/* Max Http retry for illegal response code */
/**
* \brief Defines GSM Init character \a .
  \param None.
  \returns None \a .
*/
#define GSM_ARRAY_INIT_CHAR  		(0x00)
/**
* \brief Defines Unknown data literal \a .
  \param None.
  \returns None \a .
*/
#define DATA_UNKNOWN				'?'
/**
* \brief Defines SMS array length \a .
  \param None.
  \returns None \a .
*/
#define SMS_MAX_MSG_LENGTH			(800)
/**
* \brief Defines Mobile Number array \a .
  \param None.
  \returns None \a .
*/
#define SMS_MOB_NO_LENGTH			(13)
/**
* \brief Defines SMS end character \a .
  \param None.
  \returns None \a .
*/
#define GSM_END_OF_SMS_MARKER       (0x1A)
/**
* \brief Defines Kloudq Signature \a .
  \param None.
  \returns None \a .
*/
#define KLOUDQ_SIGNATURE     		"Tor by KloudQ : \r\n"
/**
* \brief Defines System info array size \a .
  \param None.
  \returns None \a .
*/
#define SYSTEM_INFO_MAX_SIZE        (500)
/**
* \brief Defines Init character  \a .
  \param None.
  \returns None \a .
*/
#define INIT_CHAR					(0x00)
/**
* \brief Defines CRLF  \a .
  \param None.
  \returns None \a .
*/
#define CRLF						"\r\n"
/**
* \brief Defines Max array size for remote config \a .
  \param None.
  \returns None \a .
*/
#define MAX_RMT_CONFIG_SIZE_BYTES	(5000)

/******************************************************************************
* Module Preprocessor Macros
*******************************************************************************/

/******************************************************************************
* Module Typedefs
*******************************************************************************/
/**
* This enumeration is a list of GSM Tasks
*/
typedef enum
{
	enmGSMTASK_RESET,			/* Task : Reset Module*/
	enmGSMTASK_INITMODULE,		/* Task : Init Module */
	enmGSMTASK_UPDATELOCATION,	/* Task : Update GPS Location */
	enmGSMTASK_UPLOADDATA,		/* Task : GPRS Data Upload */
	enmGSMTASK_READSMS,			/* Task : Poll for new SMS */
	enmGSMTASK_SENDSMS,			/* Task : Send SMS */
	enmGSMTASK_ISALIVE,			/* Task : HeartBeat*/
	enmGSMTASK_GETDATA,			/* Task : Fetch Data from Server */
	enmGSMTASK_DOWNLOADFOTAFILE, /* Task : Fetch FOTA File from Server */
	enmGSMTASK_IDLE				/* Task : Idle  */
}enmGSMTask;
/**
* This enumeration is a list of GSM States
*/
typedef enum
{
	/* Module Initialisation Commands */
	enmGSMSTATE_AT,
	enmGSMSTATE_ATE0,
	enmGSMSTATE_CTZU,
	enmGSMSTATE_GETTIMESTAMP,
	/* Explicit APN selection */
	enmGSMSTATE_SETAPN,
	enmGSMSTATE_ATCNMP,
	enmGSMSTATE_ATCPIN,
	enmGSMSTATE_ATCSMINS,
	enmGSMSTATE_ATCSQ,
	enmGSMSTATE_CREG,
	enmGSMSTATE_CMEE,
	enmGSMSTATE_CGACT,
	enmGSMSTATE_CSCS,			/* Character Set Configuration */
	enmGSMSTATE_GPSPOWERON,
	enmGSMSTATE_AGPSURL,
	enmGSMSTATE_GPSAUTOEN,
	enmGSMSTATE_GPSINFOCFG,

	/* HTTP Sequence */
	enmGSMSTATE_HTTPTERM,
	enmGSMSTATE_HTTPINIT,
	enmGSMSTATE_HTTPPARAURL,
	enmGSMSTATE_ATHTTPDATACOMMAND,
	enmGSMSTATE_SENDDATA,
	enmGSMSTATE_HTTPACTION,


	/* GPS Commands */
	enmGSMSTATE_GPSINFO,

	/* Send SMS */
	enmGSMSTATE_CMGF,			 /* SMS Text Mode */
	enmGSMSTATE_SMSCMGS,
	enmGSMSTATE_SENDSMS,
	enmGSMSTATE_SMSEOM,

	/* Read SMS */
	enmGSMSTATE_READMODE,
	enmGSMSTATE_READSMS,
	enmGSMSTATE_DELETESMS,

	/*Fota*/
	enmGSMSTATE_HTTPTERMCONFIG,
	enmGSMSTATE_HTTPTINITCONFIG,
	enmGSMSTATE_SETFOTAURL,
	enmGSMSTATE_FOTAHTTPACTION,
	enmGSMSTATE_READFILE,
}enmGSMState;

/**
* This enumeration is a list of GPS States
*/
typedef enum
{
	enmGPSSTATE_DEVPOWER = 0,
	enmGPSSTATE_AT,					/* Task : HeartBeat*/
	enmGPSSTATE_ATE0,				/* Task : Module Initialise*/
	enmGPSSTATE_POWER,
	enmGPSSTATE_CGNSSEQ,
	enmGPSSTATE_CGNSINF,
	enmGPSSTATE_CHECKRESPONSE
}enmGPSState;
/**
* This enumeration is a list of GSM Tasks states
*/
typedef enum
{
	enmGSM_SENDCMD,
	enmGSM_CHKRESPONSE
}enmGSMTaskState;
/**
* This enumeration is a list of GSM module power state
*/
typedef enum
{
	enmGSM_PWRNOTSTARTED,
	enmGSM_PWRSTARTED,
	enmGSM_PWRCOMPLETED

}enmGSMPowerState;
/**
* This enumeration is a list of GSM Command State
*/
typedef enum
{
	enmGSM_CMDNOTSTARTED,
	enmGSM_CMDSEND,
	enmGSM_CMDINPROCESS,
	enmGSM_CMDSUCCESS,
	enmGSM_CMDRESPONSEERROR,
	enmGSM_CMDTIMEOUT,
}enmGSMCMDState;
/**
* This Structure contains SMS details
*/
typedef struct
{
	char agsmSMSRecipient[SMS_MOB_NO_LENGTH];								/* 10 or 13 Digit Mobile Number */
	char agsmSMSMessageBody[SMS_MAX_MSG_LENGTH];							/* Stores Message to send */
	uint8_t u8NewMessage;													/* Message State 0 : New 1 : Old */
}strctSMS;
/**
* This Structure contains GSM details
*/
typedef struct
{
	char agpsLocationData[GPS_LOCATION_ARRAY_SIZE];         				/* Stores GPS Data */
	char agsmSignalStrength[GSM_SIGSTRGTH_ARRAY_SIZE];						/* Stores Signal Strength */
	char agsmNetworkIP[GPRS_NETWORKIP_ARRAY_SIZE];							/* Stores Network IP after connecting to gprs n/w*/
	char agsmCommandResponse[GSM_TOTAL_COMMANDS][GSM_TOTAL_COMMANDS];		/* Stores Command Response Bug : 18-01-2019 Timer does not work when GSM_RESPONSE_ARRAY_SIZE is changed*/
	char agsmHTTPRequestStatus[GSM_HTTP_RESPONSE_CODE_SIZE]	;				/* Stores HTTP Response code */
	char u32GSMHttpResponseCode[3];
	volatile char as8GSM_Response_Buff[GSM_RESPONSE_ARRAY_SIZE];			/* Stores Response Received from module */

	uint8_t u8gsmSIMReadyStatus;											/* Tracks Sim Ready Status */
	uint8_t u8gsmRegistrationStatus;										/* Tracks Sim Registration */
	uint8_t u8gsmRetryCount;   												/* Defines Max retry attempts for each command */
	uint8_t u8isConnected;													/* Registered to network */
	uint8_t u8HTTPInitStatus;												/* HTTP Status */
	uint8_t u8AttemptFota;
	uint8_t u8IncrementGsmState;
	uint8_t u8IllegalHttpResponseCounter;

	uint16_t u8LastHttpResponseCode;

	volatile uint32_t u32GSMTimer;											/* GSM Delay */
	uint32_t u32ONPayloadUploadFreq;										/* Data upload freq : System ON*/
	uint32_t u32OFFPayloadUploadFreq;										/* Data upload freq : System OFF*/
	uint32_t u32GSMHeartbeatTimer;
	uint32_t gu32RemoteConfigSizeinBytes;

	volatile uint32_t u32GSMResponseTimer;											/* GSM Response Delay */
	volatile uint32_t u8GSM_Response_Character_Counter;								/* Tracks received characters */
	volatile enmGSMTaskState enmGSMCommandResponseState;								/* GSM State response vs command*/
	volatile enmGSMCMDState enmGSMCommandState;										/* Command Tracker */
	volatile enmGSMTask enmcurrentTask;												/* Tracks On Going Task*/
	enmGSMState enmGSMCommand;												/* Tracks GSM Command Number */
	enmGSMPowerState enmGSMPwrState;										/*Tracks PWRKEY Operations*/
	strctSMS strSystemSMS;
}strctGSM;
/**
* This Structure contains GSM State Table details
*/
typedef struct
{
	char * atCommand;														/* AT Command to Send */
	const char * atCommandResponse;											/* Expected Command Response*/
	uint32_t msTimeOut;														/* Response Time out in ms*/
	strctGSM * GSMInstance;
}strctGSMStateTable;

/******************************************************************************
* Function Prototypes
*******************************************************************************/

#ifdef __cplusplus
extern "C"{
#endif

void initGSMSIM868(void);
void operateGSMSIM868();
void sendGSMCommand();
void updateHttpDataLength();
void sendSystemConfigurationSMS(void);
void updatePhoneNumber(void);
void syncrtcwithNetworkTime(void);
void updateDeviceToken(void);
void GSM_CharReception_Callback(void);
uint32_t updateHTTPReadLength(uint32_t ConfigFileSizeBytes);
void initHTTPURLforRemoteConfig(void );
void restoreHTTPURLforData(void);
void updateNetworkAPN(void);

#ifdef __cplusplus
} // extern "C"
#endif

#endif /* GSMSIM7600_H_ */

//***************************************** End of File ********************************************************
