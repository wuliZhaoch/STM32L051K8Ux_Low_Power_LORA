#include "ml202.h"
#include "usart.h"


//static uint8_t  ml202IntoSleep_10s[14]={0x24, 0x50, 0x41, 0x52, 0x41, 0x00, 0x07, 0x0B, 0x00, 0x00, 0x00, 0x0A, 0x8C, 0x42};
static uint8_t  ml202IntoSleep_59s[14]={0x24, 0x50, 0x41, 0x52, 0x41, 0x00, 0x07, 0x0B, 0x00, 0x00, 0x00, 0x03B, 0x58, 0x83};



int ml202Send(uint8_t *buff, uint16_t len){
  return HAL_UART_Transmit_DMA(&huart1, buff, len); 
}

int ml202Recv(uint8_t *buff, uint16_t len){
 return  HAL_UART_Receive_DMA(&huart1, buff, len);
}

void ml202StopSendAndRecv(void){
  HAL_UART_DMAStop(&huart1);
}

bool ml202SendCmd(uint8_t *cmd,uint16_t cmdLen,uint8_t *ack,uint16_t ackLen,int timeout){
  
  uint8_t recvBuff[128]="";
  ml202Send(cmd,cmdLen);
  ml202Recv(recvBuff,128);
  while(timeout>0){   
    if(strstr((char const *)recvBuff,(char const *)ack)){
      ml202StopSendAndRecv();
      return true;
    }  
    HAL_Delay(10);
    timeout -=10;
  }
  ml202StopSendAndRecv();
  return false;
}

bool ml202IntoSleepMode(uint16_t sleepTime){
  ml202SendCmd(ml202IntoSleep_59s,14,"$PARA,OK.",2,1000);
  return true;
}
