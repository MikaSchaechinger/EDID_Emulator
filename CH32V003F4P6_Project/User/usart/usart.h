#ifndef USART_H
#define USART_H


#include "ch32v00x_usart.h"
#include "pins.h"
#include "string/mystring.h"
#include <string>

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
void USART_SendString(const std::string, bool newLine = true);

// Sending a string over the USART
void USART_SendString(const char* str, bool newLine = true);

// Sending bytes as hex over the USART
void USART_SendHex(const uint8_t* byte);

void USART_SendInteger(int32_t value, bool newLine = true);

void USART_SendInteger(uint32_t value, bool newLine = true);

// Send EOF
void USART_SendEOF();




#endif // USART_H
