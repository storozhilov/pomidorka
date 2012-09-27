#ifndef __PERFORMER_H
#define __PERFORMER_H

#include "defines.h"

//! ��������� ����� ��������.
#define PERFORMER_START_REG 100

//! ���-�� ���������.
#define PERFORMER_MAX_REGS 16

//! ���-�� ���-�������.
#define PERFORMER_MAX_COILS 8

//! ������������� ��������������� ������.
void PERFORMER_Init();

//! ��������� �������.
//! \param addr - ����� ��������.
//! \return �������� ��������.
WORD PERFORMER_ReadRegister(WORD addr);

//! �������� �������� ���������.
//! \param addr - ����� ��������.
//! \param reg -  �������� ��������.
void PERFORMER_WriteRegister(WORD addr, WORD reg);


//! ��������� �������� �������.
//! \param addr - ����� �������.
//! \return �������� �������.
//! \retval MODBUS_COIL_ON - ��������.
//! \retval MODBUS_COIL_OFF - ���������.
//! \sa MODBUS_COIL_ON
//! \sa MODBUS_COIL_OFF
WORD PERFORMER_ReadCoils(WORD addr);

//! �������� ��������� �������.
//! \param addr - ����� �������.
//! \param value -  �������� �������.
void PERFORMER_WriteCoil(WORD addr, WORD value);



//! �����������
void PERFORMER_Execute();



#endif
