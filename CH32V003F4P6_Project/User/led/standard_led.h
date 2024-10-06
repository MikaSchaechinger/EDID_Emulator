#ifndef STANDARD_LED_H
#define STANDARD_LED_H

#include "ch32v00x_gpio.h"
#include "led_state.h"

/*
    LED Class, wich can only be on/off/blink
*/
class StandardLED {
private:
    GPIO_TypeDef* port;  // LED GPIO-Port
    uint16_t pin;        // LED GPIO-Pin
    LEDState state;      // Current state of the LED

    uint32_t blinkOnDuration;   // Duration of the LED on time in ms
    uint32_t blinkOffDuration;  // Duration of the LED off time in ms
    uint32_t blinkToggleTime;   // Time when the LED started blinking

    uint8_t blinkAmount;        // Amount of blinks 
    uint8_t blinkCounter;       // Counter for the amount of blinks

    bool isOn;                  // Is the LED on or off

    bool inverted;

public:


    StandardLED(GPIO_TypeDef* port, uint16_t pin, bool inverted);

    void init();

    // Turn the LED on
    void on();

    // Turn the LED off
    void off();

    // Blink the LED with a given interval
    void blink(uint16_t intervall, uint8_t dutyCycleProcent=50);

    // Blink the LED with a given interval and amount
    void blink(uint16_t interval, uint8_t amount, uint8_t dutyCycleProcent=50);

    // update the LED state, so it can blink
    void update(uint32_t sysTimeMS);


};





#endif // STANDARD_LED_H
