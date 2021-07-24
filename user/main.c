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


void EXTI15_10_IRQHandler(void) {
  LED_On();
  /* Make sure that interrupt flag is set */
  if (EXTI_GetITStatus(EXTI_Line13) != RESET) {
    NRF24L01_IRQ_Handler(RX_BUF);
    /* Clear interrupt flag */
    EXTI_ClearITPendingBit(EXTI_Line13);
  }
  LED_Off();
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

