#include "communication_manager.h"



Communication::CommunicationManager::CommunicationManager(PWM_LED* statusLED, StandardLED* errorLED, EDIDManager* edidManager) {
    this->state = Communication_State::OFF;
    this->bufferIndex = 0;
    this->statusLED = statusLED;
    this->errorLED = errorLED;
    this->edidManager = edidManager;
}


void Communication::CommunicationManager::init() {
    this->state = Communication_State::IDLE;
    this->bufferIndex = 0;
}


void Communication::CommunicationManager::newByte(uint8_t byte) {
    if (this->state == Communication_State::OFF) return;

    if (this->bufferIndex >= USART_BUFFER_SIZE) {
        this->state = Communication_State::OVERFLOW;
        return;
    }

    this->buffer[this->bufferIndex] = byte;
    this->bufferIndex++;

    if (byte == '\n') {
        this->state = Communication_State::RECEIVED;
    }
}


void Communication::CommunicationManager::update() {
    if (this->state == Communication_State::RECEIVED) {
        this->processMessage();
    }
}


void Communication::CommunicationManager::enable() {
    if (this->state == Communication_State::OFF) {
        this->state = Communication_State::IDLE;
    }
}


void Communication::CommunicationManager::disable() {
    this->state = Communication_State::OFF;
}


Communication::Communication_State Communication::CommunicationManager::getState() {
    return this->state;
}




/****************** Functions called in update **************************/


bool Communication::CommunicationManager::handleError() {
    // Check for timeout
    if (this->state == Communication_State::TIMEOUT) {
        USART_SendString("A timeout occurred. Received a message without an EOF \\n");
        this->finishedProcessing();
        return true;
    }
    if (this->state == Communication_State::OVERFLOW) {
        USART_SendString("The buffer is full. Rx Buffer length: ", false);
        char numBuffer[12];
        uintToString(numBuffer, USART_BUFFER_SIZE);
        USART_SendString(numBuffer);
        USART_SendEOF();
        this->finishedProcessing();
        return true;
    }
    if (this->state == Communication_State::ERROR) {
        USART_SendString("An unknown error occurred");
        this->finishedProcessing();
        return true;
    }
    return false;
}


void Communication::CommunicationManager::finishedProcessing() {
    this->state = Communication_State::IDLE;
    this->bufferIndex = 0;
}


void Communication::CommunicationManager::processMessage() {
    // Return if an error occurred and process it.
    if (this->handleError())    
        return;
    // Only process the message if it was received completely
    if (this->state != Communication_State::RECEIVED) return;

    // Process the message
    uint16_t messageLength = this->bufferIndex - 1;  // -1 because of the EOF
    if (messageLength == 0) {
        // Empty message
        USART_SendString("Empty message received (only EOF)");
        this->finishedProcessing();
        return;
    }
    switch (this->buffer[0]) {
        case 't':
            if (bufferIsCommand("test"))
                this->testCommand();
            else{}

            break;
        default:
            this->unknownCommand();
            break;
    }
        


    this->finishedProcessing();
}


bool Communication::CommunicationManager::bufferIsCommand(const char* command) {
    return this->startsWithCommand((char*)this->buffer, command);
}


bool Communication::CommunicationManager::startsWithCommand(const char* str, const char* command) {
        uint16_t index = 0;
    while (str[index] != '\0' && command[index] != '\0') {
        if (str[index] != command[index]) {
            return false;
        }
        index++;
    }
    // Buffer can now end with \0 or continue with a space
    if (str[index] == '\n' || str[index] == ' ' || str[index] == '\0') {
        return true;
    }
    return false;   // str continues with a different character, so it is not this command
}


bool Communication::CommunicationManager::getInteger(const char* str, int32_t* value) {
    uint16_t index = 0;
    int32_t result = 0;
    bool negative = false;
    if (str[index] == '-') {
        negative = true;
        index++;
    }
    else if (str[index] == '+') {
        index++;
    }
    while ((str[index] >= '0' && str[index] <= '9')) {
        result = result * 10 + (str[index] - '0');
        index++;
    }   
    // Check next Character
    if(str[index] != '\0' && str[index] != '\n' && str[index] != ' ')
        return false;   // Next Character is not a Number
    if (negative) {
        result = -result;
    }
    *value = result;
    return true;
}


void Communication::CommunicationManager::testCommand() {
    char* buf = (char*)this->buffer + 4;
    if (buf[0] == '\n') {
        USART_SendString("Test command received without parameters");
        return;
    }
    buf++;  // Skip the space
    if (startsWithCommand(buf, "led1")){
        buf += 4;   // Skip the led1
        if (buf[0] != ' ') { USART_SendString("Missing Integer [0:100] after led1"); return; }
        buf++;  // Skip the space
        int32_t value = 1;
        if (!getInteger(buf, &value)) {
            USART_SendString("Invalid Integer [0:100] after led1");
            return;
        }
        if (value < 0 || value > 100) {
            USART_SendString("Invalid Integer [0:100] after led1. Value was: ", false);
            USART_SendInteger(value);
            return;
        }
        this->statusLED->setBrightness(value);
        USART_SendString("LED1 set to: ", false);
        USART_SendInteger(value);
    }
    else if (startsWithCommand(buf, "led2")){
        buf += 4;   // Skip the led2
        if (buf[0] != ' ') { USART_SendString("Missing Integer [0:1] after led2"); return; }
        buf++;  // Skip the space
        int32_t value = 0;
        if (!getInteger(buf, &value)) {
            USART_SendString("Invalid Integer [0:1] after led2");
            return;
        }
        if (value < 0 || value > 1) {
            USART_SendString("Invalid Integer [0:1] after led2. Value was: ", false);
            USART_SendInteger(value);
            return;
        }
        if (value == 0)     this->errorLED->off();
        else                this->errorLED->on();
        USART_SendString("LED2 set to: ", false);
        USART_SendInteger(value);
    }
    else{
        USART_SendString("Unknown test command received");
    }

}


void Communication::CommunicationManager::unknownCommand() {
    USART_SendString("Unknown command received!");
}
    
