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

#define SEND_NUMBER_VALUE       15
//#define MCU_SLEEP_TIME          300
//#define MCU_SLEEP_TIME          3600    // 1 Hour
#define MCU_SLEEP_TIME          108000    // 30 Hour

/* Lora IRWML202G047 module sleep instruction (Lora IRWML202G047 模组休眠指令) */
//uint8_t Lora_EnterInto_LowPower[14] = {0x24,0x50,0x41,0x52,0x41,0x00,0x07,0x0B,0x00,0x00,0x00,0x1E,0x83,0x42};     // Sleep 30 S
//uint8_t Lora_EnterInto_LowPower[14] = {0x24,0x50,0x41,0x52,0x41,0x00,0x07,0x0B,0x00,0x00,0x0E,0x10,0x27,0xC7};     // Sleep 3600 S (One Hour)
//uint8_t Lora_EnterInto_LowPower[14] = {0x24,0x50,0x41,0x52,0x41,0x00,0x07,0x0B,0x00,0x00,0x00,0x3C,0x9A,0xC2};   // Sleep 60 S (one Minute)
//uint8_t Lora_EnterInto_LowPower[14] = {0x24,0x50,0x41,0x52,0x41,0x00,0x07,0x0B,0x00,0x00,0x07,0x08,0x7D,0xC1};     // Sleep 1800 S (Half Hour)
uint8_t Lora_EnterInto_LowPower[14] = {0x24,0x50,0x41,0x52,0x41,0x00,0x07,0x0B,0x00,0x01,0x51,0x80,0x7B,0xAF};     // Sleep 86400 S (24 Hour)
//uint8_t Lora_EnterInto_LowPower[14] = {0x24,0x50,0x41,0x52,0x41,0x00,0x07,0x0B,0x00,0x01,0x19,0x40,0x2B,0x99};     // Sleep 72000 S (20 Hour)


extern uint8_t Send_data_node[255];
extern uint8_t Outputbuffer_len;

unsigned char pBufOut[256];

uint8_t Send_Number = 0;
uint32_t lightning_stroke_number = 0;
uint8_t pack_buffer[28] = {0};

void SystemClock_Config(void);


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

/**
  * @brief  Check Data Pack
  * @retval None
  */
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


/**
  * @brief  LED Flashing Indicator
  * @retval None
  */
void Blink(int blinkCount,int period)
{
    for(int i=0;i<blinkCount;i++)
    {
        HAL_GPIO_TogglePin(LED_GPIO_Port, LED_Pin);
        HAL_Delay(period);
    }
    HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_SET);
}

/**
  * @brief  EXTI line detection callbacks.
  * @param  GPIO_Pin Specifies the pins connected to the EXTI line.
  * @retval None
  */
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
    switch (GPIO_Pin)
    {
        case GPIO_PIN_4:
          break;
        case GPIO_PIN_7:
          break;
        case GPIO_PIN_11:
            lightning_stroke_number++;

          break;
        default:
          break;
    }
}


/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
    HAL_Init();

    SystemClock_Config();       // System Clock Initializes the Function

    HAL_Delay(5000);            // Power on and wait 5 seconds (上电等待5S, 再初始化相关函数)

    /* Initialize all configured peripherals */
    MX_GPIO_Init();             // GPIO Initializes the Function
    MX_DMA_Init();              // DMA Initializes the Function
    MX_I2C1_Init();             // DMA Initializes the Function
    MX_USART1_UART_Init();      // USART1 Initializes the Function
    MX_RTC_Init();              // RTC Initializes the Function
    MX_USART2_UART_Init();      // USART2 Initializes the Function
    MX_TIM2_Init();             // TIM2 Initializes the Function


    //LowPowerConfig();

    while (1)
    {

        /* The Serial Port USART2 DMA receives data (USART2 DMA 接收数据) */
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

                    if ((str[0] == 0xA5) && (str[27] == 0x5A))
                    {
                        for (uint8_t i = 0;i < 28;i++)
                        {
                           pack_buffer[i] = str[i];
                        }
                        pack_buffer[15] = (uint8_t)lightning_stroke_number;
                        pack_buffer[16] = (uint8_t)(lightning_stroke_number >> 8);
                        pack_buffer[17] = (uint8_t)(lightning_stroke_number >> 16);
                        pack_buffer[18] = (uint8_t)(lightning_stroke_number >> 24);

                        uint16_t checksum = 0;
                        for(uint8_t i=0; i<25; i++)
                        {
                            uint8_t Data = pack_buffer[i];
                            checksum += Data;
                        }
                        pack_buffer[25] = (uint8_t)checksum;
                        pack_buffer[26] = (uint8_t)(checksum >> 8);

                    }
                    /* Receive data parsing (解析接收过来的数据) */
                    //unsigned char ret = transfer(sensorId, str, Usart2_Receive_t.Receive_Len, pBufOut, &bufOutLen);
                    unsigned char ret = transfer(pack_buffer, Usart2_Receive_t.Receive_Len, pBufOut, &bufOutLen);

                    if(ret==0)
                    {
                        memset(Send_data_node, 0, sizeof(Send_data_node));
                        memcpy(Send_data_node, pBufOut, bufOutLen);
                        Outputbuffer_len = (uint8_t)bufOutLen; 
                        memset(Usart2_Receive_t.Receive_pData, 0, sizeof(Usart2_Receive_t.Receive_pData));

                        HAL_UART_Receive_DMA(&huart2, Usart2_Receive_t.Receive_pData, RECEIVE_LEN);
                    }
                }
                str++;
                Usart2_Receive_t.Receive_Len--;
            }
        }

        if (Outputbuffer_len > 0)
        {
            if ((HAL_GPIO_ReadPin(BUSY_GPIO_Port, BUSY_Pin) == 1) && (HAL_GPIO_ReadPin(ACOUT_GPIO_Port, ACOUT_Pin) == 1))
            {
                Blink(10,100);  // The LED flashes to indicate that data is about to be sent (LED闪烁指示将要发送数据给LORA IRWML202G047模组)

                /* The Serial Port USART1 DMA sends data (USART1 DMA 发送数据给LORA IRWML202G047模组) */
                HAL_UART_Transmit_DMA(&huart1, Send_data_node, Outputbuffer_len);       // USART1 DMA Send Data
                //HAL_UART_Transmit(&huart1, Send_data_node, Outputbuffer_len, 1000);     // USART1 Send Data

                Outputbuffer_len = 0;
                Send_Number++;

                /* Send data count (USART1 DMA 发送数据给LORA IRWML202G047模组进行计数,计
                满10次发送指令给LORA IRWML202G047模组进入低功耗模式,同时MCU进入STOP低功耗模式) */
                if (Send_Number == SEND_NUMBER_VALUE)
                {
                    Send_Number = 0;
                    HAL_Delay(500);
                    /* Cut off the power supply of the sensor base plate (切断传感器底板电源) */
                    HAL_GPIO_WritePin(PWR_EN_GPIO_Port, PWR_EN_Pin, GPIO_PIN_RESET);
                    /* The Lora IRWML202G047 module has entered low power mode (发送休眠休眠指令给LORA IRWML202G047模组进入低功耗模式) */
                    HAL_UART_Transmit_DMA(&huart1, Lora_EnterInto_LowPower, 14);
                    HAL_Delay(500);
                    lightning_stroke_number = 0;
                    /* The MCU enters STOP low power mode (MCU进入STOP低功耗模式) */
                    IntoStopModeWithRTC(MCU_SLEEP_TIME);
                }
            }

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
