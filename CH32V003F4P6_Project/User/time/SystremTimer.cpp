#include "SystemTimer.h"
#include "ch32v00x.h"

static volatile uint32_t tick_count = 0;

#ifdef ENABLE_MICROS
    #define SYSTICK_FREQ_HZ 1000000  // 1 MHz → 1 µs Ticks
#else
    #define SYSTICK_FREQ_HZ 1000     // 1 kHz → 1 ms Ticks
#endif

extern "C" void SysTick_Handler(void);
void SysTick_Handler(void) __attribute__((interrupt("WCH-Interrupt-fast")));
void SysTick_Handler(void)
{
    tick_count++;
    SysTick->SR = 0;
}

void SystemTimer_Init()
{
    SysTick->CMP  = SystemCoreClock / SYSTICK_FREQ_HZ;  // Changes based on if micros() is enabled
    SysTick->CNT  = 0;
    SysTick->SR   = 0;

    NVIC_EnableIRQ(SysTicK_IRQn);

    SysTick->CTLR = (1 << 3) | // Auto-reload
                    (1 << 2) | // Clock = HCLK
                    (1 << 1) | // Interrupt Enable
                    (1 << 0);  // Counter Enable
}

uint32_t millis()
{
#ifdef ENABLE_MICROS
    return tick_count / 1000;
#else
    return tick_count;
#endif
}

void delay_ms(uint32_t ms)
{
    uint32_t start = millis();
    while ((millis() - start) < ms);
}

#ifdef ENABLE_MICROS
uint32_t micros()
{
    return tick_count;
}

void delay_us(uint32_t us)
{
    uint32_t start = micros();
    while ((micros() - start) < us);
}
#endif
