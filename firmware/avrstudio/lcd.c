#include <avr/io.h>
#include <avr/iom88.h>
#include <avr/interrupt.h>
#include <stdio.h>

#include "main.h"
#include "lcd.h"
#include "spi.h"

#define max_lcd_code 16
unsigned char lcd_code[max_lcd_code] = {0x00,0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x0a,0x0b,0x0c,0x0d,0x0e,0x0f};
unsigned char lcd_dig[max_lcd_code]  = {" 1234567890FJLP-"};
unsigned char lcd[10];

//------------------------------------------------------------------------
//------------------------------------------------------------------------
void LCD_ShowDigit(long val)
{
 unsigned char tmp[10];
 sprintf(tmp,"%.10lu",val);
 LCD_ShowStr(tmp);
}
//------------------------------------------------------------------------
void LCD_Show2Digit(short val1, short val2)
{
 unsigned char tmp[10];

 if ((val1 == 0xffff) || (val1 > 9999))  val1 = 9999;
 if ((val2 == 0xffff) || (val2 > 9999))  val2 = 9999;
 if (val1 < -999)  val1 = -999;
 if (val2 < -999)  val2 = -999;

 if (val1 < 0)  sprintf(tmp,"%.3i  %.4i",val1, val2);
 else           sprintf(tmp,"%.4i  %.4i",val1, val2);
 LCD_ShowStr(tmp);
}
//------------------------------------------------------------------------
void LCD_Clear()
{
 char t = 5;
 while (t--)
  {
   SPI_Send(LCD_NULL);
  }
}
//------------------------------------------------------------------------
void LCD_ShowStr(char *p)
{
 unsigned char lcd[5];
 unsigned char len;
 unsigned char tmp_lcd = 0;
 signed char tmp       = 0;
 unsigned char c1, c2;

 while (p[tmp++]);
 len = --tmp;
 if (len == 0) return;

 for (tmp = len-1; tmp >= 0; tmp = tmp-2) 
  {
   c1 = lcd_get_code(p[tmp]);
   c2 = lcd_get_code(p[tmp-1]);
   if (tmp==0) c2 = 0x00;
   lcd[tmp_lcd++] = ((c2 << 4) | c1);
  }

 for (tmp = tmp_lcd - 1; tmp >= 0; tmp--)
  {
   c1 = lcd[tmp];
   SPI_Send(c1);
 }
}
//------------------------------------------------------------------------
char lcd_get_code(char dig)
{
 unsigned char t;
 for (t = 0; t < max_lcd_code; t++)
  {
   if (lcd_dig[t] == dig) return lcd_code[t]; 
  }
 return lcd_code[0]; 
}
//------------------------------------------------------------------------
void LCD_ShowTime(DWORD min, BYTE bActive)
{
  unsigned char tmp[10];
  WORD h = min/60;
  WORD m = min%60;
       m = m*10/60;

  if ((h == 0xffff) || (h > 999)) h = 999;
  if (m > 99)  m = 99;
  char c = ' ';
  if (bActive) c = '-';
//  sprintf(tmp,"%.1s %.3i-%.2i %.1s",&c, h, (BYTE)m, &c);
  sprintf(tmp,"%.1s %.6i-%.1i",&c, h, (BYTE)m);

  LCD_ShowStr(tmp);
}
//------------------------------------------------------------------------

