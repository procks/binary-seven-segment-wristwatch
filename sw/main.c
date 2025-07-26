#include "stm8l15x.h"
#include "glass_lcd.h"
#include "lcd_3digits.h"
#include "stm8l15x_it.h"
#include "button.h"

#define SCROLL_SPEED  50
#define SCROLL_NUM    1

#define LED_PORT  GPIOC
#define LED_PIN  GPIO_Pin_2

#define BUTTON_PORT1  GPIOB
#define BUTTON_PIN1   GPIO_Pin_1
#define BUTTON_PORT2     GPIOC
#define BUTTON_PIN2      GPIO_Pin_3
#define SW3_PORT     GPIOD
#define SW3_PIN      GPIO_Pin_2

#define LONG_PRESS_THRESHOLD 1000
#define SHORT_PRESS_MIN      50

Button btn1;
RTC_TimeTypeDef current_time;

RTC_TimeTypeDef current_time;
char time_str[4];
volatile uint8_t ledState = 0;

void Delay(uint32_t nCount);
bool isSW1Pressed(void);
void processSW1(void);
void delay_ms(uint16_t ms);
void LED_Init(void);

void LED_Init(void)
{
    GPIO_Init(LED_PORT, LED_PIN, GPIO_Mode_Out_PP_Low_Fast);
}

void main(void)
{
  CLK_PeripheralClockConfig(CLK_Peripheral_RTC, ENABLE);

  LCD_GLASS_Init();

  Button_Init(&btn1, BUTTON_PORT1, BUTTON_PIN1, EXTI_Pin_1);
  LED_Init();

  TIM4_TimeBaseInit(TIM4_Prescaler_128, 125);
  TIM4_ITConfig(TIM4_IT_Update, ENABLE);
  TIM4_Cmd(ENABLE);
  RTC_WakeUpClockConfig(RTC_WakeUpClock_CK_SPRE_16bits);
  RTC_ITConfig(RTC_IT_WUT, ENABLE);
  
  enableInterrupts();

  while (1)
  {
    RTC_SetWakeUpCounter(0);
    RTC_WakeUpCmd(ENABLE);
    Button_UpdateStates();
    RTC_GetTime(RTC_Format_BIN, &current_time);
    
    int hour12 = current_time.RTC_Hours % 12;
    if (hour12 == 0)
        hour12 = 12;

    LCD_WriteBin(current_time.RTC_Seconds, 2);
    LCD_WriteBin(current_time.RTC_Minutes, 1);
    LCD_WriteBin(current_time.RTC_Hours, 0);

    halt();

    RTC_WakeUpCmd(DISABLE);
  }
}

void Delay(uint32_t nCount)
{
  while (nCount != 0)
  {
    nCount--;
  }
}

INTERRUPT_HANDLER(RTC_CSSLSE_IRQHandler, 4)
{
  RTC_ClearITPendingBit(RTC_IT_WUT);
}

#ifdef  USE_FULL_ASSERT
void assert_failed(uint8_t* file, uint32_t line)
{
  while (1)
  {}
}
#endif
