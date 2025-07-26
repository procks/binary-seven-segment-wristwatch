#include "glass_lcd.h"

CONST uint16_t mask[4] =
  {
    0xF000, 0x0F00, 0x00F0, 0x000F
  };
CONST uint8_t shift[4] =
  {
    12, 8, 4, 0
  };

uint8_t digit[4];     /* Digit frame buffer */

/* Letters and number map of PD_878 LCD */
CONST uint16_t LetterMap[26] =
  {
    /* A      B      C      D      E      F      G      H      I  */
    0x4D70, 0x6469, 0x4111, 0x6449, 0x4911, 0x4910, 0x4171, 0x0D70, 0x6009,
    /* J      K      L      M      N      O      P      Q      R  */
    0x0451, 0x0B12, 0x0111, 0x8750, 0x8552, 0x4551, 0x4D30, 0x4553, 0x4D32,
    /* S      T      U      V      W      X      Y      Z  */
    0x4961, 0x6008, 0x0551, 0x0390, 0x05D2, 0x8282, 0x8208, 0x4281
  };

CONST uint16_t NumberMap[10] =
  {
    /* 0      1      2      3      4      5      6      7      8      9  */
    0x3323, 0x0220, 0x1132, 0x1232, 0x0231, 0x1213, 0x3313, 0x0222, 0x3333, 0x3233
  };

__IO uint8_t str[7] = "";
static void Convert(uint8_t* c, Point_Typedef point, Apostrophe_Typedef apostrophe);
static void delay(__IO uint32_t nCount);

void LCD_GLASS_Init(void)
{
  CLK_PeripheralClockConfig(CLK_Peripheral_LCD, ENABLE);
  CLK_RTCClockConfig(CLK_RTCCLKSource_LSE, CLK_RTCCLKDiv_1);

  LCD_Init(LCD_Prescaler_2, LCD_Divider_18, LCD_Duty_1_4,
           LCD_Bias_1_3, LCD_VoltageSource_Internal);

  LCD_PortMaskConfig(LCD_PortMaskRegister_1, 0x40);
  LCD_PortMaskConfig(LCD_PortMaskRegister_2, 0x01 | 0x04 | 0x10 | 0x20);
  LCD_PortMaskConfig(LCD_PortMaskRegister_3, 0x01);
  
    
  LCD_ContrastConfig(LCD_Contrast_Level_7);
  LCD_DeadTimeConfig(LCD_DeadTime_0);
  LCD_PulseOnDurationConfig(LCD_PulseOnDuration_1);

  LCD_Cmd(ENABLE);
}

