/**
  ******************************************************************************
  * @file    ssd1306.c
  * @author  YANDLD
  * @date    2015.12.25
  * @brief   www.beyondcore.net   http://upcmcu.taobao.com 
  ******************************************************************************
  */

#include <string.h>
#include "ssd1306.h"
#include "gpio.h"
#include "common.h"

#ifndef ARRAY_SIZE
#define ARRAY_SIZE(x)	(sizeof(x) / sizeof((x)[0]))
#endif

#define SSD1306_ADDR        (0x3C)


#define OLED_SCLK_Pin    PTB21     //D0
#define OLED_SDIN_Pin    PTB23      //D1
#define OLED_RST_Pin     PTC1     //RST
#define OLED_DC_Pin      PTC3      //DC

/************************************6*8�ĵ���************************************/
const unsigned char F6x8[][6] =
{
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00,// sp
	0x00, 0x00, 0x00, 0x2f, 0x00, 0x00,// !
	0x00, 0x00, 0x07, 0x00, 0x07, 0x00,// "
	0x00, 0x14, 0x7f, 0x14, 0x7f, 0x14,// #
	0x00, 0x24, 0x2a, 0x7f, 0x2a, 0x12,// $
	0x00, 0x62, 0x64, 0x08, 0x13, 0x23,// %
	0x00, 0x36, 0x49, 0x55, 0x22, 0x50,// &
	0x00, 0x00, 0x05, 0x03, 0x00, 0x00,// '
	0x00, 0x00, 0x1c, 0x22, 0x41, 0x00,// (
	0x00, 0x00, 0x41, 0x22, 0x1c, 0x00,// )
	0x00, 0x14, 0x08, 0x3E, 0x08, 0x14,// *
	0x00, 0x08, 0x08, 0x3E, 0x08, 0x08,// +
	0x00, 0x00, 0x00, 0xA0, 0x60, 0x00,// ,
	0x00, 0x08, 0x08, 0x08, 0x08, 0x08,// -
	0x00, 0x00, 0x60, 0x60, 0x00, 0x00,// .
	0x00, 0x20, 0x10, 0x08, 0x04, 0x02,// /
	0x00, 0x3E, 0x51, 0x49, 0x45, 0x3E,// 0
	0x00, 0x00, 0x42, 0x7F, 0x40, 0x00,// 1
	0x00, 0x42, 0x61, 0x51, 0x49, 0x46,// 2
	0x00, 0x21, 0x41, 0x45, 0x4B, 0x31,// 3
	0x00, 0x18, 0x14, 0x12, 0x7F, 0x10,// 4
	0x00, 0x27, 0x45, 0x45, 0x45, 0x39,// 5
	0x00, 0x3C, 0x4A, 0x49, 0x49, 0x30,// 6
	0x00, 0x01, 0x71, 0x09, 0x05, 0x03,// 7
	0x00, 0x36, 0x49, 0x49, 0x49, 0x36,// 8
	0x00, 0x06, 0x49, 0x49, 0x29, 0x1E,// 9
	0x00, 0x00, 0x36, 0x36, 0x00, 0x00,// :
	0x00, 0x00, 0x56, 0x36, 0x00, 0x00,// ;
	0x00, 0x08, 0x14, 0x22, 0x41, 0x00,// <
	0x00, 0x14, 0x14, 0x14, 0x14, 0x14,// =
	0x00, 0x00, 0x41, 0x22, 0x14, 0x08,// >
	0x00, 0x02, 0x01, 0x51, 0x09, 0x06,// ?
	0x00, 0x32, 0x49, 0x59, 0x51, 0x3E,// @
	0x00, 0x7C, 0x12, 0x11, 0x12, 0x7C,// A
	0x00, 0x7F, 0x49, 0x49, 0x49, 0x36,// B
	0x00, 0x3E, 0x41, 0x41, 0x41, 0x22,// C
	0x00, 0x7F, 0x41, 0x41, 0x22, 0x1C,// D
	0x00, 0x7F, 0x49, 0x49, 0x49, 0x41,// E
	0x00, 0x7F, 0x09, 0x09, 0x09, 0x01,// F
	0x00, 0x3E, 0x41, 0x49, 0x49, 0x7A,// G
	0x00, 0x7F, 0x08, 0x08, 0x08, 0x7F,// H
	0x00, 0x00, 0x41, 0x7F, 0x41, 0x00,// I
	0x00, 0x20, 0x40, 0x41, 0x3F, 0x01,// J
	0x00, 0x7F, 0x08, 0x14, 0x22, 0x41,// K
	0x00, 0x7F, 0x40, 0x40, 0x40, 0x40,// L
	0x00, 0x7F, 0x02, 0x0C, 0x02, 0x7F,// M
	0x00, 0x7F, 0x04, 0x08, 0x10, 0x7F,// N
	0x00, 0x3E, 0x41, 0x41, 0x41, 0x3E,// O
	0x00, 0x7F, 0x09, 0x09, 0x09, 0x06,// P
	0x00, 0x3E, 0x41, 0x51, 0x21, 0x5E,// Q
	0x00, 0x7F, 0x09, 0x19, 0x29, 0x46,// R
	0x00, 0x46, 0x49, 0x49, 0x49, 0x31,// S
	0x00, 0x01, 0x01, 0x7F, 0x01, 0x01,// T
	0x00, 0x3F, 0x40, 0x40, 0x40, 0x3F,// U
	0x00, 0x1F, 0x20, 0x40, 0x20, 0x1F,// V
	0x00, 0x3F, 0x40, 0x38, 0x40, 0x3F,// W
	0x00, 0x63, 0x14, 0x08, 0x14, 0x63,// X
	0x00, 0x07, 0x08, 0x70, 0x08, 0x07,// Y
	0x00, 0x61, 0x51, 0x49, 0x45, 0x43,// Z
	0x00, 0x00, 0x7F, 0x41, 0x41, 0x00,// [
	0x00, 0x55, 0x2A, 0x55, 0x2A, 0x55,// 55
	0x00, 0x00, 0x41, 0x41, 0x7F, 0x00,// ]
	0x00, 0x04, 0x02, 0x01, 0x02, 0x04,// ^
	0x00, 0x40, 0x40, 0x40, 0x40, 0x40,// _
	0x00, 0x00, 0x01, 0x02, 0x04, 0x00,// '
	0x00, 0x20, 0x54, 0x54, 0x54, 0x78,// a
	0x00, 0x7F, 0x48, 0x44, 0x44, 0x38,// b
	0x00, 0x38, 0x44, 0x44, 0x44, 0x20,// c
	0x00, 0x38, 0x44, 0x44, 0x48, 0x7F,// d
	0x00, 0x38, 0x54, 0x54, 0x54, 0x18,// e
	0x00, 0x08, 0x7E, 0x09, 0x01, 0x02,// f
	0x00, 0x18, 0xA4, 0xA4, 0xA4, 0x7C,// g
	0x00, 0x7F, 0x08, 0x04, 0x04, 0x78,// h
	0x00, 0x00, 0x44, 0x7D, 0x40, 0x00,// i
	0x00, 0x40, 0x80, 0x84, 0x7D, 0x00,// j
	0x00, 0x7F, 0x10, 0x28, 0x44, 0x00,// k
	0x00, 0x00, 0x41, 0x7F, 0x40, 0x00,// l
	0x00, 0x7C, 0x04, 0x18, 0x04, 0x78,// m
	0x00, 0x7C, 0x08, 0x04, 0x04, 0x78,// n
	0x00, 0x38, 0x44, 0x44, 0x44, 0x38,// o
	0x00, 0xFC, 0x24, 0x24, 0x24, 0x18,// p
	0x00, 0x18, 0x24, 0x24, 0x18, 0xFC,// q
	0x00, 0x7C, 0x08, 0x04, 0x04, 0x08,// r
	0x00, 0x48, 0x54, 0x54, 0x54, 0x20,// s
	0x00, 0x04, 0x3F, 0x44, 0x40, 0x20,// t
	0x00, 0x3C, 0x40, 0x40, 0x20, 0x7C,// u
	0x00, 0x1C, 0x20, 0x40, 0x20, 0x1C,// v
	0x00, 0x3C, 0x40, 0x30, 0x40, 0x3C,// w
	0x00, 0x44, 0x28, 0x10, 0x28, 0x44,// x
	0x00, 0x1C, 0xA0, 0xA0, 0xA0, 0x7C,// y
	0x00, 0x44, 0x64, 0x54, 0x4C, 0x44,// z
	0x14, 0x14, 0x14, 0x14, 0x14, 0x14,// horiz lines
};

