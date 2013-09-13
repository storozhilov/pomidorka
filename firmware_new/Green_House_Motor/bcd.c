//***************************************************************************
//
//  File........: bcd.�
//
//  Author(s)...: Pashgan ChipEnable.Ru
//
//  Target(s)...: ��� ������ ����������������
//
//  Compiler....: ��� ������ ����������� 
//
//  Description.: ���������� ��� �������� �������� ����� � ������� � ������ �� �� ��� 
//
//  Revisions...: 
//
//  18.10.2009 - 1.0 - ������                                       - Pashgan.
//
//***************************************************************************
#include "bcd.h"

/**************************************************************************
*   Function name :   Bcd1Lcd
*   Returns :         ���       
*   Parameters :      �������� ����� �� 0 �� 9    
*   Purpose :         ��������� ����� � ������ � ������� �� ���        
****************************************************************************/
void Bcd1Lcd(unsigned char value)
{
    value += 48;
    LCD_WriteData(value);  
}

/**************************************************************************
*   Function name :   Bcd2Lcd
*   Returns :         ���       
*   Parameters :      �������� ����� �� 0 �� 99   
*   Purpose :         ��������� ����� � ������ � ������� �� ���        
****************************************************************************/
void Bcd2Lcd(unsigned char value)
{
    unsigned char high = 0;
    
    while (value >= 10)                 
    {
        high++;
        value -= 10;
    }
    if (high) high += 48;
    else high = 32;
    LCD_WriteData(high);

    value += 48;
    LCD_WriteData(value);  
}

/**************************************************************************
*   Function name :   Bcd3Lcd
*   Returns :         ���       
*   Parameters :      �������� ����� �� 0 �� 255    
*   Purpose :         ��������� ����� � ������ � ������� �� ���        
****************************************************************************/
void Bcd3Lcd(unsigned char value)
{
    unsigned char high = 0;
    unsigned char flag = 0;
   
    if (value >= 100) flag = 48;   
    else flag = 32;
    while (value >= 100)               
    {
        high++;
        value -= 100;
    }
    if (high) high += 48;
    else high = 32;
    LCD_WriteData(high );
    
    high = 0;
    while (value >= 10)               
    {
        high++;
        value -= 10;
    }
    if (high) high += 48;
    else high = flag;
    LCD_WriteData(high );
    
    value += 48;
    LCD_WriteData(value);  
}

/**************************************************************************
*   Function name :   Bcd3IntLcd
*   Returns :         ���       
*   Parameters :      �������� ����� �� 0 �� 999    
*   Purpose :         ��������� ����� � ������ � ������� �� ���        
****************************************************************************/
void Bcd3IntLcd(unsigned int value)
{
    unsigned char high = 0;
    unsigned char flag = 0;
   
    if (value >= 100) flag = 48;   
    else flag = 32;
    while (value >= 100)                
    {
        high++;
        value -= 100;
    }
    if (high) high += 48;
    else high = 32;
    LCD_WriteData(high );
    
    high = 0;
    while (value >= 10)                 
    {
        high++;
        value -= 10;
    }
    if (high) high += 48;
    else high = flag;
    LCD_WriteData(high );
    
    value += 48;
    LCD_WriteData(value);  
}

/**************************************************************************
*   Function name :   Bcd4IntLcd
*   Returns :         ���       
*   Parameters :      �������� ����� �� 0 �� 9999    
*   Purpose :         ��������� ����� � ������ � ������� �� ���        
****************************************************************************/
void Bcd4IntLcd(unsigned int value)
{
    unsigned char high = 0;
    unsigned char flag = 0;
    unsigned char flag2 = 0;  
    
    if (value >= 1000) {flag = 48; flag2 = 48;}  
    else
    {
      if (value >= 100) {flag = 32; flag2 = 48;}
      else {flag = 32; flag2 = 32;}
    }
    
    while (value >= 1000)                // Count thousand
    {
        high++;
        value -= 1000;
    }
    if (high) high += 48;
    else high = 32;
    LCD_WriteData(high);
    
    high = 0;
    while (value >= 100)                
    {
        high++;
        value -= 100;
    }
    if (high) high += 48;
    else high = flag;
    LCD_WriteData(high );
    
    
    high = 0;
    while (value >= 10)                 
    {
        high++;
        value -= 10;
    }
    if (high) high += 48;
    else high = flag2;
    LCD_WriteData(high );
    
    value += 48;
    LCD_WriteData(value);  
}

/**************************************************************************
*   Function name :   Bcd5IntLcd
*   Returns :         ���       
*   Parameters :      �������� ����� �� 0 �� 65535    
*   Purpose :         ��������� ����� � ������ � ������� �� ���        
****************************************************************************/
void Bcd5IntLcd(unsigned int value)
{
    unsigned char high = 0;
    unsigned char flag = 0;
    unsigned char flag2 = 0;
	unsigned char flag3 = 0;  
    
    if (value >= 10000) {flag = 48; flag2 = 48; flag3 = 48;}  
    else
    {
      if (value >= 1000) {flag = 32; flag2 = 48; flag3 = 48;}
      else {flag = 32; flag2 = 32; flag3 = 48;}
    }
    
    while (value >= 10000)               
    {
        high++;
        value -= 10000;
    }
    if (high) high += 48;
    else high = 32;
    LCD_WriteData(high);
    
    high = 0;	


    while (value >= 1000)                // Count thousand
    {
        high++;
        value -= 1000;
    }
    if (high) high += 48;
    else high = flag;
    LCD_WriteData(high);
    
    high = 0;
    while (value >= 100)                
    {
        high++;
        value -= 100;
    }
    if (high) high += 48;
    else high = flag2;
    LCD_WriteData(high );
    
    
    high = 0;
    while (value >= 10)                 
    {
        high++;
        value -= 10;
    }
    if (high) high += 48;
    else high = flag3;
    LCD_WriteData(high );
    
    value += 48;
    LCD_WriteData(value);  
}