void LCD_GLASS_WriteChar(uint8_t* ch, Point_Typedef point,
                         Apostrophe_Typedef apostrophe, uint8_t position)
{
  Convert(ch, point, apostrophe);
  switch (position)
  {
    case 0:
      LCD->RAM[LCD_RAMRegister_1] &= ~0x08;
      LCD->RAM[LCD_RAMRegister_1] |= (uint8_t)((digit[0]<<3) & 0x08); // 1D	

      LCD->RAM[LCD_RAMRegister_4] &= ~0x80;
      LCD->RAM[LCD_RAMRegister_4] |= (uint8_t)((digit[1]<<7) & 0x80); // 1E

      LCD->RAM[LCD_RAMRegister_5] &= ~0x04;
      LCD->RAM[LCD_RAMRegister_5] |= (uint8_t)((digit[1]<<1) & 0x04); // 1C

      LCD->RAM[LCD_RAMRegister_8] &= ~0x08;
      LCD->RAM[LCD_RAMRegister_8] |= (uint8_t)((digit[2]<<3) & 0x08); // 1G

      LCD->RAM[LCD_RAMRegister_8] &= ~0x40;
      LCD->RAM[LCD_RAMRegister_8] |= (uint8_t)((digit[2]<<5) & 0x40); // 1B

      LCD->RAM[LCD_RAMRegister_11] &= ~0x80;
      LCD->RAM[LCD_RAMRegister_11] |= (uint8_t)((digit[3]<<7) & 0x80); // 1F										

      LCD->RAM[LCD_RAMRegister_12] &= ~0x04;
      LCD->RAM[LCD_RAMRegister_12] |= (uint8_t)((digit[3]<<1) & 0x04); // 1A
      
      break;
      
    case 1:
      LCD->RAM[LCD_RAMRegister_1] &= ~0x80;
      LCD->RAM[LCD_RAMRegister_1] |= (uint8_t)((digit[0]<<7) & 0x80); // 2D

      LCD->RAM[LCD_RAMRegister_5] &= ~0x08;
      LCD->RAM[LCD_RAMRegister_5] |= (uint8_t)((digit[1]<<3) & 0x08); // 2E

      LCD->RAM[LCD_RAMRegister_5] &= ~0x10;
      LCD->RAM[LCD_RAMRegister_5] |= (uint8_t)((digit[1]<<3) & 0x10); // 2C_

      LCD->RAM[LCD_RAMRegister_8] &= ~0x80;
      LCD->RAM[LCD_RAMRegister_8] |= (uint8_t)((digit[2]<<7) & 0x80); // 2G

      LCD->RAM[LCD_RAMRegister_9] &= ~0x01;
      LCD->RAM[LCD_RAMRegister_9] |= (uint8_t)((digit[2]>>1) & 0x01); // 2B_

      LCD->RAM[LCD_RAMRegister_12] &= ~0x08;
      LCD->RAM[LCD_RAMRegister_12] |= (uint8_t)((digit[3]<<3) & 0x08); // 2F

      LCD->RAM[LCD_RAMRegister_12] &= ~0x10;
      LCD->RAM[LCD_RAMRegister_12] |= (uint8_t)((digit[3]<<3) & 0x10); // 2A_

      break;
    case 2:
      LCD->RAM[LCD_RAMRegister_2] &= ~0x02;
      LCD->RAM[LCD_RAMRegister_2] |= (uint8_t)((digit[0]<<1) & 0x02); // 3D

      LCD->RAM[LCD_RAMRegister_5] &= ~0x20;
      LCD->RAM[LCD_RAMRegister_5] |= (uint8_t)((digit[1]<<5) & 0x20); // 3E

      LCD->RAM[LCD_RAMRegister_5] &= ~0x80;
      LCD->RAM[LCD_RAMRegister_5] |= (uint8_t)((digit[1]<<6) & 0x80); // 3C_

      LCD->RAM[LCD_RAMRegister_9] &= ~0x02;
      LCD->RAM[LCD_RAMRegister_9] |= (uint8_t)((digit[2]<<1) & 0x02); // 3G

      LCD->RAM[LCD_RAMRegister_9] &= ~0x08;
      LCD->RAM[LCD_RAMRegister_9] |= (uint8_t)((digit[2]<<2) & 0x08); // 3B_

      LCD->RAM[LCD_RAMRegister_12] &= ~0x20;
      LCD->RAM[LCD_RAMRegister_12] |= (uint8_t)((digit[3]<<5) & 0x20); // 3F

      LCD->RAM[LCD_RAMRegister_12] &= ~0x80;
      LCD->RAM[LCD_RAMRegister_12] |= (uint8_t)((digit[3]<<6) & 0x80); // 3A_
      
      break;
  }
}

void LCD_GLASS_DisplayString(uint8_t* ptr)
{
  uint8_t i = 0x00;

  /* Send the string character by character on lCD */
  while ((*ptr != 0) & (i < 8))
  {
    /* Display one character on LCD */
    LCD_GLASS_WriteChar(ptr, POINT_OFF, APOSTROPHE_OFF, i);

    /* Point on the next character */
    ptr++;

    /* Increment the character counter */
    i++;
  }
}