/***************************16*16�ĵ�������ȡģ��ʽ��������������ʽ�����������*********/
unsigned char F16x16[] =
{
    0x40,0x48,0x48,0x48,0xFF,0x48,0x48,0x42,0xA2,0x9E,0x82,0xA2,0xC2,0xBE,0x00,0x00,
    0x80,0x60,0x1F,0x20,0x7F,0x44,0x44,0x40,0x4F,0x48,0x48,0x48,0x48,0x4F,0x40,0x00,//?0

    0x10,0x10,0xD0,0xFF,0x90,0x10,0x08,0x88,0xC8,0xA8,0x99,0x8E,0x88,0x48,0x28,0x00,
    0x04,0x03,0x00,0xFF,0x00,0x03,0x88,0x88,0x44,0x44,0x22,0x11,0x28,0x44,0x82,0x00,//?1

    0x24,0x24,0xA4,0xFE,0xA3,0x22,0x00,0x22,0xCC,0x00,0x00,0xFF,0x00,0x00,0x00,0x00,
    0x08,0x06,0x01,0xFF,0x00,0x01,0x04,0x04,0x04,0x04,0x04,0xFF,0x02,0x02,0x02,0x00,//?2

    0x10,0x10,0x10,0xFF,0x10,0x90,0x08,0x88,0x88,0x88,0xFF,0x88,0x88,0x88,0x08,0x00,
    0x04,0x44,0x82,0x7F,0x01,0x80,0x80,0x40,0x43,0x2C,0x10,0x28,0x46,0x81,0x80,0x00,//?3
};

 		     


