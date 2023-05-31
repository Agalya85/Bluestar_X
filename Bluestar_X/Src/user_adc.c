/*******************************************************************************
* Title                 :   Internal ADC Interface
* Filename              :   user_adc.c
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

/** @file  user_adc.c
 *  @brief Internal ADC Implementation
 *
 */
/******************************************************************************
* Includes
*******************************************************************************/
#include "main.h"
#include "adc.h"
#include "user_adc.h"
#include "user_timer.h"
#include "applicationdefines.h"
#include "externs.h"

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
/*DMA Interrupt Stores data in this array */
uint32_t gau32AnalogPeripheralData[1] = {0};
uint32_t gau32BatteryPeripheralData[2] = {0};
/*User modified ADC Peripheral Data */
uint32_t gau32AnalogPeripheralProcessedData[5] = {0};
uint32_t gau32BatteryProcessedData[2] = {0};
volatile uint32_t g32AdcConversionStatus = FALSE;
volatile uint32_t g32AdcConversionStatusBatt = FALSE;
float gfInputSupplyVoltage = 0;
float gfBatteryVoltage = 0;

/* Tracks Power supply to battery switching */
uint32_t gu32InputSupplySwitchedAlert = FALSE;

enmAdcState enmAdcSMCurrentState = enmADC_IDLE;

/******************************************************************************
* Function Prototypes
*******************************************************************************/

/******************************************************************************
* Function Definitions
*******************************************************************************/
/******************************************************************************
* Function : operateadc()
*//**
* \b Description:
*
* This function is used to operate internal ADC with DMA .
* State machine implementation for ADC
*
*
* PRE-CONDITION: Enable ADC and DMA interrupts
*
* POST-CONDITION: None
*
* @return 		None.
*
* \b Example Example:
* @code
*
* 	operateadc()
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
void operateadc(void)
{
	if(gu32ADCPollTimer)
		return;

	switch(enmAdcSMCurrentState)
	{
		case enmADC_IDLE:
			/* Start ADC 1 and ADC 3 */
			if((HAL_ADC_Start_DMA(&hadc1, (uint32_t*)gau32BatteryPeripheralData, 2) != HAL_OK))
				enmAdcSMCurrentState = enmADC_ERROR;
			else
			{
				HAL_ADC_Start_IT(&hadc1);
				enmAdcSMCurrentState = enmADC_GETDATA;
				gu32ADCOperationTimer = ONE_MIN;
			}
			break;

		case enmADC_GETDATA:
			if((g32AdcConversionStatusBatt == TRUE) && (gu32ADCOperationTimer != 0))
			{
				/* ADC conversion completed */
				gu32ADCOperationTimer = 0;
				gau32BatteryProcessedData[ADC_VIN + 1] = gau32BatteryPeripheralData[ADC_VIN +1];
				gau32BatteryProcessedData[ADC_VIN] = gau32BatteryPeripheralData[ADC_VIN];
				/* Add other channels when configured */
				enmAdcSMCurrentState = enmADC_PROCESSDATA;
				g32AdcConversionStatusBatt = FALSE;
				g32AdcConversionStatus = FALSE;

			}
			else if((!g32AdcConversionStatus) | (!g32AdcConversionStatusBatt) | (gu32ADCOperationTimer == 0))
			{
				/*Error in ADC Conversion */
				enmAdcSMCurrentState = enmADC_IDLE;
			}
			break;

		case enmADC_ERROR:
			/* Process ADC Errors */
			gu32ADCPollTimer = TEN_SEC;
			enmAdcSMCurrentState = enmADC_IDLE;
			break;

		case enmADC_PROCESSDATA:
			/* Process Data as per connected slave / divider / sensor Output(s)*/
			calculateInputVoltage();
			gu32ADCPollTimer = TEN_SEC;
			enmAdcSMCurrentState = enmADC_IDLE;
			break;

		default:
			/* Undefined State . Restart the State Machine */
			gu32ADCPollTimer = TEN_SEC;
			enmAdcSMCurrentState = enmADC_IDLE;
			break;
	}
}

/******************************************************************************
* Function : calculateInputVoltage()
*//**
* \b Description:
*
* This function is used to Calculate Input Supply Voltage from adc data .
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
* 	calculateInputVoltage()
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
void calculateInputVoltage(void)
{
	/* Calculate Input Votage from adc data */
	gfInputSupplyVoltage = (float) ((MAX_INPUT_VTG * gau32BatteryProcessedData[ADC_VIN + 1])/ADC_RESOLUTION);
	gfBatteryVoltage = ((((3.3 * ( 2 * gau32BatteryProcessedData[ADC_VIN])) / 4095) * 2 ) / 0.6 ) - 0.2;
	/* Add diode drop for actual voltage */
	gfInputSupplyVoltage += INPUT_DIODE_DROP;
	/* Check if system is powered from 8.4 V dc battery pack */
	if(gfInputSupplyVoltage <= VIN_BATTERY_VTG)
	{
		gu32InputSupplySwitchedAlert = TRUE;
	}
	else
	{
		gu32InputSupplySwitchedAlert = FALSE;
	}
}
/*
 *
 *
 * */
void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc)
{
	/*Sequence conversion complete . Read Processed Value */
	if(hadc->Instance == ADC1)
	{
		if(g32AdcConversionStatusBatt == FALSE)
			g32AdcConversionStatusBatt = TRUE;

	}
	else if(hadc->Instance == ADC3)
	{
		if(g32AdcConversionStatus == FALSE)
			g32AdcConversionStatus = TRUE;
	}
	else
	{
		/* Unknown Interrupt */
	}
}

//***************************************** End of File ********************************************************
