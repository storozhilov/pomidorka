#ifndef __UART_H
#define __UART_H

#include "defines.h"

//! ������������ ��� ������ RS485.
#define UART_RS485

#ifdef UART_RS485
    #include "Controller1.h"
    #define RS485_TX_ON   TX_ON
    #define RS485_RX_ON   RX_ON
	#define RS485_TX_STATUS PORTD & TX_RX
#endif


#define UART_MAX_RECEIVE  64
//#define UART_MAX_TRANSMIT 64

//! ���������� �������� ������.
typedef void (FuncReceive)(BYTE);

//! ������������� ����.
void UART_Init();

////! ���������� 1 ���� �� ����.
//void UART_SendByteSynchronously(unsigned char byte);

////! ���������� ������ �� ���� � ���������� ������. �������� � �������� �������� ������.
////! \param txt - ��������� �� ������ � ����������� \0.
//void UART_SendTextSynchronously(char *txt);

//! ����������� �������� ������ �� ����.
//! \param txt - �����.
//! \sa UART_TransmitIsEmpty
void UART_SendTextAsynchronously(char *txt);

//! ����������� �������� ������ �� ����.
//! \param d - ��������� �� ������.
//! \param len - ����� ������.
//! \sa UART_TransmitIsEmpty
void UART_SendDataAsynchronously(BYTE *d, WORD len);

//! ���������� ������ ����������� ����.
//! \retval 0 - �� ���� ��������.
//! \retval 1 - � ������ ��������.
BYTE UART_TransmitIsEmpty();

//! �������, ���� ����� �������� �� �����������.
void UART_WaitTransmitIsEmpty();


//! ���������� ���������� �������� ������.
void UART_SetFuncReceive(FuncReceive *func);






#endif
