#ifndef __modbus__H
#define __modbus__H

#include "defines.h"


//! ������ ������ ������� ������.
#define MODBUS_MAX_RECEIVE 64

//! �������� ����������� ��������� �������.
#define MODBUS_COIL_ON      0xFF00

//! �������� ������������ ��������� �������.
#define MODBUS_COIL_OFF     0x0000

//! ����������� ��������� �������.
#define MODBUS_COIL_ERROR   0x00FF



typedef void (FuncTimerClear)(void);
typedef void (FuncTimerStart)(void);
typedef void (FuncTimerStop)(void);

typedef void (FuncWriteReg)(WORD, WORD);
typedef WORD (FuncReadRegs)(WORD);
typedef void (FuncWriteCoil)(WORD, WORD);
typedef WORD (FuncReadCoils)(WORD);

//! ���������� ���������� �� ����� �������.
void MODBUS_SetFuncTimerClear(FuncTimerClear *func);

//! ���������� ���������� �� ����� �������.
void MODBUS_SetFuncTimerStart(FuncTimerStart *func);

//! ���������� ���������� �� ���� �������.
void MODBUS_SetFuncTimerStop(FuncTimerStop *func);

//! ���������� ���������� �� ������ ���������.
void MODBUS_SetFuncWriteReg(FuncWriteReg *func);

//! ���������� ���������� �� ������ ���������.
void MODBUS_SetFuncReadRegs(FuncReadRegs *func);

//! ���������� ���������� �� ������ �������� �������.
void MODBUS_SetFuncWriteCoil(FuncWriteCoil *func);

//! ���������� ���������� �� ������ �������� �������.
void MODBUS_SetFuncReadCoils(FuncReadCoils *func);



//! ������������� ������.
void MODBUS_Init();

//! ����� ����� ��� ������.
//! \param b - �������� ����.
void MODBUS_Receive(BYTE b);

//! ���������� ������ ������ ����������, ����� ��������� ��������� CRC16.
//! \param lenData - ����� ������ � ������� � ��������.
void MODBUS_SendAnswer(WORD lenData);

//! ���������� � ����� ������.
//! \param err - ��� ������.
void MODBUS_SendException(BYTE err);

//! ���������� ��� ������ ������ ����������.
void MODBUS_SendEcho();

//! ���������� �� ������������, �����
/*!
   (1/57600)*10*4 = 694 ���. �������.

   ������������ ����������� �������.
*/
void MODBUS_Execute();

//! ������� ����������� ����� � modbus_CRC16.
//! \param b - ���� ������ ��� �������.
//! \sa modbus_CRC16.
void modbus_Calculate_CRC16(BYTE b);

//! ���������� ������� CRC.
#define MODBUS_CLEAR_CRC16 modbus_CRC16 = -1;

//! ������������� ����� ����������.
void MODBUS_SetAddress(BYTE addr);

void MODBUS_EmulateReadRegister();




//! �������. �������� ��������.
// 2 ����� - ����� ��������.
// N ���� - ������.
#define MODBUS_cmd_WriteSingleRegister      0x06


//! �������. ��������� ��������.
#define MODBUS_cmd_ReadHoldingRegisters     0x03
// 2 ����� - ����� ��������.
// 1 ���� - ���-�� ���������.

//! �������. �������� �������� �������.
#define MODBUS_cmd_WriteSingleCoil          0x05

//! �������. �������� �������� �������.
#define MODBUS_cmd_ReadCoils                0x01

#endif
