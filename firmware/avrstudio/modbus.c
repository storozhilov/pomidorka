#include <avr/io.h>
#include <util/delay.h>

#include"uart.h"
#include "modbus.h"
#include "performer.h"


BYTE modbus_buffer[MODBUS_MAX_RECEIVE];
//BYTE modbus_transmit[MODBUS_MAX_RECEIVE];

//! ������� �������� ������.
BYTE modbus_index;

//! ������� CRC16.
WORD modbus_CRC16;

FuncTimerClear *funcTimerClear;
FuncTimerStart *funcTimerStart;
FuncTimerStop  *funcTimerStop;

//! ��������� �� ���������� ������ ������ ���������.
FuncWriteReg  *funcWriteReg;

//! ��������� �� ���������� ������ ������ ���������.
FuncReadRegs   *funcReadRegs;

//! ��������� �� ���������� ������ �������� �������.
FuncWriteCoil  *funcWriteCoil;

//! ��������� �� ���������� ������ �������� �������.
FuncReadCoils   *funcReadCoils;




//! ���� ���������� ������ ������ �� MODBUS.
BYTE modbus_ReceiveEnable;

//! ����� ����������.
BYTE modbus_ADDR;

//! ���-�� �������� ���������� c ���������� CRC.
WORD modbus_TRANSACTIONS = 0;

//! ���-�� �������� ���������� � ������� CRC.
WORD modbus_ERROR_CRC_TRANSACTIONS = 0;

void MODBUS_SetFuncTimerClear(FuncTimerClear *func)
{
    funcTimerClear = func;
}

void MODBUS_SetFuncTimerStart(FuncTimerStart *func)
{
    funcTimerStart = func;
}

void MODBUS_SetFuncTimerStop(FuncTimerStop *func)
{
    funcTimerStop = func;
}

void MODBUS_SetFuncWriteReg(FuncWriteReg *func)
{
    funcWriteReg = func;
}

void MODBUS_SetFuncReadRegs(FuncReadRegs *func)
{
    funcReadRegs = func;
}

void MODBUS_SetFuncWriteCoil(FuncWriteCoil *func)
{
    funcWriteCoil = func;
}

void MODBUS_SetFuncReadCoils(FuncReadCoils *func)
{
    funcReadCoils = func;
}


void MODBUS_Init()
{
    funcTimerClear = 0;
    funcTimerStart = 0;
    funcTimerStop  = 0;
    funcWriteReg   = 0;
    funcWriteCoil  = 0;
    funcReadCoils  = 0;

    modbus_index   = 0;
    //------------------------------------
            modbus_ADDR = 0;
    //------------------------------------
    MODBUS_CLEAR_CRC16;

    modbus_ReceiveEnable = 1;
}

void MODBUS_SetAddress(BYTE addr)
{
  modbus_ADDR = addr;
}

void modbus_Calculate_CRC16(BYTE b)
{
    BYTE index;
    modbus_CRC16 ^= (WORD) b;
    for (index = 0; index < 8; index++)
     {
      if ((modbus_CRC16 & 0x01))   {
                           modbus_CRC16 = modbus_CRC16 >> 1;
                           modbus_CRC16 ^= (WORD) 0xA001;
                          }
      else { modbus_CRC16 = modbus_CRC16 >> 1; }
     }
}

void MODBUS_Receive(BYTE b)
{
    if (funcTimerClear) funcTimerClear();

    if (modbus_ReceiveEnable)
    {
        modbus_Calculate_CRC16(b);

        //    UART_SendByteSynchronously(b);
        if (modbus_index < MODBUS_MAX_RECEIVE)
        {
            if (modbus_index == 0)
            {
                // ���� ������.

                if (funcTimerStart) funcTimerStart();

                if (b != 0 && b != modbus_ADDR)
                {
                    // ��������� �� �� ������. ���������.
                    modbus_ReceiveEnable = 0;
                    return;
                }
            }

            modbus_buffer[modbus_index++] = b;
        }
    }
}

void MODBUS_SendAnswer(WORD lenData)
{
    // ��� ����������������� ������� �� ��������.
    if (modbus_buffer[0] == 0) return;

    MODBUS_CLEAR_CRC16;
    WORD i;
	lenData +=2; // +2 = addr + cmd
    for (i = 0; i < lenData; i++)
        modbus_Calculate_CRC16(modbus_buffer[i]);
    modbus_buffer[lenData]   = LOBYTE(modbus_CRC16);
    modbus_buffer[lenData+1] = HIBYTE(modbus_CRC16);

    /*
       ������ �������� � ���-�� ������, ��� � ���������, �.�.
       �� ����� �������� �� RS-485 ������ ������ �� �����.
    */

    UART_SendDataAsynchronously(modbus_buffer, lenData + 2);
}

void MODBUS_SendException(BYTE err)
{
    // ��� ����������������� ������� �� ��������.
    if (modbus_buffer[0] == 0) return;
    modbus_buffer[1] |= 0x80;   // � ������� ���������� ��� ������.
    modbus_buffer[2] = err;
    MODBUS_SendAnswer(1);
}

