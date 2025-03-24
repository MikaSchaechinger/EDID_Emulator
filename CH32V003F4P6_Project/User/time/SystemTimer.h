#pragma once

#include <stdint.h>

// configurable behavior
//#define ENABLE_MICROS  // ← activate, if you need micros()

void SystemTimer_Init();

uint32_t millis();
void delay_ms(uint32_t ms);

#ifdef ENABLE_MICROS
uint32_t micros();
void delay_us(uint32_t us);
#endif
