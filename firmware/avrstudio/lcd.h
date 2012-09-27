#ifndef __LCD_H
#define __LCD_H

 #define LCD_NULL   0xff
 #define LCD_MINUS  0x00

 void LCD_ShowDigit(long val);
 void LCD_Clear();
 void LCD_ShowStr(char *p);
 void LCD_Show2Digit(short val1, short val2);
 char lcd_get_code(char dig);
void LCD_ShowTime(DWORD min, BYTE bActive);


#endif
