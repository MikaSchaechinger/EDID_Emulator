#include "button.h"



Button::Button(GPIO_TypeDef* port, uint16_t pin)
{
    this->port = port;
    this->pin = pin;
}

void Button::init()
{
    // Button debouncing
    bool lastBounceState = false;
    this->lastBounceTime = 0;

    // Button press tracking
    this->buttonState = NO_PRESS;
    this->internalState = IDLE;

    // Timing for press types
    this->lastEventTime = 0;
}


bool Button::readButton()
{
    return GPIO_ReadInputDataBit(this->port, this->pin);
}

/***********************************************
* @fn       Button::update
*
* @brief    Update the button state. Must be called periodically (about every 10ms) by an interrupt or main loop
*
* @param    sysTimeMS - The current system time in milliseconds
*/
void Button::update(uint32_t sysTimeMS){

    // Ensure we don't debounce too quickly
    if (sysTimeMS - this->lastBounceTime < DEBOUNCE_DELAY_MS){
        return;
    }

    // Debounce the button
    bool reading = this->readButton();
    if (reading != this->lastBounceState){
        this->lastBounceTime = sysTimeMS;
        this->lastBounceState = reading;
        return;
    }
    

    uint32_t stateUpTime = sysTimeMS - this->lastEventTime;

    switch(this->internalState){
        case IDLE:
            if(reading){
                this->internalState = FIRST_PRESS;
                this->lastEventTime = sysTimeMS;
            }
            break;
        case FIRST_PRESS:
            // Long press detection
            if (stateUpTime > LONG_PRESS_THRESHOLD_MS){
                this->internalState = IDLE;
                this->buttonState = LONG_PRESS;
            }
            else if(!reading){
                if (this->doubleClickEnabled){
                    this->internalState = FIRST_RELEASE;
                    this->lastEventTime = sysTimeMS;
                }
                else{
                    this->internalState = IDLE;
                    this->buttonState = SHORT_PRESS;
                }
            }
            break;
        case FIRST_RELEASE:
            if(stateUpTime > DOUBLE_PRESS_THRESHOLD_MS){
                this->internalState = IDLE;
                this->buttonState = SHORT_PRESS;
            }
            else if(reading){
                this->internalState = SECOND_PRESS;
                this->buttonState = DOUBLE_PRESS;
            }
            break;
        case SECOND_PRESS:
            if(!reading){
                this->internalState = IDLE;
            }
            break;
    }
}


ButtonState Button::getButtonState(bool reset){
    ButtonState state = this->buttonState;
    if(reset){
        this->buttonState = NO_PRESS;
    }
    return state;
}


void Button::setDoubleClickEnabled(bool enabled){
    this->doubleClickEnabled = enabled;
}
    