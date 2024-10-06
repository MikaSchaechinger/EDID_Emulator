#include <led/pwm_led.h>


PWM_LED::PWM_LED(GPIO_TypeDef* port, uint16_t pin, __IO uint32_t* brightnessRegister) {
    this->port = port;
    this->pin = pin;
    this->brightnessRegister = brightnessRegister;
}



void PWM_LED::setBrightness(uint8_t brightness) {
    // limit brightness to 0-100%	
    if (brightness > 100) 
        brightness = 100;
    
    *(this->brightnessRegister) = brightness;
}


uint8_t PWM_LED::getBrightness() {
    return *(this->brightnessRegister);
}


void PWM_LED::update(uint32_t sysTimeMS) {
    // TODO
}