#ifndef USART_H
#define USART_H


#include "ch32v00x_usart.h"
#include "pins.h"
#include "string/mystring.h"

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


// Sending a string over the USART
void USART_SendString(const char* str, bool newLine = true);

// Sending bytes as hex over the USART
void USART_SendHex(const uint8_t* byte);

// Sending 




#endif // USART_H
