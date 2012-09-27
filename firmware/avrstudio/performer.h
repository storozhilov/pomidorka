#ifndef __PERFORMER_H
#define __PERFORMER_H

#include "defines.h"

//! Стартовый номер регистра.
#define PERFORMER_START_REG 100

//! Кол-во регистров.
#define PERFORMER_MAX_REGS 16

//! Кол-во бит-катушек.
#define PERFORMER_MAX_COILS 8

//! Инициализация исполнительного модуля.
void PERFORMER_Init();

//! Прочитать регистр.
//! \param addr - номер регистра.
//! \return значение регистра.
WORD PERFORMER_ReadRegister(WORD addr);

//! Записать значения регистров.
//! \param addr - адрес регистра.
//! \param reg -  значение регистра.
void PERFORMER_WriteRegister(WORD addr, WORD reg);


//! Прочитать значение катушки.
//! \param addr - номер катушки.
//! \return значение катушки.
//! \retval MODBUS_COIL_ON - Включена.
//! \retval MODBUS_COIL_OFF - Выключена.
//! \sa MODBUS_COIL_ON
//! \sa MODBUS_COIL_OFF
WORD PERFORMER_ReadCoils(WORD addr);

//! Записать состояние катушки.
//! \param addr - адрес катушки.
//! \param value -  значение катушки.
void PERFORMER_WriteCoil(WORD addr, WORD value);



//! Исполнитель
void PERFORMER_Execute();



#endif
