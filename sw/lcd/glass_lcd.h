#ifndef __GLASS_LCD_H
#define __GLASS_LCD_H

#include "stm8l15x.h"

typedef enum
{
  POINT_OFF = 0,
  POINT_ON = 1
}Point_Typedef;

typedef enum
{
  APOSTROPHE_OFF = 0,
  APOSTROPHE_ON = 1
}Apostrophe_Typedef;


#define LCD_CLEAR_BYTE (uint8_t)0x00
#define LCD_SET_BYTE   (uint8_t)0xFF
#define LCD_CLEAR_MSB  (uint8_t)0x0F
#define LCD_CLEAR_LSB  (uint8_t)0xF0

void LCD_GLASS_Init(void);
void LCD_GLASS_WriteChar(uint8_t* ch, Point_Typedef point, Apostrophe_Typedef apostrophe, uint8_t position);
void LCD_GLASS_DisplayString(uint8_t* ptr);
void LCD_GLASS_ClearChar(uint8_t position);
void LCD_GLASS_Clear(void);
void LCD_GLASS_ScrollString(uint8_t* ptr, uint16_t nScroll, uint16_t ScrollSpeed);
void LCD_GLASS_DisplayChar(uint8_t* ch, Point_Typedef point, Apostrophe_Typedef apostrophe, uint8_t position);

#endif

