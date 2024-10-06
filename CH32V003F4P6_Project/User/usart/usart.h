#ifndef USART_H
#define USART_H


#include "ch32v00x_usart.h"
#include "pins.h"

/*********************************************************************
 * @fn      USARTx_CFG
 *
 * @brief   Initializes the USART2 & USART3 peripheral.
 *
 * @return  none
 */
void USARTx_CFG(void)
{
    GPIO_InitTypeDef  GPIO_InitStructure = {0};
    USART_InitTypeDef USART_InitStructure = {0};
    NVIC_InitTypeDef  NVIC_InitStructure = {0};

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD | RCC_APB2Periph_USART1, ENABLE);

    // TX
    GPIO_InitStructure.GPIO_Pin = RS232_TX;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOD, &GPIO_InitStructure);

    // RX
    GPIO_InitStructure.GPIO_Pin = RS232_RX;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOD, &GPIO_InitStructure);

    USART_InitStructure.USART_BaudRate = 9600;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;

    USART_Init(USART1, &USART_InitStructure);
    USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);  // Enable the USART1 receive interrupt

    // Configure the NVIC Preemption Priority Bits
    NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);;

    USART_Cmd(USART1, ENABLE);  // Enable the USART1 peripheral
}


extern "C" {    void USART1_IRQHandler(void); }
void USART1_IRQHandler(void) __attribute__((interrupt("WCH-Interrupt-fast")));



void USART1_IRQHandler(void)
{
    // Interrupt was triggered by RXNE -> Receive Data Register Not Empty

    // Check if the interrupt was triggered by RXNE
    if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)
    {
        // Read the received byte from the USART
        uint8_t receivedByte = USART_ReceiveData(USART1);

        // Echo the received byte back
        USART_SendData(USART1, receivedByte);
    }
}


#endif // USART_H
