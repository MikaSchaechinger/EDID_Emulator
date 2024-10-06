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
void USARTx_CFG(void);


extern "C" {    void USART1_IRQHandler(void); }
void USART1_IRQHandler(void) __attribute__((interrupt("WCH-Interrupt-fast")));






#endif // USART_H
