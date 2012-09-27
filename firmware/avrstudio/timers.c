#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/wdt.h>

#include "timers.h"
#include "defines.h"


FuncTimerOverflow *funcOverflowT0;
FuncTimerOverflow *funcOverflowT1;
FuncTimerOverflow *funcOverflowT2;

volatile BYTE T1_100mls;

void T0_SetFuncOverflow(FuncTimerOverflow *func)
{
    funcOverflowT0 = func;
}

void T1_SetFuncOverflow(FuncTimerOverflow *func)
{
    funcOverflowT1 = func;
}

void T2_SetFuncOverflow(FuncTimerOverflow *func)
{
    funcOverflowT2 = func;
}


void T0_Init()
{
    funcOverflowT0 = 0;
    TCCR0A = 0x00;
    TCCR0B = 0x00;
    TIMSK0 |= (1 << TOIE0);
}

void T1_Init()
{
    funcOverflowT1 = 0;
    TCCR1A = 0x00;
    TCCR1B = 0x00;

    // 1_clk = 64/F_CPU
    // clk = 0.1 sek / (1_clk) = 0x5A00
    OCR1A = 0x5A00;
    TIMSK1 |= (1 << OCIE1A);

    //    TIMSK1 |= (1 << TOIE1);
}

void T2_Init()
{
    funcOverflowT2 = 0;
    TCCR2A = 0x00;
    TCCR2B = 0x00;
    TIMSK2 |= (1 << TOIE2);
}

void T0_Start() { T0_START; }
void T0_Stop()  { T0_STOP;  }
void T0_Clear() { T0_CLEAR; }

void T1_Start() { T1_START; }
void T1_Stop()  { T1_STOP;  }
void T1_Clear() { T1_CLEAR; }

void T2_Start() { T2_START; }
void T2_Stop()  { T2_STOP;  }
void T2_Clear() { T2_CLEAR; }




//! Прерывание. Переполнение таймера 0.
ISR(TIMER0_OVF_vect)
{
    if (funcOverflowT0) funcOverflowT0();
}

//! Прерывание. Переполнение таймера 1.
ISR(TIMER1_OVF_vect)
{
    if (funcOverflowT1) funcOverflowT1();
}

//! Прерывание. Переполнение таймера 2.
ISR(TIMER2_OVF_vect)
{
    if (funcOverflowT2) funcOverflowT2();
}

//! Прерывание. Таймер 1 достиг значение OCR1A
ISR(TIMER1_COMPA_vect)
{
    T1_100mls++;
	if (funcOverflowT1) funcOverflowT1();
}
