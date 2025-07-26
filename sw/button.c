#include "button.h"
#include "stm8l15x_it.h"

Button* active_buttons[2] = {0, 0};
uint8_t button_count = 0;

void Button_Init(Button* btn, GPIO_TypeDef* port, uint8_t pin, uint8_t exti_pin) {
    btn->port = port;
    btn->pin = pin;
    btn->exti_pin = exti_pin;
    btn->press_start = 0;
    btn->is_pressed = FALSE;
    btn->event_ready = FALSE;
    
    GPIO_Init(btn->port, btn->pin, GPIO_Mode_In_PU_IT);
    EXTI_SetPinSensitivity(btn->exti_pin, EXTI_Trigger_Falling);
    
    if(button_count < 2) {
        active_buttons[button_count++] = btn;
    }
}

ButtonEvent Button_GetEvent(Button* btn) {
    if(btn->event_ready) {
        btn->event_ready = FALSE;
        return btn->is_pressed ? BUTTON_EVENT_LONG_PRESS : BUTTON_EVENT_SHORT_PRESS;
    }
    return BUTTON_EVENT_NONE;
}

void Button_ProcessInterrupt(Button* btn) {
    static uint32_t last_interrupt = 0;
    while (RTC_WaitForSynchro() != SUCCESS);
    uint32_t current = RTC_GetSubSecond();
    
    // Software debounce
    if((current - last_interrupt) > DEBOUNCE_SUBSEC) {
        if(!GPIO_ReadInputDataBit(btn->port, btn->pin)) {
            btn->press_start = current;
            btn->is_pressed = TRUE;
        } else {
            uint32_t duration = current - btn->press_start;
            if(duration > DEBOUNCE_SUBSEC) {
                btn->event_ready = TRUE;
            }
            btn->is_pressed = FALSE;
        }
    }
    last_interrupt = current;
    EXTI_ClearITPendingBit(btn->exti_pin);
}

void Button_UpdateStates(void) {
    while (RTC_WaitForSynchro() != SUCCESS);
    uint32_t current = RTC_GetSubSecond();
    
    for(uint8_t i = 0; i < button_count; i++) {
        Button* btn = active_buttons[i];
        if(btn->is_pressed) {
            uint32_t duration = current - btn->press_start;
            
            // Handle wrap-around
            if(duration > 0x80000000) duration = current + (0xFFFFFFFF - btn->press_start);
            
            if(duration >= LONG_PRESS_SUBSEC) {
                btn->event_ready = TRUE;
                btn->is_pressed = FALSE; // Reset after long press detection
            }
        }
    }
}


INTERRUPT_HANDLER(EXTI1_IRQHandler, 9) {
    for(uint8_t i = 0; i < button_count; i++) {
        if(EXTI_GetITStatus(active_buttons[i]->exti_pin)) {
            Button_ProcessInterrupt(active_buttons[i]);
        }
    }
}