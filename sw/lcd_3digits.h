
#ifndef __LCD_3DIGITS_H
 #define __LCD_3DIGITS_H

#include "stm8l15x.h"

void LCD_WriteInt(uint32_t value);
void LCD_WriteIntPos(uint8_t value, uint8_t position);
void LCD_WriteBin(uint8_t value, uint8_t position);


#endif /* __LCD_3DIGITS_H */