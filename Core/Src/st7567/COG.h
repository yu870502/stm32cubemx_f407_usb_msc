#ifndef __COG_H
#define __COG_H	

#define uint unsigned int
#define uchar unsigned  char

void Init_ST7567(void);
void Write_ST7567_COM(uchar COMDADA_4SPI);
void Write_ST7567_DATA(uchar DATA_4SPI);
void HDReset(void);
void clear_screen(uchar xx);
void Display_pic1(void);
void Delay_ms(unsigned long value);
void WRITE_STRING(uchar plat, uchar column, uchar chr, uchar fs);
void Lcd12864_ClearScreen(void);
void Disp_Nub8X16(uchar Row,uchar Col,uchar Number,uchar fs);

void lcd_hello(void);

#endif

