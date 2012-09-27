#include <avr/io.h>

#include "performer.h"
#include "Controller1.h"
#include "ADC.h"
#include "DS18B20.h"
#include "modbus.h"
#include "load.h"




//! Рабочие регистры.
WORD PERFORMER_Regs[PERFORMER_MAX_REGS];

//! Рабочие значения катушек.
WORD PERFORMER_Coils[PERFORMER_MAX_COILS];

void PERFORMER_Init()
{
    PERFORMER_Regs[7] &= ~(3 << 8); PERFORMER_Regs[7] |= (2 << 8);

    BYTE t;
    for (t = 0; t < PERFORMER_MAX_REGS; t++) PERFORMER_Regs[0] = 0;
}

WORD PERFORMER_ReadRegister(WORD addr)
{
    if (addr >= PERFORMER_MAX_REGS) return 0;
    return PERFORMER_Regs[addr];
}

void PERFORMER_WriteRegister(WORD addr, WORD reg)
{
    if (addr < PERFORMER_MAX_REGS)  PERFORMER_Regs[addr] = reg;
}

WORD PERFORMER_ReadCoils(WORD addr)
{
    if (addr >= PERFORMER_MAX_COILS) return MODBUS_COIL_ERROR;
    return PERFORMER_Coils[addr];
}

void PERFORMER_WriteCoil(WORD addr, WORD value)
{
    if (addr >= PERFORMER_MAX_COILS) return;
    if (value != MODBUS_COIL_ON && value != MODBUS_COIL_OFF) return;

    PERFORMER_Coils[addr] = value;

    switch (addr)
    {
    case 0: // Управление нагрузкой 1.
        if (value) LOAD_1_On(PERFORMER_Regs[6]);
        else       LOAD_1_Off();
        break;

    case 1: // Управление нагрузкой 2.
        if (value) LOAD_2_On(PERFORMER_Regs[8], PERFORMER_Regs[9]);
        else       LOAD_2_Off();
        break;

    case 2: // Управление нагрузкой 3.
        if (value) LOAD3_ON;
        else       LOAD3_OFF;
        break;

//    case 4: // Управление нагрузкой 4.
//        if (value) LOAD4_ON;
//        else       LOAD4_OFF;
//        break;
    }

}

void PERFORMER_Execute()
{
//    WORD *pw;
    WORD w;
    w = 0;
    if (IS_IN1) w |= (1 << 0);
    if (IS_IN2) w |= (1 << 1);
    if (IS_IN3) w |= (1 << 2);
    if (IS_IN4) w |= (1 << 3);
    if (IS_IN5) w |= (1 << 4);
    if (IS_IN6) w |= (1 << 5);

    PERFORMER_Regs[0] = w;

    PERFORMER_Regs[2] = DS18B20_GetTemparature(0);   // Температура 1
    PERFORMER_Regs[3] = DS18B20_GetTemparature(1);   // Температура 2
    PERFORMER_Regs[4] = ADC_GetValue(6);             // АЦП 1
    PERFORMER_Regs[5] = ADC_GetValue(7);             // АЦП 2


    // Опции нагрузки.
    w = PERFORMER_Regs[7];
    LOAD_1_SetDirection(w & (1 << 4));
    if (LOAD_1_IsOn()) {
        if (w & (1 << 4)) {
            // Направление назад.
            if (w & (1 << 2)) {
                // Отключить нагрузку, если концевик 2 замкнут.
                if (!IS_IN2) {LOAD_1_Off(); PERFORMER_Regs[7] |= (3 << 8);}
            } else if (w & (1 << 3)) {
                // Отключить нагрузку, если концевик 2 разомкнут.
                if (IS_IN2) {LOAD_1_Off();  PERFORMER_Regs[7] |= (3 << 8);}
            }
        } else {
            // Направление вперед.
            if (w & (1 << 0)) {
                // Отключить нагрузку, если концевик 1 замкнут.
                if (!IS_IN1) { LOAD_1_Off(); PERFORMER_Regs[7] &= ~(3 << 8); PERFORMER_Regs[7] |= (2 << 8); }
            } else if (w & (1 << 1)) {
                // Отключить нагрузку, если концевик 1 разомкнут.
                if (IS_IN1) { LOAD_1_Off();  PERFORMER_Regs[7] &= ~(3 << 8); PERFORMER_Regs[7] |= (2 << 8); }
            }
        }
    }




    // ШИМ модуляция отопления.
//    pw = (WORD *) &(PERFORMER_Regs[8]);
//    if (*pw & 0x0001) {
        // Включаем.
        ////! Регистр настроек отопления.
        //BYTE performer_Heating_Reg;

        ////! Регистр настроек ШИМ модуляции отопления. Время включения в секундах.
        //BYTE performer_Heating_ON;

        ////! Регистр настроек ШИМ модуляции отопления. Время выключения в секундах.
        //BYTE performer_Heating_OFF;

//    } else {
        // Отключаем.
//    }





}
