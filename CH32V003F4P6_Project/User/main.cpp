/********************************** (C) COPYRIGHT *******************************
 * File Name          : main.c
 * Author             : WCH
 * Version            : V1.0.0
 * Date               : 2022/08/08
 * Description        : Main program body.
 * Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
 * SPDX-License-Identifier: Apache-2.0
 *******************************************************************************/

/*
 *@Note
 Multiprocessor communication mode routine:
 Master:USART1_Tx(PD5)\USART1_Rx(PD6).
 This routine demonstrates that USART1 receives the data sent by CH341 and inverts
 it and sends it (baud rate 115200).

 Hardware connection:PD5 -- Rx
                     PD6 -- Tx

*/

#include "debug.h"
#include "button.h"


/* Global define */
#define ever ;;

/* Global Variable */

void GPIO_Clock_Init(void){
    RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOC|RCC_APB2Periph_GPIOD, ENABLE);
}


void setup_interrupt(){
    // Enable the interrupt
    NVIC_EnableIRQ(SysTicK_IRQn);
    SysTick->SR = 0;   
    SysTick->CMP = BUTTON_SAMPLE_TIME_MS * (SystemCoreClock / 1000);
    SysTick->CNT = 0;
    //SysTick->CTLR = 0;  // Reset System Count Control Register
    //SysTick->CTLR |= 0b0001;    // Start the system counter STK
    //SysTick->CTLR |= 0b0010;    // Enabling counter interrupts
    //SysTick->CTLR |= 0b0100;    //  1: HCLK for time base; 0: HCLK/8 for time base.
    //SysTick->CTLR |= 0b1000;    // Re-counting from 0 after counting up to the comparison value.
    SysTick->CTLR = 0x000F;

}


/*********************************************************************
 * @fn      main
 *
 * @brief   Main program.
 *
 * @return  none
 */
int main(void)
{
    GPIO_Clock_Init();
    Button button(GPIOC, BUTTON_PIN);

    for(ever)
    {

    }
}