void LCD_GLASS_ClearChar(uint8_t position)
{
  switch (position)
  {
      /* Position 0 on LCD (Digit1)*/
    case 0:
      LCD->RAM[LCD_RAMRegister_3]  &=  LCD_CLEAR_LSB;
      LCD->RAM[LCD_RAMRegister_6]  &=  LCD_CLEAR_MSB;
      LCD->RAM[LCD_RAMRegister_10] &=  LCD_CLEAR_LSB;
      LCD->RAM[LCD_RAMRegister_13] &=  LCD_CLEAR_MSB;
      break;

      /* Position 1 on LCD (Digit2)*/
    case 1:
      LCD->RAM[LCD_RAMRegister_2]  &=  LCD_CLEAR_MSB;
      LCD->RAM[LCD_RAMRegister_6]  &=  LCD_CLEAR_LSB;
      LCD->RAM[LCD_RAMRegister_9]  &=  LCD_CLEAR_MSB;
      LCD->RAM[LCD_RAMRegister_13] &=  LCD_CLEAR_LSB;
      break;

      /* Position 2 on LCD (Digit3)*/
    case 2:
      LCD->RAM[LCD_RAMRegister_2]  &=  LCD_CLEAR_LSB;
      LCD->RAM[LCD_RAMRegister_5]  &=  LCD_CLEAR_MSB;
      LCD->RAM[LCD_RAMRegister_9]  &=  LCD_CLEAR_LSB;
      LCD->RAM[LCD_RAMRegister_12] &=  LCD_CLEAR_MSB;
      break;

      /* Position 3 on LCD (Digit4)*/
    case 3:
      LCD->RAM[LCD_RAMRegister_1]  &=  LCD_CLEAR_MSB;
      LCD->RAM[LCD_RAMRegister_5]  &=  LCD_CLEAR_LSB;
      LCD->RAM[LCD_RAMRegister_8]  &=  LCD_CLEAR_MSB;
      LCD->RAM[LCD_RAMRegister_12] &=  LCD_CLEAR_LSB;
      break;

      /* Position 4 on LCD (Digit5)*/
    case 4:
      LCD->RAM[LCD_RAMRegister_1]  &=  LCD_CLEAR_LSB;
      LCD->RAM[LCD_RAMRegister_4]  &=  LCD_CLEAR_MSB;
      LCD->RAM[LCD_RAMRegister_8]  &=  LCD_CLEAR_LSB;
      LCD->RAM[LCD_RAMRegister_11] &=  LCD_CLEAR_MSB;
      break;

      /* Position 5 on LCD (Digit6)*/
    case 5:
      LCD->RAM[LCD_RAMRegister_0]  &=  LCD_CLEAR_MSB;
      LCD->RAM[LCD_RAMRegister_4]  &=  LCD_CLEAR_LSB;
      LCD->RAM[LCD_RAMRegister_7]  &=  LCD_CLEAR_MSB;
      LCD->RAM[LCD_RAMRegister_11] &=  LCD_CLEAR_LSB;
      break;

      /* Position 6 on LCD (Digit7)*/
    case 6:
      LCD->RAM[LCD_RAMRegister_0]  &=  LCD_CLEAR_LSB;
      LCD->RAM[LCD_RAMRegister_3]  &=  LCD_CLEAR_MSB;
      LCD->RAM[LCD_RAMRegister_7]  &=  LCD_CLEAR_LSB;
      LCD->RAM[LCD_RAMRegister_10] &=  LCD_CLEAR_MSB;
      break;

    default:
      break;
  }
}

void LCD_GLASS_Clear(void)
{
  uint8_t counter = 0;
  for (counter = 0;counter < 0x0E; counter++)
  {
    LCD->RAM[counter] =  LCD_RAM_RESET_VALUE;
  }
}

