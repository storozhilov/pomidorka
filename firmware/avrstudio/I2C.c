
#include <avr/io.h>
#include <avr/interrupt.h>

#include <util/twi.h>
#include <stdio.h>

#include "I2C.h"
#include "main.h"
#include "defines.h"


volatile unsigned char b_i2c_Timeout;
volatile BYTE i2c_stat;
BYTE i2c_addr;
BYTE i2c_Index;
BYTE *data;
BYTE len;

unsigned char i2c_Execute(unsigned char Addr, unsigned char *val, unsigned char Len); 




//------------------------------------------------------------------------
unsigned char I2C_WriteData(unsigned char Addr, unsigned char *Data, unsigned char Len)
{
	Addr &= ~1;
	return i2c_Execute(Addr, Data, Len);
}

unsigned char I2C_ReadData(unsigned char Addr, unsigned char *Data, unsigned char Len)
{
	Addr |= 1;
	return i2c_Execute(Addr, Data, Len);
}

unsigned char i2c_Execute(unsigned char Addr, unsigned char *Data, unsigned char Len)
{


L0: if (i2c_stat == DEVICE_EXECUTE) goto L0;
LED2_ON;

 TWSR = 0x01; TWBR = 0xF8;  // ~ 10 KHz
// TWBR  = 0x0A;  // 100 Kz

 TWCR  = 0;
 i2c_addr = Addr;
 data = Data;
 len = Len;

 i2c_stat = DEVICE_START;
 I2C_START;

// Запускаем Таймер 2
// b_i2c_Timeout = 0;
// ClearT2;
// StartT2;

// Если за Время таймера2 (~13) млс процесс не закончен - устанавливается флаг таймаута операции, и выходим.

L1:
 if (i2c_stat == DEVICE_COMPLETE) 
 {
	 LED2_OFF;
//	 StopT2;
	 return 1;
 }
 if (i2c_stat == DEVICE_ERROR)
 {
//	 StopT2;
     return 0;
 }
 // if (b_i2c_Timeout)    return 0;
 goto L1;
}
//------------------------------------------------------------------------
ISR (TWI_vect)
{

// DBG("TWSR=", TW_STATUS);

  ClearT2;

 switch (TW_STATUS)
 {
  case TW_START:              // 0x08
  case TW_REP_START:          // 0x10
   TWDR = i2c_addr;
   TWCR = ((1 << TWINT) | (1 << TWEN) | (0 << TWEA) | (1 << TWIE));
   i2c_Index = 0;
   i2c_stat  = DEVICE_EXECUTE;
  break; 

  case TW_MR_SLA_ACK:         // 0x40
   TWCR = ((1 << TWINT) | (1 << TWEN) | (1 << TWEA) | (1 << TWIE));
  break;

  case TW_MR_DATA_ACK:        // 0x50
	 if (i2c_Index == len) 
	     {
          I2C_ST0P;
          i2c_stat = DEVICE_COMPLETE; 	       
		 }
	  else  
	  {
        data[i2c_Index++] = TWDR;
	  }

  break;

  case TW_MR_SLA_NACK:        // 
  case TW_MT_SLA_NACK:
	     I2C_ST0P;
         i2c_stat = DEVICE_ERROR;
  break;

  case TW_MR_DATA_NACK:       // 0x58
//   val[i2c_Index++] = TWDR;
   I2C_ST0P;
   i2c_stat = DEVICE_COMPLETE;
  break;

  case TW_MT_SLA_ACK:         // 0x18
  case TW_MT_DATA_ACK:        // 0x28
   
   if (i2c_Index < len) 
    {
       TWDR = data[i2c_Index++];
	   TWCR = ((1 << TWINT) | (1 << TWEN) | (0 << TWEA) | (1 << TWIE));
	} else {
     	I2C_ST0P;
        i2c_stat = DEVICE_COMPLETE;
	}
   
  break;

/*
  case TW_MT_SLA_NACK:
   I2C_ST0P;
   i2s_stat = DEVICE_ERROR;
  break;
*/

  default:
//   DBG("ERROR: TWSR=", TW_STATUS);
   I2C_ST0P;
   i2c_stat = DEVICE_ERROR;
  break;
 };
}
//------------------------------------------------------------------------
/*
void WaitInt()
{
 L0: 
 if (!(TWCR & (1 << TWINT))) goto L0;

// DBG("TWI, TWSR", TWSR);
}
*/
//------------------------------------------------------------------------
//------------------------------------------------------------------------
void I2C_Timeout_ISR(void)
{
 StopT2;
 ClearT2;
 b_i2c_Timeout = 1;
}



#include "uart.h"

void I2C_Sniffer(void)
{
 char str[32];
 UART_SendText("Start I2C Sniffer for addresses about 0x02 to 0xfe\r\n");
 unsigned char i;
 unsigned char c = 0;
 unsigned char tmp;
 for (i = 1; i < 0xfe; i = i + 1)
 {
 	if (i2c_Execute(i, &tmp, 0))
	{
 		sprintf(str, "-->found device with address: 0x%x\r\n", i);
		c++;
		UART_SendText(str);
	}
 }

 sprintf(str, "Stop sniffer. Found %i devices.\r\n", c);

 UART_SendText(str);
 
}

