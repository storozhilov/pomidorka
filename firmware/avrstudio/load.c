#include <avr/io.h>
#include <avr/interrupt.h>

#include "load.h"
#include "Controller1.h"

//WORD load_TimerIndex;

//! Отведенное время для таймера нагрузки 1.
WORD load1_MaxTick;

//! Кол-во тиков включенной нагрузки 2.
WORD load2_OnTick;
//! Кол-во тиков выключенной нагрузки 2.
WORD load2_OffTick;
//! Счетчик тиков нагрузки 2.
WORD load2_current;

void LOAD_Init()
{
    load2_OnTick = 0;
    LOAD_1_DIRECTION_OFF;
}

void LOAD_1_On(WORD max_tick)
{
    load1_MaxTick = max_tick;
    LOAD_1_ON;
}

void LOAD_1_Off()
{
    LOAD_1_OFF;
}

void LOAD_1_SetDirection(BYTE bReverse)
{
    if (bReverse)   LOAD_1_DIRECTION_ON;
    else            LOAD_1_DIRECTION_OFF;
}

BYTE LOAD_1_IsOn()
{
    return LOAD_1_STATUS;
}

void LOAD_2_On(WORD on_100mls, WORD off_100mls)
{
    load2_OnTick = on_100mls;
    load2_OffTick = off_100mls;

    load2_current = 0;
    LOAD_2_ON;
}

void LOAD_2_Off()
{
    load2_OnTick = 0;
    LOAD_2_OFF;
}


void LOAD_Timer_Interrupt_100_mls()
{
    if (LOAD_1_IsOn() && load1_MaxTick) if (!--load1_MaxTick)  LOAD_1_Off();

    if (load2_OnTick) {
        // ШИМ нагрузки 2 активирован
        load2_current++;
        if (LOAD_2_STATUS) {
            if (load2_current >= load2_OnTick) { if (load2_OffTick) LOAD_2_OFF; load2_current = 0;}
        } else {
            if (load2_current >= load2_OffTick) {LOAD_2_ON; load2_current = 0;}
        }
    }
}




