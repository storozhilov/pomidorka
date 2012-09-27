#ifndef __TIMERS_H
#define __TIMERS_H


//-----------------------------------------------------------------
//! ������������� ������� 0.
void T0_Init();

//! ����� ������ 0.
#define T0_START   TCCR0B = (1 << CS02) | (0 << CS01) | (0 << CS00);     // CK/256       4444 ���
//! ���� ������ 0.
#define T0_STOP    TCCR0B = 0x00;
//! ����� ������ 0.
#define T0_CLEAR   TCNT0  = 0x00;

void T0_Start();
void T0_Stop();
void T0_Clear();
//-----------------------------------------------------------------
//! ������������� ������� 1. ����������� ������ 1 �� ���������� ������ 100 ���.
void T1_Init();

//! ����� ������ 1. ����� CTC.
#define T1_START   TCCR1B = (1 << WGM12) | (0 << CS12) | (1 << CS11) | (1 << CS10);     // CK/64
//! ���� ������ 1.
#define T1_STOP    TCCR1B = 0x00;
//! ����� ������ 1.
#define T1_CLEAR   TCNT1  = 0x00;

void T1_Start();
void T1_Stop();
void T1_Clear();
//-----------------------------------------------------------------
//! ������������� ������� 2.
void T2_Init();

//! ����� ������ 2.
#define T2_START   TCCR2B = (1 << CS22) | (1 << CS21) | (1 << CS20);     // CK/1024     13 ���
//! ���� ������ 2.
#define T2_STOP    TCCR2B = 0x00;
//! ����� ������ 2.
#define T2_CLEAR   TCNT2  = 0x00;

void T2_Start();
void T2_Stop();
void T2_Clear();
//-----------------------------------------------------------------

typedef void (FuncTimerOverflow)(void);

//! ���������� ���������� ������������ ������� 0
void T0_SetFuncOverflow(FuncTimerOverflow *func);
//! ���������� ���������� ������������ ������� 1
void T1_SetFuncOverflow(FuncTimerOverflow *func);
//! ���������� ���������� ������������ ������� 2
void T2_SetFuncOverflow(FuncTimerOverflow *func);


#endif