void LCD_GLASS_ScrollString(uint8_t* ptr, uint16_t nScroll, uint16_t ScrollSpeed)
{
  uint8_t Repetition = 0;
  uint8_t* ptr1;
  ptr1 = ptr;

  LCD_GLASS_DisplayString(ptr1);

  delay(ScrollSpeed);

  for (Repetition = 0; Repetition < nScroll; Repetition++)
  {
    *(str + 1) = *ptr1;
    *(str + 2) = *(ptr1 + 1);
    *(str + 3) = *(ptr1 + 2);
    *(str + 4) = *(ptr1 + 3);
    *(str + 5) = *(ptr1 + 4);
    *(str + 6) = *(ptr1 + 5);
    *(str) = *(ptr1 + 6);
    LCD_GLASS_Clear();
    LCD_GLASS_DisplayString((uint8_t*)str);
    delay(ScrollSpeed);

    *(str + 1) = *(ptr1 + 6);
    *(str + 2) = *ptr1;
    *(str + 3) = *(ptr1 + 1);
    *(str + 4) = *(ptr1 + 2);
    *(str + 5) = *(ptr1 + 3);
    *(str + 6) = *(ptr1 + 4);
    *(str) = *(ptr1 + 5);
    LCD_GLASS_Clear();
    LCD_GLASS_DisplayString((uint8_t*)str);
    delay(ScrollSpeed);

    *(str + 1) = *(ptr1 + 5);
    *(str + 2) = *(ptr1 + 6);
    *(str + 3) = *ptr1;
    *(str + 4) = *(ptr1 + 1);
    *(str + 5) = *(ptr1 + 2);
    *(str + 6) = *(ptr1 + 3);
    *(str) = *(ptr1 + 4);
    LCD_GLASS_Clear();
    LCD_GLASS_DisplayString((uint8_t*)str);
    delay(ScrollSpeed);

    *(str + 1) = *(ptr1 + 4);
    *(str + 2) = *(ptr1 + 5);
    *(str + 3) = *(ptr1 + 6);
    *(str + 4) = *ptr1;
    *(str + 5) = *(ptr1 + 1);
    *(str + 6) = *(ptr1 + 2);
    *(str) = *(ptr1 + 3);
    LCD_GLASS_Clear();
    LCD_GLASS_DisplayString((uint8_t*)str);
    delay(ScrollSpeed);

    *(str + 1) = *(ptr1 + 3);
    *(str + 2) = *(ptr1 + 4);
    *(str + 3) = *(ptr1 + 5);
    *(str + 4) = *(ptr1 + 6);
    *(str + 5) = *ptr1;
    *(str + 6) = *(ptr1 + 1);
    *(str) = *(ptr1 + 2);
    LCD_GLASS_Clear();
    LCD_GLASS_DisplayString((uint8_t*)str);
    delay(ScrollSpeed);

    *(str + 1) = *(ptr1 + 2);
    *(str + 2) = *(ptr1 + 3);
    *(str + 3) = *(ptr1 + 4);
    *(str + 4) = *(ptr1 + 5);
    *(str + 5) = *(ptr1 + 6);
    *(str + 6) = *ptr1;
    *(str) = *(ptr1 + 1);
    LCD_GLASS_Clear();
    LCD_GLASS_DisplayString((uint8_t*)str);
    delay(ScrollSpeed);

    *(str + 1) = *(ptr1 + 1);
    *(str + 2) = *(ptr1 + 2);
    *(str + 3) = *(ptr1 + 3);
    *(str + 4) = *(ptr1 + 4);
    *(str + 5) = *(ptr1 + 5);
    *(str + 6) = *(ptr1 + 6);
    *(str) = *ptr1;
    LCD_GLASS_Clear();
    LCD_GLASS_DisplayString((uint8_t*)str);
    delay(ScrollSpeed);
  }
}

static void Convert(uint8_t* c, Point_Typedef point, Apostrophe_Typedef apostrophe)
{
  uint16_t ch = 0 , tmp = 0;
  uint8_t i;

  /* The character c is a letter in upper case*/
  if ((*c < 0x5B)&(*c > 0x40))
  {
    ch = LetterMap[*c-0x41];
  }
  /* The character c is a number*/
  if ((*c < 0x3A)&(*c > 0x2F))
  {
    ch = NumberMap[*c-0x30];
  }
  /* The character c is a space character */
  if (*c == 0x20)
  {
    ch = 0x00;
  }
  /* Set the DP seg in the character that can be displayed if the point is on */
  if (point == POINT_ON)
  {
    ch |= 0x0004;
  }

  /* Set the X seg in the character that can be displayed if the apostrophe is on */
  if (apostrophe == APOSTROPHE_ON)
  {
    ch |= 0x1000;
  }

  for (i = 0;i < 4; i++)
  {
    tmp = ch & mask[i];
    digit[i] = (uint8_t)(tmp >> (uint8_t)shift[i]);
  }
}

static void delay(__IO uint32_t nCount)
{
  __IO uint32_t index = 0;
  for (index = (0x60 * nCount); index != 0; index--)
  {}
}

