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
#include "button/button.h"
#include "led/led.h"


/* Global define */
#define ever ;;

/* Global Variable */
volatile uint32_t msTicks = 0;

void GPIO_Clock_Init(void){
    RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOC|RCC_APB2Periph_GPIOD, ENABLE);
}


void setup_interrupt(){
    // Enable the interrupt
    NVIC_EnableIRQ(SysTicK_IRQn);
    SysTick->SR = 0;   
    SysTick->CMP = (SystemCoreClock / 2000);  // SystemCoreClock was 2x slower then SysTick Clock
    SysTick->CNT = 0;
    SysTick->CTLR = 0;  // Reset System Count Control Register
    SysTick->CTLR |= 0b0001;    // Start the system counter STK
    SysTick->CTLR |= 0b0010;    // Enabling counter interrupts
    SysTick->CTLR |= 0b0100;    //  1: HCLK for time base; 0: HCLK/8 for time base.
    SysTick->CTLR |= 0b1000;    // Re-counting from 0 after counting up to the comparison value.
    //SysTick->CTLR = 0x000F;

}

// essential for the timer running with cpps
extern "C" {    void SysTick_Handler(void); }
void SysTick_Handler(void) __attribute__((interrupt("WCH-Interrupt-fast")));  //("WCH-Interrupt-fast")
void SysTick_Handler(void)
{
    SysTick->SR = 0;    // Reset the Status Registers
    msTicks++;
}


void setupSysTickTimer(){
    // Starts the Systick Timer without interrupts. Timer runs repeatedly over the full range.
    SysTick->CMP = 0xFFFFFF;    // Set the comparison value to the maximum value
    SysTick->CNT = 0;           // Reset the counter
    SysTick->CTLR = 0x00000005; // Enable the timer
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
    setup_interrupt();
    Button button(GPIOD, BUTTON_PIN, true);
    // //LED statusLED(GPIOD, STATUS_LED, TIM1, 1);
    // LED errorLED(GPIOD, ERROR_LED, TIM1, 2);
    button.init();
    // //statusLED.init();
    // errorLED.init();

    // For testing, turn on the error LED
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Pin = ERROR_LED;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOD, &GPIO_InitStructure);
    GPIO_ResetBits(GPIOD, ERROR_LED);

    GPIO_InitStructure.GPIO_Pin = STATUS_LED;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOD, &GPIO_InitStructure);
    GPIO_ResetBits(GPIOD, STATUS_LED);


    for(ever)
    {   

        // if (button.readButton()){
        //     GPIO_SetBits(GPIOD, STATUS_LED);
        // }
        // else{
        //     GPIO_ResetBits(GPIOD, STATUS_LED);
        // }


//         if(msTicks % 1000 > 500){
//             GPIO_SetBits(GPIOD, ERROR_LED);
//         }
//         else {
//             GPIO_ResetBits(GPIOD, ERROR_LED);
//        }

         button.update(msTicks);
         ButtonState buttonState = button.getButtonState();

//         if (buttonState == NO_PRESS){
//             GPIO_SetBits(GPIOD, STATUS_LED);
//             GPIO_SetBits(GPIOD, ERROR_LED);
//         }

         if (buttonState == SHORT_PRESS){
             GPIO_ResetBits(GPIOD, STATUS_LED);
             GPIO_SetBits(GPIOD, ERROR_LED);

         }
         else if (buttonState == LONG_PRESS){

             GPIO_SetBits(GPIOD, STATUS_LED);
             GPIO_SetBits(GPIOD, ERROR_LED);
         }
         else if (buttonState == DOUBLE_PRESS){
             GPIO_SetBits(GPIOD, STATUS_LED);
             GPIO_ResetBits(GPIOD, ERROR_LED);
         }

        //  if(SysTick->CNT%100000 > 100000){
        //      errorLED.setBrightness(100);
        //  }
        //  else{
        //      errorLED.setBrightness(0);
        //  }

    }
}
