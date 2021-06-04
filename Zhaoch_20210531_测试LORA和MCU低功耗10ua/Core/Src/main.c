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
* This software component is licensed by ST under BSD 3-Clause license,
* the "License"; You may not use this file except in compliance with the
* License. You may obtain a copy of the License at:
*                        opensource.org/licenses/BSD-3-Clause
*
******************************************************************************
*/
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "dma.h"
#include "i2c.h"
#include "rtc.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "power.h"
#include "ml202.h"
#include "string.h"
#include "transfer.h"
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
uint8_t Lora_EnterInto_power[14] = {0x24,0x50,0x41,0x52,0x41,0x00,0x07,0x0B,0x00,0x00,0x00,0x32,0x5E,0x43};
//static uint8_t status_cmd[10] = {0x24,0x50,0x41,0x52,0x41,0x00,0x03,0x09,0xF2,0x71};
uint8_t Send_Cmd_AT[2] = {0x41,0x54};
uint8_t Send_Cmd_AT_RUM[6] = {0x41,0x54,0x2B,0x52,0x55,0x4D};
uint8_t Send_Cmd_AT_RSYS[7] = {0x41,0x54,0x2B,0x52,0x53,0x59,0x53};
uint8_t Send_Cmd_AT_SMdata[9] = {0x41,0x54,0x2B,0x53,0x4D,0x64,0x61,0x74,0x61};

uint8_t Send_data_buffer[21] = {0xA9,0x2E,0x00,0x0C,0x00,0x01,0x20,0x20,0x00,0xC8,0x7A,0xB2,0x41,0x0C,0x00,0x04,0x6C,0x63,0x40,0x66,0x67};
uint8_t crc_buffer[19] = {0xA5,0xA1,0x01,0x8F,0xC2,0x75,0x3F,0x9E,0xEF,0xA7,0x3D,0x00,0x00,0xAA,0x42,0x0C,0xAE,0x01,0x02};
extern uint8_t Send_data_node[255];
extern uint8_t Outputbuffer_len;

uint8_t mlRecvBuff[11]="";
int timeout=0;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
void Blink(int blinkCount,int period){
  for(int i=0;i<blinkCount;i++){
    HAL_GPIO_TogglePin(LED_GPIO_Port, LED_Pin);
    HAL_Delay(period);
  } 
  HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_SET);
}

/*
int checkPack(uint8_t *buff, uint16_t len)
{
  
    if(len<74){
      return 1;
    }
    
    if(buff[0]==0xA5 && buff[73]==0x5A){
        return 0;
    }
    return 1;
}*/

int checkPack(uint8_t *buff, uint16_t len)
{
  
    if(len<28){
      return 1;
    }
    
    if(buff[0]==0xA5 && buff[27]==0x5A){
        return 0;
    }
    return 1;
}


unsigned char pBufOut[256];
/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
    HAL_Init();

    SystemClock_Config();


    /* Initialize all configured peripherals */
    MX_GPIO_Init();
    MX_DMA_Init();
    MX_I2C1_Init();
    MX_USART1_UART_Init();
    MX_RTC_Init();
    MX_USART2_UART_Init();
    MX_TIM2_Init();

    HAL_Delay(3000);
    //LowPowerConfig();

    while (1)
    {


      HAL_UART_Receive_DMA(&huart2, Usart2_Receive_t.Receive_pData, RECEIVE_LEN);
      if (Usart2_Receive_t.Receive_Flag)
      {
          uint8_t *str = Usart2_Receive_t.Receive_pData;

          while (Usart2_Receive_t.Receive_Len>27)
          {
              int res = checkPack(str, Usart2_Receive_t.Receive_Len);
              if(res == 0)
              {
                 //unsigned char sensorId[6] = {0xA9,0x2E,0x10,0x22,0xFF,0x02};
                 unsigned char bufOutLen = 0;
                 //unsigned char ret = transfer(sensorId, str, Usart2_Receive_t.Receive_Len, pBufOut, &bufOutLen);
                 unsigned char ret = transfer(str, Usart2_Receive_t.Receive_Len, pBufOut, &bufOutLen);
                 if(ret==0)
                 {
                     memset(Send_data_node, 0, sizeof(Send_data_node));
                     memcpy(Send_data_node, pBufOut, bufOutLen);
                     Outputbuffer_len = (uint8_t)bufOutLen; 
                     HAL_UART_Receive_DMA(&huart2, Usart2_Receive_t.Receive_pData, RECEIVE_LEN);
                 }
              }
              str++;
              Usart2_Receive_t.Receive_Len--;
          }
      }



      if (Outputbuffer_len > 0)
      {
          HAL_UART_Transmit_DMA(&huart1, Send_data_node, Outputbuffer_len);
          HAL_Delay(100);
          HAL_UART_Transmit_DMA(&huart1, Lora_EnterInto_power, 14);
          HAL_Delay(100);

          Outputbuffer_len = 0;
          Blink(10,100);
          IntoStopModeWithRTC(60);
      }

    }
}


/*while (Usart2_Receive_t.Receive_Len>73)
{
  int res = checkPack(str, Usart2_Receive_t.Receive_Len);
  if(res == 0)
  {
     unsigned char sensorId[6] = {0xA9,0x2E,0x10,0x22,0xFF,0x02};
     unsigned char bufOutLen = 0;
     unsigned char ret = transfer(sensorId, str, Usart2_Receive_t.Receive_Len, pBufOut, &bufOutLen);
     if(ret==0)
     {
         memset(Send_data_node, 0, sizeof(Send_data_node));
         memcpy(Send_data_node, pBufOut, bufOutLen);
         Outputbuffer_len = (uint8_t)bufOutLen; 
         HAL_UART_Receive_DMA(&huart2, Usart2_Receive_t.Receive_pData, RECEIVE_LEN);
     }
    
  }
  str++;
  Usart2_Receive_t.Receive_Len--;
}*/

/**
* @brief System Clock Configuration
* @retval None
*/
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

  /** Configure the main internal regulator output voltage
  */
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);
  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI|RCC_OSCILLATORTYPE_LSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.LSIState = RCC_LSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLLMUL_3;
  RCC_OscInitStruct.PLL.PLLDIV = RCC_PLLDIV_3;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV2;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    Error_Handler();
  }
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_USART1|RCC_PERIPHCLK_USART2
                              |RCC_PERIPHCLK_I2C1|RCC_PERIPHCLK_RTC;
  PeriphClkInit.Usart1ClockSelection = RCC_USART1CLKSOURCE_PCLK2;
  PeriphClkInit.Usart2ClockSelection = RCC_USART2CLKSOURCE_PCLK1;
  PeriphClkInit.I2c1ClockSelection = RCC_I2C1CLKSOURCE_PCLK1;
  PeriphClkInit.RTCClockSelection = RCC_RTCCLKSOURCE_LSI;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }
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
  __disable_irq();
  while (1)
  {
  }
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
  ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
