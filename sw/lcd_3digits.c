#include "lcd_3digits.h"

void setDigitSegments(const uint8_t active[7], uint8_t position);

#define DIGIT_N 13
const uint8_t numberSegments[DIGIT_N][7] = {
    // Segments for digits 0-9 (A, B, C, D, E, F, G)
    {1,1,1,1,1,1,0}, // 0
    {0,1,1,0,0,0,0}, // 1
    {1,1,0,1,1,0,1}, // 2
    {1,1,1,1,0,0,1}, // 3
    {0,1,1,0,0,1,1}, // 4
    {1,0,1,1,0,1,1}, // 5
    {1,0,1,1,1,1,1}, // 6
    {1,1,1,0,0,0,0}, // 7
    {1,1,1,1,1,1,1}, // 8
    {1,1,1,1,0,1,1}, // 9
    {1,1,1,1,1,0,0}, // 10
    {0,1,1,0,0,1,0}, // 11
    {1,1,0,1,1,1,1}, // 12
};

const uint8_t emptySegments[DIGIT_N] = {0, 0, 0, 0, 0, 0, 0};  // empty
const uint8_t bitZeroSegments[DIGIT_N] = {0, 0, 0, 0, 0, 0, 1};  // -

static uint8_t activeBits[7];

const uint8_t digit0Segments[7][2] = {
  {2, 0x20}, //A
  {6, 0x02}, //B
  {9, 0x20}, //C
  {13, 0x10}, //D
  {10, 0x01}, //E
  {3, 0x01}, //F
  {6, 0x10}, //G
};

const uint8_t digit1Segments[7][2] = {
  {2, 0x04}, //A
  {5, 0x40}, //B
  {9, 0x04}, //C
  {13, 0x01}, //D
  {9, 0x10}, //E
  {2, 0x10}, //F
  {6, 0x01}, //G
};

const uint8_t digit2Segments[7][2] = {
  {1, 0x40}, //A
  {5, 0x04}, //B
  {8, 0x40}, //C
  {12, 0x10}, //D
  {9, 0x01}, //E
  {2, 0x01}, //F
  {5, 0x10}, //G
};

//Seg1 PB4 SEG14
//Seg2 PB6 SEG16
//Seg3 PD4 SEG18
//Seg4 PD6 SEG20
//Seg5 PD7 SEG21
//Seg6 PC4 SEG24
  
// 28 bit
//example segment 14 com 1: 1 * 28 + 14 = 42;  RAM[42 / 8] = 1 << 42 % 8  //   LCD->RAM[5] |= 0x04
// https://pl.kotl.in/D8QqgNEhd
// https://pl.kotl.in/o2Y2ADsXu
//    val segments = listOf(14, 16, 18, 20, 21, 24)
//    segments.forEach { seg ->
//        for (com in 0..3) {
//                val bitsn = com * 28 + seg
//                val ramIndex = bitsn / 8
//                val bit = bitsn % 8
//                val mask = 1 shl bit
//                //println("LCD->RAM[$ramIndex] |= 0x${mask.toString(16).padStart(2, '0').uppercase()}; // SEG$seg COM$com")
//                println("{$ramIndex, 0x${mask.toString(16).padStart(2, '0').uppercase()}},")
//        }
//    }

//LCD->RAM[1] |= 0x40; // SEG14 COM0 // 3A
//LCD->RAM[5] |= 0x04; // SEG14 COM1 // 3B
//LCD->RAM[8] |= 0x40; // SEG14 COM2 // 3C
////LCD->RAM[12] |= 0x04; // SEG14 COM3 // nc
//LCD->RAM[2] |= 0x01; // SEG16 COM0 // 3F
//LCD->RAM[5] |= 0x10; // SEG16 COM1 // 3G
//LCD->RAM[9] |= 0x01; // SEG16 COM2 // 3E
//LCD->RAM[12] |= 0x10; // SEG16 COM3 // 3D
    
//LCD->RAM[2] |= 0x04; // SEG18 COM0 // 2A
//LCD->RAM[5] |= 0x40; // SEG18 COM1 // 2B
//LCD->RAM[9] |= 0x04; // SEG18 COM2 // 2C
////LCD->RAM[12] |= 0x40; // SEG18 COM3 // nc
//LCD->RAM[2] |= 0x10; // SEG20 COM0 // 2F
//LCD->RAM[6] |= 0x01; // SEG20 COM1 // 2G
//LCD->RAM[9] |= 0x10; // SEG20 COM2 // 2E
//LCD->RAM[13] |= 0x01; // SEG20 COM3 // 2D
    
//LCD->RAM[2] |= 0x20; // SEG21 COM0 // 1A
//LCD->RAM[6] |= 0x02; // SEG21 COM1 // 1B
//LCD->RAM[9] |= 0x20; // SEG21 COM2 // 1C
////LCD->RAM[13] |= 0x02; // SEG21 COM3 // nc
//LCD->RAM[3] |= 0x01; // SEG24 COM0 // 1F
//LCD->RAM[6] |= 0x10; // SEG24 COM1 // 1G
//LCD->RAM[10] |= 0x01; // SEG24 COM2 // 1E
//LCD->RAM[13] |= 0x10; // SEG24 COM3 // 1D

const uint8_t *digitSegmentMappings[] = {
    (const uint8_t *)digit0Segments,
    (const uint8_t *)digit1Segments,
    (const uint8_t *)digit2Segments
};

void setDigitSegments(const uint8_t active[7], uint8_t position) {
    const uint8_t (*segments)[2] = (const uint8_t (*)[2])digitSegmentMappings[position];

    for (int i = 0; i < 7; i++) {
        if (active[i]) {
            LCD->RAM[segments[i][0]] |= segments[i][1];
        } else {
            LCD->RAM[segments[i][0]] &= ~segments[i][1];
        }
    }
}

void LCD_WriteInt(uint32_t value) {
    const uint8_t num_digits = 3;
    for (int i = num_digits - 1; i >= 0; i--) {
        if (value != 0 || i == num_digits - 1) {
            setDigitSegments(numberSegments[value % 10], i);
            value /= 10;
        } else {
            setDigitSegments(emptySegments, i);
        }
    }
}

void LCD_WriteIntPos(uint8_t value, uint8_t position) {
    setDigitSegments(numberSegments[value], position);
}

void LCD_WriteBin(uint8_t value, uint8_t position) {
    if (value == 0) {
        setDigitSegments(bitZeroSegments, position);
    } else {
        activeBits[0] = (value >> 5) & 1;  // Directly assign bit 5 to index 0

        for (int i = 0; i < 5; i++) {
            //activeBits[i] = (value >> i) & 1;
            activeBits[i + 1] = (value >> i) & 1;
        }

        setDigitSegments(activeBits, position);
    }
}
