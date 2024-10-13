#ifndef COMMUNICATION_MANAGER_H
#define COMMUNICATION_MANAGER_H


#include <usart/usart.h>
#include <string>

#include <eeprom/edid_manager.h>
#include <led/pwm_led.h>
#include <led/standard_led.h>



#define USART_BUFFER_SIZE 512
// #define DEBUG

namespace Communication{

    enum Communication_State {
        OFF,        // Receiver is off
        IDLE,       // Ready to receive a new message
        RECEIVING,  // Receiving a message
        RECEIVED,   // Message finished receiving. EOF received.    (\n)
        TIMEOUT,    // Timeout while receiving a message. Message is incomplete, eof not received
        OVERFLOW,   // Buffer overflow
        ERROR       // Unknown error
    };

    /*
    *   OFF
    *    |
    *    | enable()
    *    | 
    *   IDLE <--------------------------+ <---------+
    *    |                              |           |
    *    | receive Interrupt            |           | update()
    *    |                              |           |
    *   RECEIVING---------------------- | --> TIMEOUT/OVERFLOW
    *    |                              |
    *    | EOF received                 |
    *    |                              |
    *   RECEIVED                        |
    *    |                              |
    *    | update() (in main loop)      |
    *    |                              |
    *    +------------------------------+

    */



    class CommunicationManager {

    private:
        PWM_LED* statusLED;
        StandardLED* errorLED;
        EDIDManager* edidManager;

        Communication_State state;

        uint8_t buffer[USART_BUFFER_SIZE];
        uint16_t bufferIndex;


    /************* Functions called in update ****************************/

        // Processing is done and state can be set to IDLE again
        void finishedProcessing();

        // Handles the Timeout, Overflow and Error States. Returns true if an error occured
        bool handleError();

        // Process the received message, when the state is RECEIVED
        void processMessage();

        // 
        bool bufferIsCommand(const char* command);
        bool startsWithCommand(const char* str, const char* prefix);
        bool getInteger(const char* str, int32_t* value);

        void testCommand();

        void unknownCommand();

    public:

        CommunicationManager(PWM_LED* statusLED, StandardLED* errorLED, EDIDManager* edidManager);

        void init();

        // Function processes the received byte. Is called by the USART Interrupt
        void newByte(uint8_t byte);

        // Processes the received message. Is called by the main loop
        void update();

        // enable the receiver -> state: IDLE
        void enable();

        // disable the receiver -> state: OFF
        void disable();

        // get the current state of the receiver
        Communication_State getState();



    };

} // namespace Communication


#endif // COMMUNICATION_MANAGER_H
