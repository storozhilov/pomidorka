#ifndef __24LC64_H
#define __24LC64_H

#include "defines.h"


//#define MaxSize           8192/Unit


 BYTE EE_ReadPage(BYTE NumPage);
 BYTE EE_WritePage(BYTE NumPage);
 unsigned char EE_WriteValue(unsigned short Offzet);

 BYTE EE_ReadConfig();
 BYTE EE_WriteConfig();



#define EEAdressRead        0xA1
#define EEAdressWrite       0xA0


struct _EE_ 
 {
   BYTE   AddrH;
   BYTE   AddrL;
   DWORD   EE_TotalGerkon;        // ����� ��������� 
   DWORD   EE_TotalHour;          // ����� �����, ���
   WORD    EE_LocalGerkon;        // ��������� �� �����
   WORD    EE_LocalHour;          // ����� �� �����, ���
   BYTE    EE_Adress;             // ����� ����������
   BYTE    EE_MAX_DrebezgGerkon;  // ������� �������    
   BYTE    EE_MAX_TimeoutGerkon;  // ������� �������, ���  
   BYTE    EE_MAX_ThermoValue;    // ������������ ����������� ���������
   BYTE    EE_ParameterMask;      // ����� ����������
   BYTE    EE_lcdParameter;       // ������� ���. ���������� �� ������
   DWORD   EE_TotalStartCount;    // ����� ���-�� ��������� ������
   WORD    EE_LocalStartCount;    // ���-�� ��������� ������ �� �����
   DWORD   EE_TotalRewriteEE;     // ���-�� ���������� EEPROM�
 } EE_24lc64;                // 2 + 24 < UART_MAX_RECEIVE_BUFFER

//#if sizeof(struct _EE_) > UART_MAX_RECEIVE_BUFFER
// #error Size Buffer Error !!! (EE_24lc64 > UART_MAX_RECEIVE_BUFFER)
//#endif
 
 #define EE_MSK_ThermoEnable   (1 << 0) // ������� �� ������������?
 #define EE_MSK_GerkonEnable   (1 << 1) // ������� �� ��������?
 #define EE_MSK_ThermoDS       (1 << 2) // �������� ��������� DS, ����� ADC
 #define EE_MSK_Lock           (1 << 3) // ������ ������������
 #define EE_MSK_LockOfThermo   (1 << 4) // ������ �������� ������������ �� ������� ���������� ������� �����������
 #define EE_MSK_LockOfGerkon   (1 << 5) // ������ ��� ���������� �� �������� �������


 BYTE EE_PAGE[df_EE_PAGE_SIZE + 2];

 #define max_lcd_parameter 3
 enum lcdParam {lcdDefault, lcdThermo_LocalGerkon, lcdTotalGerkon, lcdTotalHour};

#endif
