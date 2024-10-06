#include <usart/usart_receiver.h>



USARTReceiver::USARTReceiver() {
    this->state = USARTReceiverState::OFF;
    this->bufferIndex = 0;
}


void USARTReceiver::init() {
    this->state = USARTReceiverState::IDLE;
    this->bufferIndex = 0;
}


void USARTReceiver::newByte(uint8_t byte) {
    if (this->state == USARTReceiverState::OFF) return;

    if (this->bufferIndex >= USART_BUFFER_SIZE) {
        this->state = USARTReceiverState::OVERFLOW;
        return;
    }

    this->buffer[this->bufferIndex] = byte;
    this->bufferIndex++;

    if (byte == '\n') {
        this->state = USARTReceiverState::RECEIVED;
    }
}


void USARTReceiver::enable() {
    if (this->state == USARTReceiverState::OFF) {
        this->state = USARTReceiverState::IDLE;
    }
}


void USARTReceiver::disable() {
    this->state = USARTReceiverState::OFF;
}


USARTReceiverState USARTReceiver::getState() {
    return this->state;
}


void USARTReceiver::finishtProcessing() {
    if (this->state == USARTReceiverState::RECEIVED) {
        this->state = USARTReceiverState::IDLE;
    }
}


void USARTReceiver::processMessage() {
    // Only process the message if it was received completely
    if (this->state != USARTReceiverState::RECEIVED) return;

    // Process the message

    // Reset the buffer index
}



