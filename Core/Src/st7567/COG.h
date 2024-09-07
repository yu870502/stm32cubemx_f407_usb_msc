#ifndef __COG_H
#define __COG_H	

#define uint unsigned int
#define uchar unsigned  char
#define int8 signed char

#define DDRAM_PAGES_MAX 8
#define DDRAM_LINE_PAGES_MAX 2
#define DDRAM_COLUMN_CHARS_MAX 16

void Delay_ms(unsigned long value);
void Init_ST7567(void);
void Write_ST7567_COM(uchar COMDADA_4SPI);
void Write_ST7567_DATA(uchar DATA_4SPI);
void HDReset(void);
void clear_screen(uchar xx);
int8 clearDDRAMLine(uchar line);
int8 refreshDDRAMLine(uchar line, char *cont, uchar fs);
void WRITE_CHAR(uchar plat, uchar column, uchar chr, uchar fs);
void Lcd12864_ClearScreen(void);
void Disp_Nub8X16(uchar Row,uchar Col,uchar Number,uchar fs);

void lcd_hello(void);

#endif

