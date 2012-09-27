#ifndef __DS18B20_H
#define __DS18B20_H

#include "defines.h"

//! Инициализация модуля.
void DS18B20_Init();

//! Код семейства и коды команд датчика DS18B20
#define DS18B20_FAMILY_ID                0x28
#define DS18B20_CONVERT_T                0x44
#define DS18B20_READ_SCRATCHPAD          0xbe
#define DS18B20_WRITE_SCRATCHPAD         0x4e
#define DS18B20_COPY_SCRATCHPAD          0x48
#define DS18B20_RECALL_E                 0xb8
#define DS18B20_READ_POWER_SUPPLY        0xb4

//! Вывод, к которому подключены 1Wire устройства
#define BUS   OWI_PIN_2

//! Точка входа для чтения и запоминанния температуры датчиков.
void DS18B20_Execute();

//! Возвращает последнее действительное значение температуры для датчика номер index.
//! \param index - номер датчика температуры.
//! \sa MAX_DEVICES
WORD DS18B20_GetTemparature(BYTE index);

//! Макс. количество устройств на шине 1Wire.
#define MAX_DEVICES       2

//! Коды ошибок для функции чтения температуры
#define READ_SUCCESSFUL   0x00
#define READ_CRC_ERROR    0x01

#define SEARCH_SENSORS 0x00
#define SENSORS_FOUND 0xff


#endif
