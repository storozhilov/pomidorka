#ifndef __UART_H
#define __UART_H

#include "defines.h"

//! Использовать как модуль RS485.
#define UART_RS485

#ifdef UART_RS485
    #include "Controller1.h"
    #define RS485_TX_ON   TX_ON
    #define RS485_RX_ON   RX_ON
	#define RS485_TX_STATUS PORTD & TX_RX
#endif


#define UART_MAX_RECEIVE  64
//#define UART_MAX_TRANSMIT 64

//! Обработчик входящих данных.
typedef void (FuncReceive)(BYTE);

//! Инициализация УАРТ.
void UART_Init();

////! Отправляет 1 байт по УАРТ.
//void UART_SendByteSynchronously(unsigned char byte);

////! Отправляет строку по УАРТ в синхронном режиме. Зависнет в ожидании передачи данных.
////! \param txt - указатель на строку с завершающим \0.
//void UART_SendTextSynchronously(char *txt);

//! Асинхронная отправка текста по УАРТ.
//! \param txt - текст.
//! \sa UART_TransmitIsEmpty
void UART_SendTextAsynchronously(char *txt);

//! Асинхронная отправка данных по УАРТ.
//! \param d - указатель на данные.
//! \param len - длина данных.
//! \sa UART_TransmitIsEmpty
void UART_SendDataAsynchronously(BYTE *d, WORD len);

//! Возвращает статус передатчика УАРТ.
//! \retval 0 - Не идет передача.
//! \retval 1 - В режиме передачи.
BYTE UART_TransmitIsEmpty();

//! Ожидает, пока буфер передачи не освободится.
void UART_WaitTransmitIsEmpty();


//! Установить обработчик входящих данных.
void UART_SetFuncReceive(FuncReceive *func);






#endif