//GPIO_WriteBit(HW_GPIOB, 11, 1);
static inline void WriteCmd(unsigned char cmd)//д����
{
     GPIO_WriteBit(HW_GPIOC, 1, 0);
    //I2C_BurstWrite(0, SSD1306_ADDR, 0x00, 1, &cmd, 1);
    //OLED_CS_Clr();
  for(int i=0;i<8;i++)
  {
    //PTXn_T(OLED_SCLK_Pin,OUT)=0
      GPIO_WriteBit(HW_GPIOC, 3, 0);
    if(cmd&0x80)
      //OLED_SDIN_Set();
    GPIO_WriteBit(HW_GPIOC, 2, 1);
    else 
      //OLED_SDIN_Clr();
    GPIO_WriteBit(HW_GPIOC, 2, 0);
    //OLED_SCLK_Set();
    GPIO_WriteBit(HW_GPIOC, 3, 1);
    cmd<<=1;   
  }				 		  
  //OLED_CS_Set();
  //OLED_DC_Set();   
GPIO_WriteBit(HW_GPIOC, 1, 1);  
}
static inline void WriteDat(unsigned char data)//д����
{
    GPIO_WriteBit(HW_GPIOC, 1, 1);
    //I2C_BurstWrite(0, SSD1306_ADDR, 0x00, 1, &cmd, 1);
    //OLED_CS_Clr();
  for(int i=0;i<8;i++)
  {
    //PTXn_T(OLED_SCLK_Pin,OUT)=0
      GPIO_WriteBit(HW_GPIOC, 3, 0);
    if(data&0x80)
      //OLED_SDIN_Set();
    GPIO_WriteBit(HW_GPIOC, 2, 1);
    else 
      //OLED_SDIN_Clr();
    GPIO_WriteBit(HW_GPIOC, 2, 0);
    //OLED_SCLK_Set();
    GPIO_WriteBit(HW_GPIOC, 3, 1);
    data<<=1;   
  }				 		  
  //OLED_CS_Set();
  //OLED_DC_Set();    
GPIO_WriteBit(HW_GPIOC, 1, 1);  
}



