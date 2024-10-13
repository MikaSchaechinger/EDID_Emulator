

#include "button/button.h"



Button::Button(GPIO_TypeDef* port, uint16_t pin, bool inverted)
{
    this->port = port;
    this->pin = pin;
    this->inverted = inverted;

    this->doubleClickEnabled = true;
    this->lastBounceState = false;

    // Button debouncing
    this->lastBounceState = false;
    this->debouncedState = false;
    this->lastBounceTime = 0;

    // Button press tracking
    this->buttonState = ButtonState::NO_PRESS;
    this->internalState = ButtonInternalState::IDLE;

    // Timing for press types
    this->lastEventTime = 0;
}

void Button::init()
{
    // GPIO initialization
    GPIO_InitTypeDef GPIO_InitStructure;

    GPIO_InitStructure.GPIO_Pin = this->pin;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(this->port, &GPIO_InitStructure);

}


bool Button::readButton()
{
    return this->inverted xor GPIO_ReadInputDataBit(this->port, this->pin);
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
    }
    else{
        this->debouncedState = reading;
    }
    

    uint32_t stateUpTime = sysTimeMS - this->lastEventTime;

    switch(this->internalState){
        case IDLE:
            if(this->debouncedState){
                this->internalState = FIRST_PRESS;
                this->lastEventTime = sysTimeMS;
            }
            break;
        case FIRST_PRESS:
            // Long press detection
            if (stateUpTime > LONG_PRESS_THRESHOLD_MS){
                this->internalState = SECOND_PRESS;
                this->buttonState = LONG_PRESS;
            }
            else if(!this->debouncedState){
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
            else if(this->debouncedState){
                this->internalState = SECOND_PRESS;
                this->buttonState = DOUBLE_PRESS;
            }
            break;
        case SECOND_PRESS:
            if(!this->debouncedState){
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
    
