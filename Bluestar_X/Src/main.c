/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2021 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under Ultimate Liberty license
  * SLA0044, the "License"; You may not use this file except in compliance with
  * the License. You may obtain a copy of the License at:
  *                             www.st.com/SLA0044
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "adc.h"
#include "dma.h"
#include "i2c.h"
#include "iwdg.h"
#include "lwip.h"
#include "rtc.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "applicationdefines.h"
#include "externs.h"
#include <string.h>
#include "user_adc.h"
#include "user_rtc.h"
#include "user_eeprom.h"
#include "user_timer.h"
#include "deviceinfo.h"
#include "modbus_rs485.h"
#include "modbus_tcp.h"
#include "gsmSim7600.h"
#include "payload.h"
#include "queue.h"
#include "user_ethernet.h"
#include "remote_configuration.h"
#include "user_flash.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
uint32_t gu32OnstartEthernetStatusAlert = 0;
extern strctQUEUE gsmPayload;
extern strctGSM gsmInstance;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();


  /* USER CODE BEGIN SysInit */
  /* Check if configuration is present in the memory */
  getLastKnownConfiguration();
  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_DMA_Init();
  MX_IWDG_Init();
  MX_TIM7_Init();
  MX_UART4_Init();
  MX_RTC_Init();
  MX_USART3_UART_Init();
  MX_ADC1_Init();
  MX_LWIP_Init();
  MX_I2C1_Init();
  /* USER CODE BEGIN 2 */
  	updateDeviceSignature();
	HAL_GPIO_WritePin(MB_DE_GPIO_Port,MB_DE_Pin,GPIO_PIN_RESET);
	HAL_GPIO_WritePin(MB_RE_GPIO_Port,MB_RE_Pin,GPIO_PIN_SET);
	HAL_GPIO_WritePin(LED_1_GPIO_Port, LED_1_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(LED_2_GPIO_Port, LED_2_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(LED_3_GPIO_Port, LED_3_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GSM_PWR_KEY_GPIO_Port,GSM_PWR_KEY_Pin,GPIO_PIN_RESET);

	setupModbus();
	setupModbusRTU();

	if(gu32UploadTimeSupply == 0)
		gu32UploadTimeSupply = TWO_MIN;

	if(gu32UploadTimeBatt == 0)
		gu32UploadTimeBatt = FIVE_MIN;

	enqueue(&gsmPayload,(char *)getSystemConfig());
	HAL_GPIO_WritePin(LED_1_GPIO_Port, LED_1_Pin, GPIO_PIN_SET);	// LED OFF

	LL_USART_EnableIT_RXNE(USART3);
	LL_USART_EnableIT_RXNE(UART4);
	if(HAL_TIM_Base_Start_IT(&htim7)!= HAL_OK)
	{
		Error_Handler();
	}
	/*Added on 28-1-22 . APN from configuration*/
	updateNetworkAPN();
	gu32TempPayloadQueueEnqueue = ONE_MIN;
	memset(gau8TempMBPayloadString,0x00,sizeof(char) * gu32MBDataByteCounterLimit);
	memset(gau8TempMBRTUPayloadString,0x00,sizeof(char) * gu32MBDataByteCounterLimitRTU);

	/* Only For Testing */
//	flashWriteBootSection(enmBOOTSEGMENT_Y_JUMP);
	/* Only For Testing */
//	getNewFirmwareStorageLocation();
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
	/********************** Main Application Program ***************************/

	/***** Ethernet  *****/
	MX_LWIP_Process();
	updateEthLinkStatus();
	/*Execute Modbus TCP Poll*/
	if(gu32OperateModbusTCP == 1)
	{
		mbTCPPoll();
		updateModbusPayload();
	}
	/* RTU Poll */
	if(gu32OperateModbus485 == 1)
	{
		modbusQueryFSM();
		uploadSlaveData();
	}
//	if(gu32LinkDownTimer == 0)
//	{
//		/* If no data received on modbus tcp . Take reset */
//		if(gu32MBClientConnectedFlag == 0)
//			while(1);
//		else
//			gu32LinkDownTimer = FIFTEEN_MIN;
//	}

	/***** Internal ADC  *****/
	operateadc();

	/***** Payload Generation  *****/
	if(gu32TempPayloadQueueEnqueue == 0)
	{
		enqueue(&gsmPayload,(char *)getSystemDataString());
		if(gu32InputSupplySwitchedAlert == TRUE)
			gu32TempPayloadQueueEnqueue = TEN_SEC;//gu32UploadTimeBatt;
		else
			gu32TempPayloadQueueEnqueue = gu32UploadTimeSupply;

		if(gu32TempPayloadQueueEnqueue < THIRTY_SEC)
			gu32TempPayloadQueueEnqueue = THIRTY_SEC;
	}

	/***** GSM Module Operation  *****/
	if(gsmInstance.u32GSMTimer > FIVE_MIN)
		gsmInstance.u32GSMTimer = FOUR_SEC;

	if(gu32GSMHangTimer == 0)
		initGSMSIM868();

	if(gu32GSMRestartTimer == 0)
		initGSMSIM868();

	operateGSMSIM868();

	/***** RTC Sync  *****/
	if(gu32RTCUpdateTimer == 0)
	{
//		if(u32GPSTimeSyncFlag == TRUE)
//		{
//			/*Updated GPS Time Stamp Available */
//			syncRTCGPS();
//			u32GPSTimeSyncFlag = FALSE;
//		}
//		else
//		{
//			updateElapsedTime(gu32TimeSyncVariable);
//		}
		//backupRTCCalender();
		gu32RTCUpdateTimer = FIVE_SEC;
	}

	/***** LED Indication  *****/
	if(gu32LedTimer == 0)
	{
		HAL_GPIO_TogglePin(LED_2_GPIO_Port, LED_2_Pin);
		HAL_GPIO_TogglePin(HEARTBEAT_GPIO_Port,HEARTBEAT_Pin);
		if(gu32InputSupplySwitchedAlert == FALSE)
			gu32LedTimer = ONE_SEC;
		else
			gu32LedTimer = FIVE_SEC;
	}
	if(gu32SystemResetTimer == 0 )
	{
		while(1); /* Forced Reset after 30 Mins or unable to upload data */
	}

	/***** Remote Configuration Updates *****/
	verifyRemoteConfiguration();
	parseRemoteconfig();

	/***** Internal Watchdog *****/
	/* Use of Internal WatchDog is mandatory irrespective of logic being implemented
	* Can be explicitly disabled whenever required . */
	/* Refresh IWDT */
	getRTCCalender();
	HAL_IWDG_Refresh(&hiwdg);
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
  RCC_PeriphCLKInitTypeDef PeriphClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);
  /** Initializes the CPU, AHB and APB busses clocks
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_LSI|RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.LSIState = RCC_LSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 15;
  RCC_OscInitStruct.PLL.PLLN = 216;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 4;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Activate the Over-Drive mode
  */
  if (HAL_PWREx_EnableOverDrive() != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB busses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK)
  {
    Error_Handler();
  }
  PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_RTC;
  PeriphClkInitStruct.RTCClockSelection = RCC_RTCCLKSOURCE_LSI;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Enables the Clock Security System
  */
  HAL_RCC_EnableCSS();
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */

  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
