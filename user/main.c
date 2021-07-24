#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <stdlib.h>

#include "led.h"
#include "timer.h"
#include "uart.h"
#include "nrf24l01.h"

u8 RX_ADDRESS[NRF24L01_ADDR_WIDTH] = {0x32,0x4E,0x6F,0x64,0x65};
u8 TX_ADDRESS[NRF24L01_ADDR_WIDTH] = {0x11,0x22,0x33,0x44,0x55};

/*-----------------------------------------------------*/
/* Here control sending:1 or receiving:0 mode */
u8 Mode = 0;
/*-----------------------------------------------------*/
extern u8 RX_BUF[];
extern u8 TX_BUF[];


void EXTILine13_Config(void)
{
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
  /* Enable SYSCFG clock */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);

  GPIO_InitTypeDef   GPIO_InitStructure;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_13;
  GPIO_Init(GPIOB, &GPIO_InitStructure);
  
  /* Connect EXTI Line13 to PG13 pin */
  SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOB, EXTI_PinSource13);
  
  EXTI_InitTypeDef   EXTI_InitStructure;
  EXTI_InitStructure.EXTI_Line    = EXTI_Line13;
  EXTI_InitStructure.EXTI_Mode    = EXTI_Mode_Interrupt;
  EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
  EXTI_InitStructure.EXTI_LineCmd = ENABLE;
  EXTI_Init(&EXTI_InitStructure);
  
  NVIC_InitTypeDef   NVIC_InitStructure;
  NVIC_InitStructure.NVIC_IRQChannel = EXTI15_10_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x01; 
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x01;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
}

void EXTI15_10_IRQHandler(void) {
  printf("EXTI15_10_IRQHandler\r\n");
  /* Make sure that interrupt flag is set */
  if (EXTI_GetITStatus(EXTI_Line13) != RESET) {
    NRF24L01_RxPacket_NoBlock(RX_BUF);
    /* Clear interrupt flag */
    EXTI_ClearITPendingBit(EXTI_Line13);
  }
}

int main(void)
{
  Systick_Init();
  USART1_Init();
  NRF24L01_Init();
  NRF24L01_DumpConfig();
  while(NRF24L01_Check() != 0) {
    printf("nRF24L01 check failed\r\n");
    Systick_Delay_ms(2000);
  }
  printf("nRF24L01 check succeeded\r\n");

  if(Mode == 1) {
    printf("nRF24L01 in SEND mode\r\n");
    NRF24L01_TX_Mode(RX_ADDRESS, TX_ADDRESS);
  } else if(Mode == 0) {
    printf("nRF24L01 in RECEIVE mode\r\n");
    NRF24L01_RX_Mode(RX_ADDRESS, TX_ADDRESS);
  }

  LED_Init();
  EXTILine13_Config();

  while(1) {
    /*LED_On();
    NRF24L01_DumpConfig();
    if(Mode == 1) {
      u8 tmp[] = {0x1f, 
        0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18,
        0x21, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x28,
        0x31, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x38,
        0x41, 0x12, 0x13, 0x14, 0x15, 0x16, 0x47, 0x48
      };
      NRF24L01_TxPacket(tmp, 32);

    } else if(Mode == 0) {
      // NRF24L01_RxPacket(RX_BUF);
    }
    LED_Off();
    Systick_Delay_ms(3000);*/
  }
}

