#ifndef __LOAD_H
#define __LOAD_H

#include "defines.h"

/*!
    Модуль управлением нагрузками.
    Нагрузка 1 умеет автоматически выключаться по истечении заданного времени, а так-же по сигналу от двух концевиков.
    Нагрузка 2 умеет работать в режиме ШИМ.
*/

//! Включение нагрузки 1.
#define LOAD_1_ON               LOAD1_ON
//! Выключение нагрузки 1.
#define LOAD_1_OFF              LOAD1_OFF
//! Обратное подключение нагрузки 1.
#define LOAD_1_DIRECTION_ON     LOAD1_DIRECTION_ON
//! Прямое подключение нагрузки 1.
#define LOAD_1_DIRECTION_OFF    LOAD1_DIRECTION_OFF
//! Состояние нагрузки 1.
#define LOAD_1_STATUS           LOAD1_STATUS

//! Включение нагрузки 2.
#define LOAD_2_ON               LOAD2_ON
//! Выключение нагрузки 2.
#define LOAD_2_OFF              LOAD2_OFF
//! Состояние нагрузки 2.
#define LOAD_2_STATUS           LOAD2_STATUS




//! Модуль управления нагрузкой.
void LOAD_Init();

//! Включение нагрузки 1.
//! \param tick_100mls - максимальное время включения, 1=100 mls, 0 = всегда.
//! \sa LOAD_Timer_Interrupt_100_mls.
void LOAD_1_On(WORD tick_100mls);

//! Выключение нагрузки 1.
void LOAD_1_Off();

//! Установить направление включение нагрузки 1.
//! \param bReverse, 0 = прямое подключение нагрузки, 1 = инвертированное подключение нагрузки.
void LOAD_1_SetDirection(BYTE bReverse);


//! Возвращает текущее состояние нагрузки 1.
//! \retval >= 1 - нагрузка включена.
//! \retval = 0 - нагрузка выключена.
BYTE LOAD_1_IsOn();

//! Включение нагрузки 2 в режиме ШИМ.
//! \param on_100mls - время включения нагрузки, 1=100 mls.
//! \param off_100mls - время выключения нагрузки, 1=100 mls.
//! \sa LOAD_Timer_Interrupt_100_mls.
void LOAD_2_On(WORD on_100mls, WORD off_100mls);

//! Выключение нагрузки 1.
void LOAD_2_Off();

//! Возвращает текущее состояние нагрузки 2.
//! \retval >= 1 - нагрузка включена.
//! \retval = 0 - нагрузка выключена.
void LOAD_2_IsOn();



//! Точка входа прерывания таймера, должен входить каждые 100 млс.
void LOAD_Timer_Interrupt_100_mls();


#endif

