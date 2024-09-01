#include <stdio.h>

#include "main.h"
#include "COG.h"
#include "LCDFont.h"

// #define SPI_DC_Pin GPIO_PIN_2
// #define SPI_DC_GPIO_Port GPIOA
// #define SPI_RST_Pin GPIO_PIN_4
// #define SPI_RST_GPIO_Port GPIOA
// #define SPI_CS_Pin GPIO_PIN_6
// #define SPI_CS_GPIO_Port GPIOA

#define Page0	0xb0  //第0页地址
#define Page_max   0xb8  //最大页地址,第8页
#define Column_h   0x10  //列地址(高位)
#define Column_l   0x00  //列地址(低位)

//--------------------------------------------------------- 4线SPI接口 															  
// sbit    RSTB	=P3^0;
// sbit    SDO   =P1^4;//串行数据
// sbit		SCLK	=P1^3;//串行时钟
// sbit    CS0  	=P1^5;//片选信号
// sbit    CD    =P1^0;//命令/数据切换
// //--------------------------------------------------------------------------
extern SPI_HandleTypeDef hspi1;

#define ST7567A_TIMEOUT  				100

//写命令函数
void Write_ST7567_COM(uchar COMDADA_4SPI)
{
//	uchar bit8;

	HAL_GPIO_WritePin(GPIOA, SPI_DC_RS_Pin|SPI_CS_Pin, GPIO_PIN_RESET);
	// CD=0;
	// CS0=0;

// 	for(bit8=0;bit8<8;bit8++)
// 	{
// 		SCLK=0;
// 		if((COMDADA_4SPI&0x80)==0x80)
// 		SDO=1;
// 		else		
// 		SDO=0;
// //		SCL=0;
// 		SCLK=1;
// 		COMDADA_4SPI=(COMDADA_4SPI<<1);

// 	}

	HAL_StatusTypeDef ret = HAL_SPI_Transmit(&hspi1, &COMDADA_4SPI, 1, ST7567A_TIMEOUT);
	if(ret != HAL_OK)
		printf("Write_ST7567_COM err:%d\r\n", ret);
	
  //  CS1=1;
	HAL_GPIO_WritePin(GPIOA, SPI_CS_Pin, GPIO_PIN_SET);	
			
}


//写数据函数
void Write_ST7567_DATA(uchar DATA_4SPI)
{   
	// uchar bit8;

	HAL_GPIO_WritePin(GPIOA, SPI_DC_RS_Pin, GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOA, SPI_CS_Pin, GPIO_PIN_RESET);
	// CD=1;
	// CS0=0;				

	// for(bit8=0;bit8<8;bit8++)
	// {
	// 	SCLK=0;
	// 	if((DATA_4SPI&0x80)==0x80)
	// 	SDO=1;
	// 	else
	// 	SDO=0;
	// 	SCLK=1;
	// 	DATA_4SPI=(DATA_4SPI<<1);

	HAL_StatusTypeDef ret = HAL_SPI_Transmit(&hspi1, &DATA_4SPI, 1, ST7567A_TIMEOUT);
	if(ret != HAL_OK)
		printf("Write_ST7567_COM err:%d\r\n", ret);

	// }
//	CS1=1;
	HAL_GPIO_WritePin(GPIOA, SPI_CS_Pin, GPIO_PIN_SET);		
}



//清屏函数
void clear_screen(uchar xx)
{
uchar i,j;
for(i=0;i<8;i++)
	{
		// CS0=0;
		HAL_GPIO_WritePin(GPIOA, SPI_CS_Pin, GPIO_PIN_RESET);
		
		Write_ST7567_COM(0xb0+i);	 //SET PAGE 0---8
	  Write_ST7567_COM(0x10);	 //SET COLUMN MSB
		Write_ST7567_COM(0x00);	  //SET CLUMN LSB
		for(j=0;j<128;j++)
		{
			Write_ST7567_DATA(xx);
		}
	 }
}



//显示图片函数
void Display_pic1()
{
	uint i,j,adressd=0;

	for(i=0;i<8;i++)
	{
		// CS0=0;
		HAL_GPIO_WritePin(GPIOA, SPI_CS_Pin, GPIO_PIN_RESET);

		Write_ST7567_COM(0xb0+i);
	  Write_ST7567_COM(0x10);
		Write_ST7567_COM(0x00);
		for(j=0;j<96;j++)
		{
			Write_ST7567_DATA(graphic1[adressd]);
			adressd++;
		}
	}
} 
		
//软件延时（不怎么准
void Delay_ms(unsigned long value)
{
	// uint i,j;
    // for (i=0;i<value;i++)
	// 	for (j=0;j<500;j++);
	HAL_Delay(value);
}



//硬件复位函数
void HDReset()
{
	// RSTB=0;
	HAL_GPIO_WritePin(GPIOA, SPI_RST_Pin, GPIO_PIN_RESET);
	Delay_ms(2);

	// RSTB=1;
	HAL_GPIO_WritePin(GPIOA, SPI_RST_Pin, GPIO_PIN_SET);
	Delay_ms(40);
}




