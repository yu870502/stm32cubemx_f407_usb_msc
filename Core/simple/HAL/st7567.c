#include <stdio.h>
#include <string.h>

#include "main.h"
#include "st7567.h"
#include "LCDFont.h"

// #define SPI_DC_Pin GPIO_PIN_2
// #define SPI_DC_GPIO_Port GPIOA
// #define SPI_RST_Pin GPIO_PIN_4
// #define SPI_RST_GPIO_Port GPIOA
// #define SPI_CS_Pin GPIO_PIN_6
// #define SPI_CS_GPIO_Port GPIOA

#define ST7567A_DISPLAY_ON 					0xAF
#define ST7567A_DISPLAY_OFF					0xAE
#define ST7567A_SET_START_LINE				0x40			// + line0 - line63
#define ST7567A_SEG_NORMAL					0xA0
#define ST7567A_SEG_REVERSE					0xA1
#define ST7567A_COLOR_NORMAL				0xA6
#define ST7567A_COLOR_INVERSE				0xA7
#define ST7567A_DISPLAY_DRAM				0xA4
#define ST7567A_DISPLAY_ALL_ON				0xA5
#define ST7567A_SW_RESET					0xE2
#define ST7567A_COM_NORMAL					0xC0
#define ST7567A_COM_REVERSE					0xC8
#define ST7567A_POWER_CONTROL				0x28
#define ST7567A_SET_RR						0x20			// + RR[2:0]; 3.0, 3.5, ..., 6.5
#define ST7567A_SET_EV_CMD					0x81
#define ST7567A_NOP							0xE3

#define ST7567A_PAGE_ADDR					0xB0			// + 0x0 - 0x7 -> page0 - page7
#define ST7567A_COL_ADDR_H					0x10			// + X[7:4]
#define ST7567A_COL_ADDR_L					0x00			// + X[3:0]

#define ST7567A_BIAS7						0xA3
#define ST7567A_BIAS9						0xA2

#define ST7567A_PWR_BOOSTER_ON				0x04
#define ST7567A_PWR_REGULATOR_ON			0x02
#define ST7567A_PWR_FOLLOWER_ON				0x01

#define Page0	0xb0  //第0页地址

extern SPI_HandleTypeDef hspi1;

#define ST7567A_TIMEOUT  				100

void Delay_ms(unsigned long value)
{
	HAL_Delay(value);
}

//write commond
void Write_ST7567_COM(uchar COMDADA_4SPI)
{
	// CS0=0; CD=0;
	HAL_GPIO_WritePin(GPIOA, SPI_DC_RS_Pin|SPI_CS_Pin, GPIO_PIN_RESET);

	HAL_StatusTypeDef ret = HAL_SPI_Transmit(&hspi1, &COMDADA_4SPI, 1, ST7567A_TIMEOUT);
	if(ret != HAL_OK)
		printf("Write_ST7567_COM err:%d\r\n", ret);
	
	// CS1=1;
	HAL_GPIO_WritePin(GPIOA, SPI_CS_Pin, GPIO_PIN_SET);			
}


//write data
void Write_ST7567_DATA(uchar DATA_4SPI)
{   
	// CD=1;CS0=0;
	HAL_GPIO_WritePin(GPIOA, SPI_DC_RS_Pin, GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOA, SPI_CS_Pin, GPIO_PIN_RESET);

	HAL_StatusTypeDef ret = HAL_SPI_Transmit(&hspi1, &DATA_4SPI, 1, ST7567A_TIMEOUT);
	if(ret != HAL_OK)
		printf("Write_ST7567_COM err:%d\r\n", ret);

	// CS1=1;
	HAL_GPIO_WritePin(GPIOA, SPI_CS_Pin, GPIO_PIN_SET);		
}

void clear_screen(uchar xx)
{
	uchar i,j;
	for(i=0;i<8;i++){
		// CS0=0;
		HAL_GPIO_WritePin(GPIOA, SPI_CS_Pin, GPIO_PIN_RESET);
		
		Write_ST7567_COM(0xb0+i);	 //SET PAGE 0---8
		Write_ST7567_COM(0x10);	 //SET COLUMN MSB
		Write_ST7567_COM(0x00);	  //SET CLUMN LSB
		for(j=0;j<128;j++){
			Write_ST7567_DATA(xx);
		}
	}
}

int8 clearDDRAMLine(uchar line)
{
	if(line >= (DDRAM_PAGES_MAX >> 1)){
		printf("[%s] param[line] error:%d\r\n", __FUNCTION__, line);
		return -1;
	}

	uchar lineToPage = line << 1;	//line * 2
	uchar i,j;
	for(i=0; i < DDRAM_LINE_PAGES_MAX; i++){
		HAL_GPIO_WritePin(GPIOA, SPI_CS_Pin, GPIO_PIN_RESET);	// CS0=0;
		
		Write_ST7567_COM(Page0 + lineToPage + i);	 //SET PAGE 0---8
		Write_ST7567_COM(0x10);	//SET COLUMN MSB
		Write_ST7567_COM(0x00);	//SET CLUMN LSB
		for(j=0;j<128;j++){
			Write_ST7567_DATA(0);
		}
	}
	return 0;
}

