#ifndef PWM_LED_H
#define PWM_LED_H

#include "ch32v00x_gpio.h"
#include "led_state.h"

class PWM_LED {
private:
    GPIO_TypeDef* port;  // PWM_LED GPIO-Port
    uint16_t pin;        // PWM_LED GPIO-Pin
    __IO uint32_t* brightnessRegister;  // Register for the brightness of the PWM_LED
    LEDState state;      // Current state of the PWM_LED

public:

    // Constructor
    PWM_LED(GPIO_TypeDef* port, uint16_t pin, __IO uint32_t* brightnessRegister);

    void init() {};

    // Set the brightness of the PWM_LED (0-100%)
    void setBrightness(uint8_t brightness);

    uint8_t getBrightness();

    // 
    void startBlink(uint16_t interval, uint8_t brightness=100);

    void startBlink(uint16_t interval, uint8_t amount, uint8_t brightness=100);

    // Turn the PWM_LED off. Stops the current blink/function process
    void off();

    // Control the PWM_LED with a brightness function for a duration
    void setBrightnessFunction(uint8_t (*brightnessFunction)(uint32_t timeMS), uint32_t durationMS);

    // Update the PWM_LED state. Must be called periodically (about every 10ms) by an interrupt or main loop
    void update(uint32_t sysTimeMS);



};




#endif // PWM_LED_H
