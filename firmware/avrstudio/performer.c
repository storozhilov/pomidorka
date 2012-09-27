#include <avr/io.h>

#include "performer.h"
#include "Controller1.h"
#include "ADC.h"
#include "DS18B20.h"
#include "modbus.h"
#include "load.h"




//! ������� ��������.
WORD PERFORMER_Regs[PERFORMER_MAX_REGS];

//! ������� �������� �������.
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
    case 0: // ���������� ��������� 1.
        if (value) LOAD_1_On(PERFORMER_Regs[6]);
        else       LOAD_1_Off();
        break;

    case 1: // ���������� ��������� 2.
        if (value) LOAD_2_On(PERFORMER_Regs[8], PERFORMER_Regs[9]);
        else       LOAD_2_Off();
        break;

    case 2: // ���������� ��������� 3.
        if (value) LOAD3_ON;
        else       LOAD3_OFF;
        break;

//    case 4: // ���������� ��������� 4.
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

    PERFORMER_Regs[2] = DS18B20_GetTemparature(0);   // ����������� 1
    PERFORMER_Regs[3] = DS18B20_GetTemparature(1);   // ����������� 2
    PERFORMER_Regs[4] = ADC_GetValue(6);             // ��� 1
    PERFORMER_Regs[5] = ADC_GetValue(7);             // ��� 2


    // ����� ��������.
    w = PERFORMER_Regs[7];
    LOAD_1_SetDirection(w & (1 << 4));
    if (LOAD_1_IsOn()) {
        if (w & (1 << 4)) {
            // ����������� �����.
            if (w & (1 << 2)) {
                // ��������� ��������, ���� �������� 2 �������.
                if (!IS_IN2) {LOAD_1_Off(); PERFORMER_Regs[7] |= (3 << 8);}
            } else if (w & (1 << 3)) {
                // ��������� ��������, ���� �������� 2 ���������.
                if (IS_IN2) {LOAD_1_Off();  PERFORMER_Regs[7] |= (3 << 8);}
            }
        } else {
            // ����������� ������.
            if (w & (1 << 0)) {
                // ��������� ��������, ���� �������� 1 �������.
                if (!IS_IN1) { LOAD_1_Off(); PERFORMER_Regs[7] &= ~(3 << 8); PERFORMER_Regs[7] |= (2 << 8); }
            } else if (w & (1 << 1)) {
                // ��������� ��������, ���� �������� 1 ���������.
                if (IS_IN1) { LOAD_1_Off();  PERFORMER_Regs[7] &= ~(3 << 8); PERFORMER_Regs[7] |= (2 << 8); }
            }
        }
    }




    // ��� ��������� ���������.
//    pw = (WORD *) &(PERFORMER_Regs[8]);
//    if (*pw & 0x0001) {
        // ��������.
        ////! ������� �������� ���������.
        //BYTE performer_Heating_Reg;

        ////! ������� �������� ��� ��������� ���������. ����� ��������� � ��������.
        //BYTE performer_Heating_ON;

        ////! ������� �������� ��� ��������� ���������. ����� ���������� � ��������.
        //BYTE performer_Heating_OFF;

//    } else {
        // ���������.
//    }





}
