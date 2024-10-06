#include "led.h"


LED::LED(GPIO_TypeDef* port, uint16_t pin, __IO uint32_t* brightnessRegister) {
    this->port = port;
    this->pin = pin;
    this->brightnessRegister = brightnessRegister;
}



void LED::setBrightness(uint8_t brightness) {
    // limit brightness to 0-100%	
    if (brightness > 100) 
        brightness = 100;
    
    *(this->brightnessRegister) = brightness;
}


uint8_t LED::getBrightness() {
    return *(this->brightnessRegister);
}