void ssd1306_init(void)
{
     GPIO_QuickInit(HW_GPIOC, 0, kGPIO_Mode_OPP);
     GPIO_QuickInit(HW_GPIOC, 1, kGPIO_Mode_OPP);
     GPIO_QuickInit(HW_GPIOC, 2, kGPIO_Mode_OPP);
     GPIO_QuickInit(HW_GPIOC, 3, kGPIO_Mode_OPP);
    
    
    GPIO_WriteBit(HW_GPIOC, 0, 1); 
 DelayMs(100);
    GPIO_WriteBit(HW_GPIOC, 0, 0); 
 DelayMs(200);
    GPIO_WriteBit(HW_GPIOC, 0, 1);  

	//DelayMs(100); //�������ʱ����Ҫ
	
    WriteCmd(0xAE); //display off
	WriteCmd(0x20);	//Set Memory Addressing Mode	
	WriteCmd(0x10);	//00,Horizontal Addressing Mode;01,Vertical Addressing Mode;10,Page Addressing Mode (RESET);11,Invalid
	WriteCmd(0xb0);	//Set Page Start Address for Page Addressing Mode,0-7
	WriteCmd(0xc8);	//Set COM Output Scan Direction
	WriteCmd(0x00); //---set low column address
	WriteCmd(0x10); //---set high column address
	WriteCmd(0x40); //--set start line address
	WriteCmd(0x81); //--set contrast control register
	WriteCmd(0xff); //���ȵ��� 0x00~0xff
	WriteCmd(0xa1); //--set segment re-map 0 to 127
	WriteCmd(0xa6); //--set normal display
	WriteCmd(0xa8); //--set multiplex ratio(1 to 64)
	WriteCmd(0x3F); //
	WriteCmd(0xa4); //0xa4,Output follows RAM content;0xa5,Output ignores RAM content
	WriteCmd(0xd3); //-set display offset
	WriteCmd(0x00); //-not offset
	WriteCmd(0xd5); //--set display clock divide ratio/oscillator frequency
	WriteCmd(0xf0); //--set divide ratio
	WriteCmd(0xd9); //--set pre-charge period
	WriteCmd(0x22); //
	WriteCmd(0xda); //--set com pins hardware configuration
	WriteCmd(0x12);
	WriteCmd(0xdb); //--set vcomh
	WriteCmd(0x20); //0x20,0.77xVcc
	WriteCmd(0x8d); //--set DC-DC enable
	WriteCmd(0x14); //
	WriteCmd(0xaf); //--turn on oled panel
    
    OLED_Fill(0x00);
}


void OLED_Fill(unsigned char dat)
{
    int i;
    unsigned char m;
    for(m=0;m<8;m++)
    {
        WriteCmd(0xb0+m);		//page0-page1
		WriteCmd(0x00);		//low column start address
        WriteCmd(0x10);		//high column start address
        for(i=0; i<128; i++)
        {
            WriteDat(dat); 
            
        }
        //I2C_BurstWrite(0,SSD1306_ADDR, 0x40, 1, buf, sizeof(buf));
	}
}

void OLED_SetPos(unsigned char x, unsigned char y) //������ʼ������
{ 
	WriteCmd(0xb0+y);
	WriteCmd(((x&0xf0)>>4)|0x10);
	WriteCmd((x&0x0f)|0x01);
}



void OLED_Enable(void)
{
	WriteCmd(0X8D);  //���õ�ɱ�
	WriteCmd(0X14);  //������ɱ�
	WriteCmd(0XAF);  //OLED����
}


void OLED_Disable(void)
{
	WriteCmd(0X8D);  //���õ�ɱ�
	WriteCmd(0X10);  //�رյ�ɱ�
	WriteCmd(0XAE);  //OLED����
}

void OLED_ShowStr(unsigned char x, unsigned char y, char *str)
{
	unsigned char c = 0,i = 0,j = 0;
    while(str[j] != '\0')
    {
        c = str[j] - 32;
        if(x > 126)
        {
            x = 0;
            y++;
        }
        OLED_SetPos(x,y);
        for(i=0;i<6;i++)
            WriteDat(F6x8[c][i]);
        x += 6;
        j++;
    }
}

