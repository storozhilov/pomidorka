#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/wdt.h>
#include <util/delay_basic.h>
#include <avr/pgmspace.h>
#include <stdio.h>
#include <string.h>

#include "Controller1.h"
#include "modbus.h"
#include "uart.h"
#include "timers.h"
#include "performer.h"
#include "ADC.h"
#include "DS18B20.h"
#include "load.h"

extern volatile BYTE T1_100mls;

char txt[32];
int main()
{

 wdt_enable(WDTO_1S);


 DDRB = (LOAD1 | LOAD1_DIRECTION);
 DDRC = (LOAD2 | LOAD3);
 DDRD = (TX_RX);

 // ������� ��� ������.
 PORTD = IN1 | IN3 | IN4 | IN5 | IN6;
 PORTC = IN2;


 UART_Init();       // �������������� ������ UART.
 MODBUS_Init();     // �������������� ������ ��������� ModBus.
 T0_Init();         // �������������� ������ 0.
 T1_Init();         // �������������� ������ 1.
 T1_Start();
 ADC_Init();        // �������������� ������ ���.
 LOAD_Init();       // �������������� ������ ����������� ��������.


 // ������������� ���������� �������� ������ � ���� �� �������� MODBUS.
 UART_SetFuncReceive(MODBUS_Receive);

 // ������������� ���������� ������������ �������0 �� �������� MODBUS.
 T0_SetFuncOverflow(MODBUS_Execute);

 // ������������� ����������� ��� ������ MODBUS.
 MODBUS_SetFuncTimerClear(T0_Clear);
 MODBUS_SetFuncTimerStart(T0_Start);
 MODBUS_SetFuncTimerStop(T0_Stop);
 MODBUS_SetFuncWriteReg(PERFORMER_WriteRegister);
 MODBUS_SetFuncReadRegs(PERFORMER_ReadRegister);
 MODBUS_SetFuncWriteCoil(PERFORMER_WriteCoil);
 MODBUS_SetFuncReadCoils(PERFORMER_ReadCoils);


 // ������������� ���������� ������������ �������1 (100 ���) �� ����� ����� ������ ��������.
 T1_SetFuncOverflow(LOAD_Timer_Interrupt_100_mls);


 MODBUS_SetAddress(1);

 sei();

 UART_SendTextAsynchronously("Controller1, version 1.0.1\r\n");
 UART_WaitTransmitIsEmpty();

 BYTE b[] = {6, 7};
 ADC_Start(b, 2);

 DS18B20_Init();


 //MODBUS_EmulateReadRegister();
 while(1)
 {
  wdt_reset();

     DS18B20_Execute();
     PERFORMER_Execute();

//     if (!IS_IN4) LOAD2_ON; else LOAD2_OFF;

     if (!IS_IN6)
      if (T1_100mls >= 2)
		{
			T1_100mls = 0;
			sprintf(txt, "ADC6=%i\r\n", ADC_GetValue(6));
            UART_SendTextAsynchronously(txt); UART_WaitTransmitIsEmpty();
//            sprintf(txt, "ds2=%i\r\n", DS18B20_GetTemparature(1));
//            UART_SendTextAsynchronously(txt); UART_WaitTransmitIsEmpty();
        }

 };

}
