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


void pwmPD4init(){
    // Remap TIM1_RM to 11  (Datasheet page 55, table Table 7-8 TIM1 alternate function remapping)
    // PD4 gets TIM1_CH4
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
    AFIO->PCFR1 &= ~(AFIO_PCFR1_TIM1_REMAP);
    AFIO->PCFR1 |= AFIO_PCFR1_TIM1_REMAP_1;     // set first bit to 1
    AFIO->PCFR1 |= AFIO_PCFR1_TIM1_REMAP_0;     // set second bit to 1
    // AFIO->PCFR1 |= AFIO_PCFR1_TIM1_REMAP_FULLREMAP;

    GPIO_InitTypeDef GPIO_InitStructure = {0};
    TIM_OCInitTypeDef TIM_OCInitStructure = {0};
    TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure = {0};

    RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOD | RCC_APB2Periph_TIM1, ENABLE );

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;     // Alternate Function Push-Pull
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOD, &GPIO_InitStructure);

    TIM_TimeBaseInitStructure.TIM_Period = 1000;  // Set the period for PWM. Count value from 0 to 100
    TIM_TimeBaseInitStructure.TIM_Prescaler = 480 - 1;  // Set the prescaler (depending on system clock)
    TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInit(TIM1, &TIM_TimeBaseInitStructure);

    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
    TIM_OCInitStructure.TIM_Pulse = 1;  // Set the initial duty cycle to 50%
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low;   //

    // Select Channel 4
    TIM_OC4Init(TIM1, &TIM_OCInitStructure);

    // Enable PWM
    TIM_CtrlPWMOutputs(TIM1, ENABLE);
    TIM_OC4PreloadConfig(TIM1, TIM_OCPreload_Enable);
    TIM_ARRPreloadConfig(TIM1, ENABLE);
    TIM_Cmd(TIM1, ENABLE);
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
    button.init();

    pwmPD4init();
    LED statusLED(GPIOD, STATUS_LED, &TIM1->CH4CVR);

    // LED errorLED(GPIOD, ERROR_LED, TIM1, 2);
    // errorLED.init();

    // For testing, turn on the error LED
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Pin = ERROR_LED;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOD, &GPIO_InitStructure);
    GPIO_ResetBits(GPIOD, ERROR_LED);

    // GPIO_InitStructure.GPIO_Pin = STATUS_LED;
    // GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    // GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    // GPIO_Init(GPIOD, &GPIO_InitStructure);
    // GPIO_ResetBits(GPIOD, STATUS_LED);


    for(ever)
    {   

        // if (button.readButton()){
        //     GPIO_SetBits(GPIOD, STATUS_LED);
        // }
        // else{
        //     GPIO_ResetBits(GPIOD, STATUS_LED);
        // }

        uint8_t brightness = (msTicks %10000)/10;
        statusLED.setBrightness(brightness);

         if(msTicks % 1000 > 500){
             GPIO_SetBits(GPIOD, ERROR_LED);
         }
         else {
             GPIO_ResetBits(GPIOD, ERROR_LED);
        }

//         button.update(msTicks);
//         ButtonState buttonState = button.getButtonState();
//
//
//         if (buttonState == SHORT_PRESS){
//             statusLED.setBrightness(100);
//             GPIO_SetBits(GPIOD, ERROR_LED);
//
//         }
//         else if (buttonState == LONG_PRESS){
//
//             statusLED.setBrightness(0);
//             GPIO_SetBits(GPIOD, ERROR_LED);
//         }
//         else if (buttonState == DOUBLE_PRESS){
//             statusLED.setBrightness(50);
//             GPIO_ResetBits(GPIOD, ERROR_LED);
//         }
    }
}