void MODBUS_SendEcho()
{
    UART_SendDataAsynchronously(modbus_buffer, modbus_index);
}

void MODBUS_Execute()
{
    if (funcTimerStop) funcTimerStop();

    modbus_ReceiveEnable = 0;
    if (modbus_CRC16)
    {
        // ������ CRC.
        MODBUS_CLEAR_CRC16;
        modbus_index = 0;
        modbus_ReceiveEnable = 1;
        modbus_ERROR_CRC_TRANSACTIONS++;
//        UART_SendTextAsynchronously("Error CRC\r\n");
        return;
    }

    modbus_TRANSACTIONS++;

/*
  1 ���� - �����.
  1 ���� - �������.
  N ���� - ������.
  2 ����� - CRC16
*/

    // ���������� ������

	// ���� 30 ���.
 	_delay_ms(30);

//    WORD cmd = MAKEWORD(modbus_buffer[1], modbus_buffer[2]);
    BYTE cmd = modbus_buffer[1];

    WORD lenData = modbus_index - 4;    // ����� ������.
    BYTE *data = &(modbus_buffer[2]);

    if (lenData == 4 && cmd == MODBUS_cmd_WriteSingleRegister && funcWriteReg) {
        WORD numReg = MAKEWORD(data[1], data[0]);
        WORD value  = MAKEWORD(data[3], data[2]);

        funcWriteReg(numReg, value);

        MODBUS_SendEcho();

    } else if (lenData == 4 && cmd == MODBUS_cmd_ReadHoldingRegisters && funcReadRegs) {

        WORD numReg = MAKEWORD(data[1], data[0]);
        WORD cnt    = MAKEWORD(data[3], data[2]);
        WORD r = 0;
        WORD i;

        if (cnt >= 1 && cnt <= PERFORMER_MAX_REGS)
        {
            if (numReg + cnt <= PERFORMER_MAX_REGS)
            {
                if (numReg >= 0 && numReg < PERFORMER_MAX_REGS)
                {

                    data[0] = (BYTE) cnt*2; // ���-�� ����.
                    for (i = 0; i < data[0]; i += 2)
                    {
                            r = funcReadRegs(numReg++);
                            data[i+1] = HIBYTE(r);
                            data[i+2] = LOBYTE(r);
                    }
                    r = -1; // ����, ��� ��� ������ ������.
                } else MODBUS_SendException(0x04);
            } else MODBUS_SendException(0x02);
        } else  MODBUS_SendException(0x03);

        if (r == -1) MODBUS_SendAnswer(data[0] + 1);   // ���� ��� ������, ����������...

    } else if (lenData == 4 && cmd == MODBUS_cmd_WriteSingleCoil && funcWriteCoil) {
        WORD numReg = MAKEWORD(data[1], data[0]);
        WORD value  = MAKEWORD(data[3], data[2]);

        funcWriteCoil(numReg, value);

        MODBUS_SendEcho();

    } else if (lenData == 4 && cmd == MODBUS_cmd_ReadCoils && funcReadCoils) {

        WORD numReg = MAKEWORD(data[1], data[0]);
        WORD cnt    = MAKEWORD(data[3], data[2]);
        WORD r = 1;
        WORD i;
        BYTE i2;
        if (cnt >= 1 && cnt <= PERFORMER_MAX_COILS)
        {
            if (numReg + cnt <= PERFORMER_MAX_COILS)
            {
                if (numReg >= 0 && numReg < PERFORMER_MAX_COILS)
                {
                    // �� 8 ������������ �������� ������� ���������� � 1 ����. ������� ���-�� ����.
                    data[0] = (BYTE) (cnt/8);
					if (cnt%8) (data[0])++;
                    i2 = 0;
					data[r] = 0;
                    for (i = 0; i < cnt; i++)
                    {
                        data[r] = data[r] << 1;
                        if (funcReadCoils(numReg + i) == MODBUS_COIL_ON) data[r] |= 1;
                        if (i2++ == 7) {i2 = 0; data[++r] = 0x00;}
                    }
                    r = -1; // ����, ��� ��� ������ ������.
                } else MODBUS_SendException(0x04);
            } else MODBUS_SendException(0x02);
        } else  MODBUS_SendException(0x03);

        if (r == -1) MODBUS_SendAnswer(data[0] + 1);   // ���� ��� ������, ����������...

    } else MODBUS_SendException(0x01);      // ������� �� ��������������.

    MODBUS_CLEAR_CRC16;
    modbus_index = 0;
    modbus_ReceiveEnable = 1;
}

void MODBUS_EmulateReadRegister()
{
    // ������� 3 ���������� 1 �� ������ 1 �������� �� ������ 0.

    MODBUS_Receive(0x01);
    MODBUS_Receive(0x03);
    MODBUS_Receive(0x00);
    MODBUS_Receive(0x00);
    MODBUS_Receive(0x00);
    MODBUS_Receive(0x10);
    MODBUS_Receive(0x44);
    MODBUS_Receive(0x06);
}