///****************************************************************************/
////函数名称:WRITE_STRING(uchar plat,uchar column,uchar xs,uchar chr, uchar c)
////函数功能:显示字符
////输入参数:plat      行地址
////输入参数:column    列地址
////输入参数:chr    显示数值
////输入参数:fs   显示方式（0，反显，否则正常显示）
////返 回 值:无
///****************************************************************************/
void WRITE_STRING(uchar plat, uchar column, uchar chr, uchar fs)
{
	uchar page,page1,col,col_h,col_l;
	uchar i,a,j=0;
	page1= plat*2;
	page=page1+Page0;
	col=column*8;
	col_h=((col>>4)|0xf0)&0x1f;
	col_l=col&0x0f;
	
	chr=chr-' ';//得到偏移后的值	
	for(a = 0; a <2; a++)
	{
		Write_ST7567_COM(col_h);
		Write_ST7567_COM(col_l);
		Write_ST7567_COM(page+a);
  	for(i = 0;  i <8; i ++ )
	 	{
				if(fs == 0){Write_ST7567_DATA(s0[chr][j++]);}
				if(fs == 1){Write_ST7567_DATA(~(s0[chr][j++]));}
		}
	}	 
}


///****************************************************************************/
////函数名称:Disp_Nub8X16(uchar Row,uchar Col,uchar Number,uchar fs)
////函数功能:将数据显示出来
////输入参数:low       行地址
////输入参数:Col      列地址
////输入参数:Number    显示数值
////输入参数:fs   显示方式（0，反显，否则正常显示）
////返 回 值:无
///****************************************************************************/
void Disp_Nub8X16(uchar Row,uchar Col,uchar Number,uchar fs)
{
	uchar page,page1,col,col_h,col_l,chr;
	uchar i,a,j=0;
	page1=Row*2;
	page=page1+Page0;
	col=Col*8;
	col_h=((col>>4)|0xf0)&0x1f;
	col_l=col&0x0f;
	
	if(Number<11)//十进制数据
	{
		chr=Number+16;
	}
		
	else//十六进制数据
	{
		chr=Number-32;
	}

	for(a = 0; a <2; a++)
	{
		Write_ST7567_COM(col_h);
		Write_ST7567_COM(col_l);
		Write_ST7567_COM(page+a);
  	for(i = 0;  i <8; i ++ )
	 	{
				if(fs == 0){Write_ST7567_DATA(s0[chr][j++]);}
				if(fs == 1){Write_ST7567_DATA(~(s0[chr][j++]));}
		}
	}	
}






//清屏函数
void Lcd12864_ClearScreen(void)
{
	uchar i, j;

	for(i=0; i<8; i++)
	{
		Write_ST7567_COM(0xB0+i); 
		Write_ST7567_COM(0x10); 
		Write_ST7567_COM(0x00);							   
		for(j=0; j<128; j++)
		{
			Write_ST7567_DATA(0x00);  
		}
	}
}


///****************************************************************************/
////函数名称:Init_ST7567()
////函数功能:初始化液晶屏
////输入参数:无
////返 回 值:无
////		TIPS:命令符见ST7567数据手册
///****************************************************************************/
void Init_ST7567()
{
	HDReset();//硬复位
	// Timer3_Delay100ms(1);
	HAL_Delay(100);

	Write_ST7567_COM(0xe2);//软复位

		Write_ST7567_COM(0xa2);//背压比设置

	Write_ST7567_COM(0xaf);//0xaf显示器开

	Write_ST7567_COM(0x2f);//0x2f升压电路,电压管理电路,
		
	Write_ST7567_COM(0x25);//0x20-0x27为V5电压内部电阻调整设置 

	Write_ST7567_COM(0x81);// SET EV 调对比度
	Write_ST7567_COM(0x01);//0x01-0x3f电量寄存器设置模式

	Write_ST7567_COM(0xa0);//0xa0为Segment正向,0xa1 为Segment反向
		
	Write_ST7567_COM(0xc8);//0xc0正向扫描,0xc8反射扫描

	Write_ST7567_COM(0xa6);//0xa6正向显示,0xa7反向显示
	Write_ST7567_COM(0xa4);//0xa4正常显示,0xa5全屏点亮

	Write_ST7567_COM(0xf8);//背压比设置
	Write_ST7567_COM(0x00);//00--10
			
	Lcd12864_ClearScreen();//清屏
}

void lcd_hello(void)
{
	WRITE_STRING(0,0,'H',0);
	WRITE_STRING(0,1,'e',0);
	WRITE_STRING(0,2,'l',0);
	WRITE_STRING(0,3,'l',0);
	WRITE_STRING(0,4,'o',0);
	
	WRITE_STRING(2,0,'n',0);
	WRITE_STRING(2,1,'i',0);
	WRITE_STRING(2,2,'h',0);
	WRITE_STRING(2,3,'a',0);
	WRITE_STRING(2,4,'o',0);

	WRITE_STRING(3,0,'6',0);
	WRITE_STRING(3,1,'6',0);
	WRITE_STRING(3,2,'8',0);
	WRITE_STRING(3,3,'8',0);
	WRITE_STRING(3,4,'8',0);
}
