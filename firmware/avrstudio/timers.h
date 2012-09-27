#ifndef __TIMERS_H
#define __TIMERS_H


//-----------------------------------------------------------------
//! Инициализация таймера 0.
void T0_Init();

//! Старт таймер 0.
#define T0_START   TCCR0B = (1 << CS02) | (0 << CS01) | (0 << CS00);     // CK/256       4444 мкс
//! Стоп таймер 0.
#define T0_STOP    TCCR0B = 0x00;
//! Сброс таймер 0.
#define T0_CLEAR   TCNT0  = 0x00;

void T0_Start();
void T0_Stop();
void T0_Clear();
//-----------------------------------------------------------------
//! Инициализация таймера 1. Настраиваем таймер 1 на прерывание каждые 100 млс.
void T1_Init();

//! Старт таймер 1. Режим CTC.
#define T1_START   TCCR1B = (1 << WGM12) | (0 << CS12) | (1 << CS11) | (1 << CS10);     // CK/64
//! Стоп таймер 1.
#define T1_STOP    TCCR1B = 0x00;
//! Сброс таймер 1.
#define T1_CLEAR   TCNT1  = 0x00;

void T1_Start();
void T1_Stop();
void T1_Clear();
//-----------------------------------------------------------------
//! Инициализация таймера 2.
void T2_Init();

//! Старт таймер 2.
#define T2_START   TCCR2B = (1 << CS22) | (1 << CS21) | (1 << CS20);     // CK/1024     13 млс
//! Стоп таймер 2.
#define T2_STOP    TCCR2B = 0x00;
//! Сброс таймер 2.
#define T2_CLEAR   TCNT2  = 0x00;

void T2_Start();
void T2_Stop();
void T2_Clear();
//-----------------------------------------------------------------

typedef void (FuncTimerOverflow)(void);

//! Установить обработчик переполнения таймера 0
void T0_SetFuncOverflow(FuncTimerOverflow *func);
//! Установить обработчик переполнения таймера 1
void T1_SetFuncOverflow(FuncTimerOverflow *func);
//! Установить обработчик переполнения таймера 2
void T2_SetFuncOverflow(FuncTimerOverflow *func);


#endif