void OLED_ShowCN(unsigned char x, unsigned char y, unsigned char N)
{
	unsigned char wm=0;
	unsigned int  adder=32*N;
	OLED_SetPos(x , y);
	for(wm = 0;wm < 16;wm++)
	{
		WriteDat(F16x16[adder]);
		adder += 1;
	}
	OLED_SetPos(x,y + 1);
	for(wm = 0;wm < 16;wm++)
	{
		WriteDat(F16x16[adder]);
		adder += 1;
	}
}
//void OLED_DrawBMP(int xSize, int ySize, uint8_t** ppData)
   void OLED_DrawBMP(unsigned char x0,unsigned char y0,unsigned char high,unsigned char width,unsigned char *p)
{
	    unsigned int i,j;
    unsigned int temp,temp1;
    unsigned char dat;
    
    
    temp1 = high%8;
    if(temp1 == 0) temp = high/8;
    else           temp = high/8+1;
    
 
    for(i=0; i<temp; i++)
    {
        OLED_SetPos(0,i);
        for(j=0; j<width; j++)
        {
            dat = 0;
            if( i<(temp-1) || !temp1 || temp1>=1)dat |= (*(p+i*8*width+j+width*0) > 0? 1: 0)<<0;
            if( i<(temp-1) || !temp1 || temp1>=2)dat |= (*(p+i*8*width+j+width*1) > 0? 1: 0)<<1;
            if( i<(temp-1) || !temp1 || temp1>=3)dat |= (*(p+i*8*width+j+width*2) > 0? 1: 0)<<2;
            if( i<(temp-1) || !temp1 || temp1>=4)dat |= (*(p+i*8*width+j+width*3) > 0? 1: 0)<<3;
            if( i<(temp-1) || !temp1 || temp1>=5)dat |= (*(p+i*8*width+j+width*4) > 0? 1: 0)<<4;
            if( i<(temp-1) || !temp1 || temp1>=6)dat |= (*(p+i*8*width+j+width*5) > 0? 1: 0)<<5;
            if( i<(temp-1) || !temp1 || temp1>=7)dat |= (*(p+i*8*width+j+width*6) > 0? 1: 0)<<6;
            if( i<(temp-1) || !temp1 || temp1>=8)dat |= (*(p+i*8*width+j+width*7) > 0? 1: 0)<<7;
            WriteDat(dat);
        }
    }
    /*unsigned int i,j;
    unsigned char temp,temp1,y3;
    unsigned char dat;
    
    y3=y1/8;
    temp1 = x1%8;
    if(temp1 == 0) temp = x1/8;
    else           temp = x1/8+1;
    
 
    for(i=0; i<temp; i++)
    {
        OLED_SetPos(0,i);
        for(j=0; j<y1; j++)
        {
            dat = 0;
            if( i<(temp-1) || !temp1 || temp1>=1)dat |= (BMP[(i*8+0)*y3+j/8+1]&(1<<(j%8))?1:0)<<0;
            if( i<(temp-1) || !temp1 || temp1>=2)dat |= (BMP[(i*8+1)*y3+j/8+1]&(1<<(j%8))?1:0)<<1;
            if( i<(temp-1) || !temp1 || temp1>=3)dat |= (BMP[(i*8+2)*y3+j/8+1]&(1<<(j%8))?1:0)<<2;
            if( i<(temp-1) || !temp1 || temp1>=4)dat |= (BMP[(i*8+3)*y3+j/8+1]&(1<<(j%8))?1:0)<<3;
            if( i<(temp-1) || !temp1 || temp1>=5)dat |= (BMP[(i*8+4)*y3+j/8+1]&(1<<(j%8))?1:0)<<4;
            if( i<(temp-1) || !temp1 || temp1>=6)dat |= (BMP[(i*8+5)*y3+j/8+1]&(1<<(j%8))?1:0)<<5;
            if( i<(temp-1) || !temp1 || temp1>=7)dat |= (BMP[(i*8+6)*y3+j/8+1]&(1<<(j%8))?1:0)<<6;
            if( i<(temp-1) || !temp1 || temp1>=8)dat |= (BMP[(i*8+7)*y3+j/8+1]&(1<<(j%8))?1:0)<<7;
           WriteDat(dat);
        }
    }*/
    /*
    unsigned int j=0;
	unsigned char x,y;

  if(y1%8==0)
		y1 = y1/8;
  else
		y1 = y1/8 + 1;
	for(x=x0;x<x1;x++)
	{
		
    for(y=y0;y<y1;y++)
		{
            
            OLED_SetPos(x,y1-y);
			WriteDat(BMP[j++]);
		}
	}
   */
}

