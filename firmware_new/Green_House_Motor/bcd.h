//***************************************************************************
//
//  File........: bcd.h
//
//  Author(s)...: Pashgan ChipEnable.Ru
//
//  Target(s)...: ��� ������ ����������������
//
//  Compiler....: �������� ��� ������ ����������� 
//
//  Description.: ���������� ��� �������� �������� ����� � ������� � ������ �� �� ��� 
//
//  Revisions...: 
//
//  18.10.2009 - 1.0 - ������                                       - Pashgan.
//
//***************************************************************************
#ifndef bcd_h
#define bcd_h

//����� ���������� ���� ���� ��� ���
#include "lcd_lib.h"
//����� �������������� ������� �� ������ ������� �� �����
//#define LCDSendData(data) LoadDataLCD(data)

/**************************************************************************
*   Function name :   Bcd1Lcd
*   Returns :         ���       
*   Parameters :      �������� ����� �� 0 �� 9    
*   Purpose :         ��������� ����� � ������ � ������� �� ���        
****************************************************************************/
void Bcd1Lcd(unsigned char value);

/**************************************************************************
*   Function name :   Bcd2Lcd
*   Returns :         ���       
*   Parameters :      �������� ����� �� 0 �� 99   
*   Purpose :         ��������� ����� � ������ � ������� �� ���        
****************************************************************************/
void Bcd2Lcd(unsigned char value);

/**************************************************************************
*   Function name :   Bcd3Lcd
*   Returns :         ���       
*   Parameters :      �������� ����� �� 0 �� 255    
*   Purpose :         ��������� ����� � ������ � ������� �� ���        
****************************************************************************/
void Bcd3Lcd(unsigned char value);

/**************************************************************************
*   Function name :   Bcd3IntLcd
*   Returns :         ���       
*   Parameters :      �������� ����� �� 0 �� 999    
*   Purpose :         ��������� ����� � ������ � ������� �� ���        
****************************************************************************/
void Bcd3IntLcd(unsigned int value);

/**************************************************************************
*   Function name :   Bcd4IntLcd
*   Returns :         ���       
*   Parameters :      �������� ����� �� 0 �� 9999    
*   Purpose :         ��������� ����� � ������ � ������� �� ���        
****************************************************************************/
void Bcd4IntLcd(unsigned int value);

/**************************************************************************
*   Function name :   Bcd5IntLcd
*   Returns :         ���       
*   Parameters :      �������� ����� �� 0 �� 65535    
*   Purpose :         ��������� ����� � ������ � ������� �� ���        
****************************************************************************/
void Bcd5IntLcd(unsigned int value);

#endif  //bcd_h




