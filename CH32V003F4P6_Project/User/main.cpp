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

#include <led/standard_led.h>
#include <led/pwm_led.h>
#include "debug.h"
#include <button/button.h>
#include <usart/usart.h>



/* Global define */
#define ever ;;

/* Global Variable */
volatile uint32_t msTicks = 0;

Button button(GPIOD, BUTTON_PIN, true);
StandardLED errorLED(GPIOD, ERROR_LED, true);
PWM_LED statusLED(GPIOD, STATUS_LED, &TIM1->CH4CVR);






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
    button.update(msTicks);
    statusLED.update(msTicks);
    errorLED.update(msTicks);
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

    // Initialice USART1 with the RX interrupt
    USARTx_CFG();

    button.init();
    errorLED.init();
    pwmPD4init();
    statusLED.init();

    setup_interrupt();  // Setup the interrupt for the button, LEDs and SysTick Timer

    ButtonState buttonState;

    for(ever)
    {   
        buttonState = button.getButtonState();
        if (buttonState != ButtonState::NO_PRESS){
            // User Pressed the Button
            if (buttonState == ButtonState::SHORT_PRESS)
                errorLED.blink(1000, 1, 50);
            else if (buttonState == ButtonState::DOUBLE_PRESS)
                errorLED.blink(500, 5, 50);
            else if (buttonState == ButtonState::LONG_PRESS)
                errorLED.blink(2000, 2, 50);

        }


    }
}
