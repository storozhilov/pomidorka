#ifndef __LOAD_H
#define __LOAD_H

#include "defines.h"

/*!
    ������ ����������� ����������.
    �������� 1 ����� ������������� ����������� �� ��������� ��������� �������, � ���-�� �� ������� �� ���� ����������.
    �������� 2 ����� �������� � ������ ���.
*/

//! ��������� �������� 1.
#define LOAD_1_ON               LOAD1_ON
//! ���������� �������� 1.
#define LOAD_1_OFF              LOAD1_OFF
//! �������� ����������� �������� 1.
#define LOAD_1_DIRECTION_ON     LOAD1_DIRECTION_ON
//! ������ ����������� �������� 1.
#define LOAD_1_DIRECTION_OFF    LOAD1_DIRECTION_OFF
//! ��������� �������� 1.
#define LOAD_1_STATUS           LOAD1_STATUS

//! ��������� �������� 2.
#define LOAD_2_ON               LOAD2_ON
//! ���������� �������� 2.
#define LOAD_2_OFF              LOAD2_OFF
//! ��������� �������� 2.
#define LOAD_2_STATUS           LOAD2_STATUS




//! ������ ���������� ���������.
void LOAD_Init();

//! ��������� �������� 1.
//! \param tick_100mls - ������������ ����� ���������, 1=100 mls, 0 = ������.
//! \sa LOAD_Timer_Interrupt_100_mls.
void LOAD_1_On(WORD tick_100mls);

//! ���������� �������� 1.
void LOAD_1_Off();

//! ���������� ����������� ��������� �������� 1.
//! \param bReverse, 0 = ������ ����������� ��������, 1 = ��������������� ����������� ��������.
void LOAD_1_SetDirection(BYTE bReverse);


//! ���������� ������� ��������� �������� 1.
//! \retval >= 1 - �������� ��������.
//! \retval = 0 - �������� ���������.
BYTE LOAD_1_IsOn();

//! ��������� �������� 2 � ������ ���.
//! \param on_100mls - ����� ��������� ��������, 1=100 mls.
//! \param off_100mls - ����� ���������� ��������, 1=100 mls.
//! \sa LOAD_Timer_Interrupt_100_mls.
void LOAD_2_On(WORD on_100mls, WORD off_100mls);

//! ���������� �������� 1.
void LOAD_2_Off();

//! ���������� ������� ��������� �������� 2.
//! \retval >= 1 - �������� ��������.
//! \retval = 0 - �������� ���������.
void LOAD_2_IsOn();



//! ����� ����� ���������� �������, ������ ������� ������ 100 ���.
void LOAD_Timer_Interrupt_100_mls();


#endif

