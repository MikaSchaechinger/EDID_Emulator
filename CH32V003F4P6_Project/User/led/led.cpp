#include "led.h"


LED::LED(GPIO_TypeDef* port, uint16_t pin, TIM_TypeDef* timer, uint8_t pwmChannel)
{
    this->port = port;
    this->pin = pin;
    this->timer = timer;
    this->pwmChannel = pwmChannel;
    this->brightness = 0;
    this->brightnessFunction = NULL;

}


void LED::init() {
    GPIO_InitTypeDef GPIO_InitStructure = {0};
    TIM_OCInitTypeDef TIM_OCInitStructure = {0};
    TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure = {0};

    // Aktiviere die Clock für GPIO und den Timer (TIM1 oder TIM2, abhängig von timer)
    if (timer == TIM1) {
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD | RCC_APB2Periph_TIM1, ENABLE);
    } else if (timer == TIM2) {
        RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
    }

    // Konfiguriere den GPIO-Pin als alternativen Funktionsausgang (PWM)
    GPIO_InitStructure.GPIO_Pin = pin;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(port, &GPIO_InitStructure);

    // Konfiguriere den Timer (PWM-Modus)
    TIM_TimeBaseInitStructure.TIM_Period = 100;  // Setzt den Zeitraum für PWM
    TIM_TimeBaseInitStructure.TIM_Prescaler = 48000 - 1;  // Setzt den Prescaler (je nach Systemtakt)
    TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInit(timer, &TIM_TimeBaseInitStructure);

    // Konfiguriere den Output-Compare (PWM) Kanal
    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
    TIM_OCInitStructure.TIM_Pulse = 50;  // Setzt den anfänglichen Duty Cycle auf 50%
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;

    // Konfiguriere den richtigen PWM-Kanal basierend auf dem pwmChannel Parameter
    if (pwmChannel == 1) {
        TIM_OC1Init(timer, &TIM_OCInitStructure);
    } else if (pwmChannel == 2) {
        TIM_OC2Init(timer, &TIM_OCInitStructure);
    } else if (pwmChannel == 3) {
        TIM_OC3Init(timer, &TIM_OCInitStructure);
    } else if (pwmChannel == 4) {
        TIM_OC4Init(timer, &TIM_OCInitStructure);
    }

    // Aktiviere PWM
    TIM_CtrlPWMOutputs(timer, ENABLE);
    TIM_OC1PreloadConfig(timer, TIM_OCPreload_Enable);
    TIM_ARRPreloadConfig(timer, ENABLE);
    TIM_Cmd(timer, ENABLE);
}


void LED::setBrightness(uint8_t brightness) {
    // limit brightness to 0-100%	
    if (brightness > 100) {
        brightness = 100;
    }

    // Calculate the pulse value based on the brightness
    uint16_t pulseValue = (brightness * timer->ATRLR) / 100;

    // Set the pulse value based on the PWM channel
    if (pwmChannel == 1) {
        timer->CH1CVR = pulseValue;  // Set the pulse value for channel 1
    } else if (pwmChannel == 2) {
        timer->CH2CVR = pulseValue;  // Set the pulse value for channel 2
    } else if (pwmChannel == 3) {
        timer->CH3CVR = pulseValue;  // Set the pulse value for channel 3
    } else if (pwmChannel == 4) {
        timer->CH4CVR = pulseValue;  // Set the pulse value for channel 4
    }
}


uint8_t LED::getBrightness() {
    return this->brightness;
}