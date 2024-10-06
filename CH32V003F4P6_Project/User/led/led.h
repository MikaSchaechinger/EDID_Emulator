#ifndef LED_H
#define LED_H

#include "ch32v00x_gpio.h"

class LED {
private:
    GPIO_TypeDef* port;  // LED GPIO-Port
    uint16_t pin;        // LED GPIO-Pin
    __IO uint32_t* brightnessRegister;  // Register for the brightness of the LED

public:

    // Constructor
    LED(GPIO_TypeDef* port, uint16_t pin, __IO uint32_t* brightnessRegister);

    // Set the brightness of the LED (0-100%)
    void setBrightness(uint8_t brightness);

    uint8_t getBrightness();

    // 
    void startBlink(uint16_t interval, uint8_t brightness=100);

    void startBlink(uint16_t interval, uint8_t amount, uint8_t brightness=100);

    // Turn the LED off. Stops the current blink/function process
    void off();

    // Control the LED with a brightness function for a duration
    void setBrightnessFunction(uint8_t (*brightnessFunction)(uint32_t timeMS), uint32_t durationMS);

    // Update the LED state. Must be called periodically (about every 10ms) by an interrupt or main loop
    void update(uint32_t sysTimeMS);



};




#endif // LED_H
