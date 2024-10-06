

#include "standard_led.h"

StandardLED::StandardLED(GPIO_TypeDef* port, uint16_t pin, bool inverted) {
    this->port = port;
    this->pin = pin;
    this->state = LEDState::OFF;
    this->isOn = false;
    this->inverted = inverted;

    this->blinkOnDuration = 500;
    this->blinkOffDuration = 500;
    this->blinkToggleTime = 0;
    this->blinkAmount = 0;
    this->blinkCounter = 0;
    
}


void StandardLED::init() {
    // Configure the GPIO Pin
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Pin = pin;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(port, &GPIO_InitStructure);

    // Turn the LED off
    this->off();
}


void StandardLED::on() {
    this->state = LEDState::ON;
    this->isOn = true;
    if (this->inverted) 
        GPIO_ResetBits(this->port, this->pin);
    else 
        GPIO_SetBits(this->port, this->pin);
}


void StandardLED::off() {
    this->state = LEDState::OFF;
    this->isOn = false;
    if (this->inverted) 
        GPIO_SetBits(this->port, this->pin);
    else 
        GPIO_ResetBits(this->port, this->pin);
}




void StandardLED::blink(uint16_t interval, uint8_t dutyCycleProcent) {
    this->blinkOnDuration = interval * dutyCycleProcent / 100;
    this->blinkOffDuration = interval - this->blinkOnDuration;
    this->blinkAmount = 0;
    this->blinkToggleTime = 0;
    this->state = LEDState::BLINKING;
}


void StandardLED::blink(uint16_t interval, uint8_t amount, uint8_t dutyCycleProcent) {
    this->blinkOnDuration = interval * dutyCycleProcent / 100;
    this->blinkOffDuration = interval - this->blinkOnDuration;
    this->blinkAmount = amount;
    this->blinkCounter = 0;
    this->blinkToggleTime = 0;
    this->state = LEDState::BLINKING;
}


void StandardLED::update(uint32_t sysTimeMS) {
    if (this->state != LEDState::BLINKING) {
        return;
    }

    if (isOn){
        if (sysTimeMS - this->blinkToggleTime > this->blinkOnDuration) {
            if (this->inverted) 
                GPIO_SetBits(this->port, this->pin);
            else 
                GPIO_ResetBits(this->port, this->pin);
            this->isOn = false;
            this->blinkToggleTime = sysTimeMS;
        }
    }
    else {
        if (sysTimeMS - this->blinkToggleTime > this->blinkOffDuration) {
            if (this->inverted) 
                GPIO_ResetBits(this->port, this->pin);
            else 
                GPIO_SetBits(this->port, this->pin);
            this->isOn = true;
            this->blinkToggleTime = sysTimeMS;
            if (this->blinkAmount > 0) {
                if (this->blinkCounter >= this->blinkAmount) {
                    this->state = LEDState::OFF;
                    this->isOn = false;
                    if (this->inverted) 
                        GPIO_SetBits(this->port, this->pin);
                    else 
                        GPIO_ResetBits(this->port, this->pin);
                }
                this->blinkCounter++;
            }
        }
    }
}

