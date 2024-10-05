#ifndef BUTTON_H
#define BUTTON_H

/*
    Button class handles the user button, including debounce and detection of short, long, and double presses.
*/

#include "ch32v00x_gpio.h"
#include "pins.h"



#define BUTTON_SAMPLE_TIME_MS 10


#define DEBOUNCE_DELAY_MS 50
#define LONG_PRESS_THRESHOLD_MS 1000    // 1 second for long press
#define DOUBLE_PRESS_THRESHOLD_MS 500   // Second click must occur within 500ms of the first click 




enum ButtonState
{
    NO_PRESS,
    SHORT_PRESS,
    LONG_PRESS,
    DOUBLE_PRESS
};

enum ButtonInternalState
{
    IDLE,
    FIRST_PRESS,
    FIRST_RELEASE,
    SECOND_PRESS
};


class Button
{
private:
    // Button Port and Pin
    GPIO_TypeDef* port;
    uint16_t pin;
    
    // Button debouncing
    bool lastBounceState;
    uint32_t lastBounceTime;

    // Button press tracking
    ButtonInternalState internalState;
    ButtonState buttonState;
    
    // Timing for press types
    uint32_t lastEventTime;

    // Button options
    bool doubleClickEnabled;
    

public:
    // Constructor
    Button(GPIO_TypeDef* port, uint16_t pin);

    // Method to initialize button (set pin mode, etc.)
    void init();

    // Method to update button state, call this in the main loop
    void update(uint32_t sysTime);

    // Method to check if a short press occurred
    ButtonState getButtonState(bool reset = true);

    // Internal function to read the button state with debounce
    bool readButton();

    // Disable for multiple fast single click detection
    void setDoubleClickEnabled(bool enable=true);
};

#endif // BUTTON_H
