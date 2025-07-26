#ifndef BUTTON_H
#define BUTTON_H

#include "stm8l15x.h"
//#include <stdbool.h>

typedef enum {
    BUTTON_EVENT_NONE,
    BUTTON_EVENT_SHORT_PRESS,
    BUTTON_EVENT_LONG_PRESS
} ButtonEvent;

typedef struct {
    GPIO_TypeDef* port;
    uint8_t pin;
    uint8_t exti_pin;
    volatile uint32_t press_start;
    volatile bool is_pressed;
    volatile bool event_ready;
} Button;

// Debounce time in subsecond ticks (30ms ˜ 1000 ticks)
#define DEBOUNCE_SUBSEC 1000
// Long press duration in subsecond ticks (2s = 65536 ticks)
#define LONG_PRESS_SUBSEC 65536

void Button_Init(Button* btn, GPIO_TypeDef* port, uint8_t pin, uint8_t exti_pin);
ButtonEvent Button_GetEvent(Button* btn);
void Button_ProcessInterrupt(Button* btn);
void Button_UpdateStates(void);

#endif /* BUTTON_H */