int8 refreshDDRAMLine(uchar line, char *cont, uchar fs)
{
	if(line >= (DDRAM_PAGES_MAX >> 1)){
		printf("[%s] param[line] error:%d\r\n", __FUNCTION__, line);
		return -1;
	}
	if(!cont){
		printf("[%s] param[cont] error:%p\r\n", __FUNCTION__, cont);
		return -1;
	}
	uchar len = strlen((char *)cont);
	if(len > DDRAM_COLUMN_CHARS_MAX){
		printf("[%s] param[cont] len error:%d\r\n", __FUNCTION__, len);
		return -1;
	}

	uchar i = 0;
	for(i = 0; i < len; i++){
		WRITE_CHAR(line, i, *(cont + i), fs);
	}
	return 0;
}

void HDReset()
{
	// RSTB=0;
	HAL_GPIO_WritePin(GPIOA, SPI_RST_Pin, GPIO_PIN_RESET);
	Delay_ms(2);

	// RSTB=1;
	HAL_GPIO_WritePin(GPIOA, SPI_RST_Pin, GPIO_PIN_SET);
	Delay_ms(40);
}

/**
* 函数名称:WRITE_CHAR(uchar plat,uchar column,uchar xs,uchar chr, uchar c)
* 函数功能:显示字符
* 输入参数:plat      行地址
* 输入参数:column    列地址
* 输入参数:chr    显示数值
* 输入参数:fs   显示方式（0，反显，否则正常显示）
* 返 回 值:无
*/
void WRITE_CHAR(uchar plat, uchar column, uchar chr, uchar fs)
{
	uchar page,page1,col,col_h,col_l;
	uchar i,a,j=0;
	page1= plat*2;
	page=page1+Page0;
	col=column*8;
	col_h=((col>>4)|0xf0)&0x1f;
	col_l=col&0x0f;
	
	chr=chr-' ';//得到偏移后的值
	for(a = 0; a < 2; a++){
		Write_ST7567_COM(col_h);
		Write_ST7567_COM(col_l);
		Write_ST7567_COM(page+a);
  		for(i = 0;  i < 8; i ++ ){
			if(fs == 0){Write_ST7567_DATA(s0[chr][j++]);}
			if(fs == 1){Write_ST7567_DATA(~(s0[chr][j++]));}
		}
	}	 
}

/**
* 函数名称:Disp_Nub8X16(uchar Row,uchar Col,uchar Number,uchar fs)
* 函数功能:将数据显示出来
* 输入参数:low       行地址
* 输入参数:Col      列地址
* 输入参数:Number    显示数值
* 输入参数:fs   显示方式（0，反显，否则正常显示）
* 返 回 值:无
*/
void Disp_Nub8X16(uchar Row,uchar Col,uchar Number,uchar fs)
{
	uchar page,page1,col,col_h,col_l,chr;
	uchar i,a,j=0;
	page1=Row*2;
	page=page1+Page0;
	col=Col*8;
	col_h=((col>>4)|0xf0)&0x1f;
	col_l=col&0x0f;
	
	if(Number<11){//十进制
		chr=Number+16;
	}
	else{//十六进制
		chr=Number-32;
	}

	for(a = 0; a <2; a++){
		Write_ST7567_COM(col_h);
		Write_ST7567_COM(col_l);
		Write_ST7567_COM(page+a);
  		for(i = 0;  i <8; i ++ ){
			if(fs == 0){Write_ST7567_DATA(s0[chr][j++]);}
			if(fs == 1){Write_ST7567_DATA(~(s0[chr][j++]));}
		}
	}
}

void Lcd12864_ClearScreen(void)
{
	uchar i, j;
	for(i=0; i<8; i++){
		Write_ST7567_COM(0xB0+i); 
		Write_ST7567_COM(0x10); 
		Write_ST7567_COM(0x00);							   
		for(j=0; j<128; j++){
			Write_ST7567_DATA(0x00);  
		}
	}
}

void Init_ST7567()
{
	HDReset();//硬复位
	HAL_Delay(100);

	Write_ST7567_COM(0xe2);//软复位

	Write_ST7567_COM(0xaf);//0xaf显示器开
	Write_ST7567_COM(0x2f);//0x2f升压电路,电压管理电路,
		
	Write_ST7567_COM(0x25);//0x20-0x27为V5电压内部电阻调整设置 

	Write_ST7567_COM(0x81);// SET EV 调对比度
	Write_ST7567_COM(0x06);//0x01-0x3f电量寄存器设置模式

	Write_ST7567_COM(0xa0);//0xa0为Segment正向,0xa1 为Segment反向
		
	Write_ST7567_COM(0xc8);//0xc0正向扫描,0xc8反射扫描

	Write_ST7567_COM(0xa6);//0xa6正向显示,0xa7反向显示
	Write_ST7567_COM(0xa4);//0xa4正常显示,0xa5全屏点亮

	Write_ST7567_COM(0xf8);//背压比设置
	Write_ST7567_COM(0x00);//00--10
	Lcd12864_ClearScreen();//清屏
	HAL_Delay(1000);
}
