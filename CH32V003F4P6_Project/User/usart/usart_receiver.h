#ifndef USART_RECEIVER_H
#define USART_RECEIVER_H

#include <usart/usart.h>



#define USART_BUFFER_SIZE 512



enum USARTReceiverState {
    OFF,        // Receiver is off
    IDLE,       // Ready to receive a new message
    RECEIVING,  // Receiving a message
    RECEIVED,   // Message finished receiving. EOF received.    (\n)
    TIMEOUT,    // Timeout while receiving a message. Message is incomplete, eof not received
    OVERFLOW,   // Buffer overflow
    ERROR       // Unknown error
};



class USARTReceiver {

private:
    USARTReceiverState state;

    uint8_t buffer[USART_BUFFER_SIZE];
    uint16_t bufferIndex;

public:

    USARTReceiver();

    void init();

    // Function processes the received byte
    void newByte(uint8_t byte);

    // enable the receiver -> state: IDLE
    void enable();

    // disable the receiver -> state: OFF
    void disable();

    // get the current state of the receiver
    USARTReceiverState getState();

    // Processing is done and state can be set to IDLE again
    void finishtProcessing();

    // Process the received message, when the state is RECEIVED
    void processMessage();
};




#endif // USART_RECEIVER_H