#include <usart/usart.h>





void USARTx_CFG(void) {
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


void USART_SendString(const std::string str, bool newLine) {
    USART_SendString(str.c_str(), newLine);
}   


void USART_SendString(const char* str, bool newLine) {
    while (*str != '\0')
    {
        USART_SendData(USART1, *str);
        str++;
        while(USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);
    }

    if (newLine) {
        USART_SendData(USART1, '\n');
        while(USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);
    }
}


void USART_SendHex(uint8_t* byte, uint16_t length) {
    if (length == 0) return;
    char buffer1[2];
    char buffer2[2];
    char* sendBuffer = buffer1;
    char* workBuffer = buffer2;

    // Calculate the first byte
    byteToHexString(sendBuffer, *byte);

    for (uint16_t i = 0; i < length; i++) {
        USART_SendData(USART1, sendBuffer[0]);
        while(USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);

        USART_SendData(USART1, sendBuffer[1]);
        // calculate the next byte
        byteToHexString(workBuffer, *(++byte));
        // Now wait for the second byte halfe to be sent
        while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);
        // Swap the buffers
        char* temp = sendBuffer;
        sendBuffer = workBuffer;
        workBuffer = temp;
    }
}


void USART_SendInteger(int32_t value, bool newLine) {
    char buffer[12];
    intToString(buffer, value);
    USART_SendString(buffer, newLine);
}

void USART_SendInteger(uint32_t value, bool newLine) {
    char buffer[12];
    uintToString(buffer, value);
    USART_SendString(buffer, newLine);
}


void USART_SendEOF() {
    USART_SendData(USART1, '\n');
    while(USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);
}
