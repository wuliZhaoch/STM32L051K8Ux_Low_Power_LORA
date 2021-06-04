#include "power.h"
#include "main.h"
#include "rtc.h"
#include "tim.h"
#include "usart.h"
#include "i2c.h"
#include "gpio.h"
#include "dma.h"
#include "ml202.h"
void WakeFromStopMode(void);

void LowPowerConfig(void){
  /* Enable Ultra low power mode */
  HAL_PWREx_EnableUltraLowPower();
  /* Enable the fast wake up from Ultra low power mode */
  HAL_PWREx_EnableFastWakeUp();
  /* Select HSI as system clock source after Wake Up from Stop mode */
  __HAL_RCC_WAKEUPSTOP_CLK_CONFIG(RCC_STOP_WAKEUPCLOCK_HSI);
}

void GpioLowPowerConfig(void){
   GPIO_InitTypeDef GPIO_InitStruct = {0};
  
  /* Enable GPIOs clock */
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOC_CLK_ENABLE();


  /* Configure all GPIO port pins in Analog Input mode (floating input trigger OFF) */
/*Configure GPIO pins : PC14 PC15 */
  GPIO_InitStruct.Pin = GPIO_PIN_14|GPIO_PIN_15;
  GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pins : PA0 PA1 PA2 PA3
                           PA4 PA5 PA6 PA7
                           PA8 PA9 PA10 PA11
                           PA12 PA15 */
  /*
  GPIO_InitStruct.Pin = GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3
                          |GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7
                          |GPIO_PIN_8|GPIO_PIN_9|GPIO_PIN_10|GPIO_PIN_11
                          |GPIO_PIN_12|GPIO_PIN_15;
  GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
*/
  
    GPIO_InitStruct.Pin = GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3
                          |GPIO_PIN_4|GPIO_PIN_5
                          |GPIO_PIN_8|GPIO_PIN_9|GPIO_PIN_10|GPIO_PIN_11|GPIO_PIN_13|GPIO_PIN_14
                          |GPIO_PIN_12|GPIO_PIN_15;
  GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : PB0 PB1 PB2 PB3
                           PB4 PB5 PB6 PB7 */
  GPIO_InitStruct.Pin = GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3
                          |GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7|GPIO_PIN_8;
  GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /* Disable GPIOs clock */
  __HAL_RCC_GPIOA_CLK_DISABLE();
  __HAL_RCC_GPIOB_CLK_DISABLE();
  __HAL_RCC_GPIOC_CLK_DISABLE();
}

void TimerLowPowerConfig(void){
  HAL_TIM_Base_MspDeInit(&htim2);
}

void I2cLowPowerConfig(void){
  HAL_I2C_MspDeInit(&hi2c1);
}

void UartLowPowerConfig(void){
  HAL_UART_MspDeInit(&huart2);
  HAL_UART_MspDeInit(&huart1);
}

void PreIntoStopMode(void){
  UartLowPowerConfig();
  //I2cLowPowerConfig(); 
  //TimerLowPowerConfig();
  GpioLowPowerConfig();
  HAL_Delay(10);
}

void IntoStopModeWithRTC(uint32_t sec){
   
   //while(ml202IntoSleepMode(sec)!=true);
   // HAL_Delay(1000);
   PreIntoStopMode();
  
   HAL_RTCEx_DeactivateWakeUpTimer(&hrtc);
   HAL_RTCEx_SetWakeUpTimer_IT(&hrtc, sec, RTC_WAKEUPCLOCK_CK_SPRE_16BITS);	
   HAL_PWR_EnterSTOPMode(PWR_LOWPOWERREGULATOR_ON, PWR_STOPENTRY_WFI);
   
   WakeFromStopMode();
}


void WakeFromStopMode(void){
    /* Enable Power Control clock */
  __HAL_RCC_PWR_CLK_ENABLE();
  extern void SystemClock_Config(void);
  SystemClock_Config();
  MX_GPIO_Init();
  MX_DMA_Init();
 // MX_I2C1_Init();
  HAL_UART_DeInit(&huart2);
  HAL_UART_DeInit(&huart1);
  MX_USART1_UART_Init();
  MX_RTC_Init();
  MX_USART2_UART_Init();
  MX_TIM2_Init();
  uint8_t sed_data = 0xFF;
  HAL_UART_Transmit_DMA(&huart1, &sed_data, 1);
}