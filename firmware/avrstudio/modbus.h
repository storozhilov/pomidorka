#ifndef __modbus__H
#define __modbus__H

#include "defines.h"


//! Размер буфера входных данных.
#define MODBUS_MAX_RECEIVE 64

//! Значение включенного состояния катушки.
#define MODBUS_COIL_ON      0xFF00

//! Значение выключенного состояния катушки.
#define MODBUS_COIL_OFF     0x0000

//! Неизвестное состояние катушки.
#define MODBUS_COIL_ERROR   0x00FF



typedef void (FuncTimerClear)(void);
typedef void (FuncTimerStart)(void);
typedef void (FuncTimerStop)(void);

typedef void (FuncWriteReg)(WORD, WORD);
typedef WORD (FuncReadRegs)(WORD);
typedef void (FuncWriteCoil)(WORD, WORD);
typedef WORD (FuncReadCoils)(WORD);

//! Установить обработчик на сброс таймера.
void MODBUS_SetFuncTimerClear(FuncTimerClear *func);

//! Установить обработчик на старт таймера.
void MODBUS_SetFuncTimerStart(FuncTimerStart *func);

//! Установить обработчик на стоп таймера.
void MODBUS_SetFuncTimerStop(FuncTimerStop *func);

//! Установить обработчик на запись регистров.
void MODBUS_SetFuncWriteReg(FuncWriteReg *func);

//! Установить обработчик на чтение регистров.
void MODBUS_SetFuncReadRegs(FuncReadRegs *func);

//! Установить обработчик на запись значения катушек.
void MODBUS_SetFuncWriteCoil(FuncWriteCoil *func);

//! Установить обработчик на чтение значения катушек.
void MODBUS_SetFuncReadCoils(FuncReadCoils *func);



//! Инициализация моделя.
void MODBUS_Init();

//! Точка входа для данных.
//! \param b - входяший байт.
void MODBUS_Receive(BYTE b);

//! Отправляет данные мастер устройству, перед отправкой формирует CRC16.
//! \param lenData - длина данных с адресом и командой.
void MODBUS_SendAnswer(WORD lenData);

//! Отправляет в ответ ошибку.
//! \param err - код ошибки.
void MODBUS_SendException(BYTE err);

//! Отправляет эхо запрос мастер устройству.
void MODBUS_SendEcho();

//! Вызывается по спецификации, через
/*!
   (1/57600)*10*4 = 694 мкс. минимум.

   Обрабатывает поступившую команду.
*/
void MODBUS_Execute();

//! Считает контрольную сумму в modbus_CRC16.
//! \param b - байт данных для расчета.
//! \sa modbus_CRC16.
void modbus_Calculate_CRC16(BYTE b);

//! Сбрасывает подсчет CRC.
#define MODBUS_CLEAR_CRC16 modbus_CRC16 = -1;

//! Устанавливает адрес устройства.
void MODBUS_SetAddress(BYTE addr);

void MODBUS_EmulateReadRegister();




//! Команда. Записать регистры.
// 2 байта - номер регистра.
// N байт - данные.
#define MODBUS_cmd_WriteSingleRegister      0x06


//! Команда. Прочитать регистры.
#define MODBUS_cmd_ReadHoldingRegisters     0x03
// 2 байта - номер регистра.
// 1 байт - кол-во регистров.

//! Команда. Записать значение катушки.
#define MODBUS_cmd_WriteSingleCoil          0x05

//! Команда. Записать значение катушки.
#define MODBUS_cmd_ReadCoils                0x01

#endif
