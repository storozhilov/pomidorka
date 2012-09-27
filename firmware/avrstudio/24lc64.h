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
   DWORD   EE_TotalGerkon;        // Общая наработка 
   DWORD   EE_TotalHour;          // Общее время, мин
   WORD    EE_LocalGerkon;        // Наработка за смену
   WORD    EE_LocalHour;          // Время за смену, мин
   BYTE    EE_Adress;             // Адрес устройства
   BYTE    EE_MAX_DrebezgGerkon;  // Дребезг геркона    
   BYTE    EE_MAX_TimeoutGerkon;  // Таймаут геркона, сек  
   BYTE    EE_MAX_ThermoValue;    // Максимальная температура двигателя
   BYTE    EE_ParameterMask;      // Маска параметров
   BYTE    EE_lcdParameter;       // Наличие доп. параметров на экране
   DWORD   EE_TotalStartCount;    // Общее кол-во включений станка
   WORD    EE_LocalStartCount;    // Кол-во включений станка за смену
   DWORD   EE_TotalRewriteEE;     // Кол-во перезаписи EEPROMа
 } EE_24lc64;                // 2 + 24 < UART_MAX_RECEIVE_BUFFER

//#if sizeof(struct _EE_) > UART_MAX_RECEIVE_BUFFER
// #error Size Buffer Error !!! (EE_24lc64 > UART_MAX_RECEIVE_BUFFER)
//#endif
 
 #define EE_MSK_ThermoEnable   (1 << 0) // Следить за температурой?
 #define EE_MSK_GerkonEnable   (1 << 1) // Следить за Герконом?
 #define EE_MSK_ThermoDS       (1 << 2) // Активный термометр DS, иначе ADC
 #define EE_MSK_Lock           (1 << 3) // Станок заблокирован
 #define EE_MSK_LockOfThermo   (1 << 4) // Станок временно заблокирован по причине превышения рабочей температуры
 #define EE_MSK_LockOfGerkon   (1 << 5) // Станок был остановлен по Таймауту Геркона


 BYTE EE_PAGE[df_EE_PAGE_SIZE + 2];

 #define max_lcd_parameter 3
 enum lcdParam {lcdDefault, lcdThermo_LocalGerkon, lcdTotalGerkon, lcdTotalHour};

#endif
