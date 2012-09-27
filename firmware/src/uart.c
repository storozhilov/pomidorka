#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/wdt.h>
#include <util/delay.h>
#include <avr/pgmspace.h>
#include <stdio.h>
#include <string.h>


#include "uart.h"
#include "defines.h"


//! Функтор на функцию, для входящих данных.
FuncReceive *funcReceive = NULL;

//! Указатель на отправляемые данные.
BYTE *uart_pTransmit;

//! Кол-во данных для отправки.
volatile WORD uart_len;

void UART_Init()
{

#ifdef UART_RS485
    RS485_RX_ON;
#endif

 // 57600, 8 бит, четность even


 UBRR0L = 0x0F;
 UBRR0H = 0x00;
 UCSR0C = (1 << UPM01) | (0 << UPM00) | 0x06; // Event parity + 8 bit
 UCSR0A = 0x40;
 UCSR0B = (1 << TXCIE0 | 1 << RXCIE0);              // Прерывание
 UCSR0B = UCSR0B | (1 << RXEN0) | (1 << TXEN0);     //

}

//void UART_SendByteSynchronously(unsigned char byte)
//{
//    while (!(UCSR0A & (1 << UDRE0)));
//    UDR0    = byte;
//}

//void UART_SendTextSynchronously(char *txt)
//{
//    unsigned short t = 0;
//    while (txt[t])
//    {
//        UART_SendByteSynchronously(txt[t++]);
//    }
//}

void UART_SendTextAsynchronously(char *txt)
{
#ifdef UART_RS485
    RS485_TX_ON;
	_delay_us(5);
#endif
    uart_pTransmit = (BYTE *) txt;
    uart_len = strlen(txt);
    while (!(UCSR0A & (1 << UDRE0)));
    UDR0    = *(uart_pTransmit++);
}

void UART_SendDataAsynchronously(BYTE *d, WORD len)
{
#ifdef UART_RS485
    RS485_TX_ON;
	_delay_us(5);
#endif
    uart_pTransmit = d;
    uart_len  = len;
    while (!(UCSR0A & (1 << UDRE0)));
    UDR0    = *(uart_pTransmit++);
}


BYTE UART_TransmitIsEmpty()
{
#ifdef UART_RS485
 	return  !(RS485_TX_STATUS);
#else
    if (uart_len) return 0;
    return (UCSR0A & (1 << UDRE0));
#endif
}

void UART_WaitTransmitIsEmpty()
{
    while(!UART_TransmitIsEmpty());
}


void UART_SetFuncReceive(FuncReceive *func)
{
    funcReceive = func;
}


//! Пришел 1 байт по UART
ISR(USART_RX_vect) 
{
    BYTE rec = UDR0;
    if (funcReceive) funcReceive(rec);
}


//! 1 байт по UART передан
ISR(USART_TX_vect)
{
    if (--uart_len)
    {
        UDR0 = *(uart_pTransmit++);
    } else {
        #ifdef UART_RS485
		_delay_us(200); // 200 микросекунд для передачи 11 бит на скорости 57600.
        RS485_RX_ON;
        #endif
    }

//  UCSR0B &= ~(1 << TXEN0);
//  UCSR0B |= (1 << RXEN0);
}

