/**
  ******************************************************************************
  * @file    usart.h
  * @brief   This file contains all the function prototypes for
  *          the usart.c file
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
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __USART_H__
#define __USART_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* USER CODE BEGIN Includes */
#include "stdio.h"
#include "string.h"
/* USER CODE END Includes */

extern UART_HandleTypeDef huart1;
extern UART_HandleTypeDef huart2;

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

void MX_USART1_UART_Init(void);
void MX_USART2_UART_Init(void);

/* USER CODE BEGIN Prototypes */
#define RECEIVE_LEN 255
typedef struct
{
    uint8_t  Receive_Flag:1;                //IDLE receive flag
    uint16_t Receive_Len;                   //receive length
    uint8_t  Receive_pData[RECEIVE_LEN];    //DMA receive buffer
}USART_RECEIVE_TYPE;

extern USART_RECEIVE_TYPE Usart1_Receive_t;
extern USART_RECEIVE_TYPE Usart2_Receive_t;
extern uint8_t Enter_stop_flag;

typedef struct {
    uint16_t Len;
    uint8_t curbuf[255];
}Uart_ReceiveData_t;

extern Uart_ReceiveData_t RevStruct;

/* USER CODE END Prototypes */

#ifdef __cplusplus
}
#endif

#endif /* __USART_H__ */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
