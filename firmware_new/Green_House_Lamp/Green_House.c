
/* ----------------------- AVR includes -------------------------------------*/
#include "avr/io.h"
#include "avr/interrupt.h"
#include <util/delay.h> 

/* ----------------------- Modbus includes ----------------------------------*/
#include "mb.h"
#include "mbport.h"

/* ----------------------- LCD includes ------------------------------------*/
//#include "bcd.h"
//#include "lcd_lib.h"

/* ----------------------- ADC includes ------------------------------------*/
#include "adc.h"

/* ----------------------- macros includes ------------------------------------*/
#include "macros.h"

/* ----------------------- EEPROM includes ------------------------------------*/
#include "eeprom.h"

/* ----------------------- Defines ------------------------------------------*/
#define 					REG_INPUT_START 0
#define REG_INPUT_NREGS 	5
#define StartConvAdc()  	ADCSRA |= (1<<ADSC)

/* ----------------------- Static variables ---------------------------------*/
static USHORT   usRegInputStart = REG_INPUT_START;
static USHORT   usRegInputBuf[REG_INPUT_NREGS];//2 bytes


#define Status 						EEPROM_read(0x00)
#define Status_temp					usRegInputBuf[0]
#define OCR_temp					usRegInputBuf[1]
#define illumination_intensity  	usRegInputBuf[2]
#define measured_U 					usRegInputBuf[3]
#define measured_I 					usRegInputBuf[4]


/* ----------------------- Dynamic variables --------------------------------*/
//volatile unsigned char AdcBuf;
char EEPROM_value;
unsigned char State = 0/*,OCR_value, illumination_intensity, measured_U, measured_I, Status_temp = 0*/;

/* ----------------------- Functions ----------------------------------------*/
void PWM_Init(void)
{
//Вывод порта ШИМа на вход
//SetBit(DDRD,3); 
//Обнуляем счётный регистр
TCNT2 = 0;
//FAST PWM, сброс при совпадении, частота fclk/256 
TCCR2B = /*(1<<WGM21)|(1<<WGM20)|(1<<COM21)|(0<<COM20)|*/(1<<CS22)|(1<<CS21)|(0<<CS20);
TCCR2A = (1<<WGM21)|(1<<WGM20)|(1<<COM2B1)|(0<<COM2B0);
//разрешение прерывания от таймера2
//TIMSK = (1<<OCIE2);
}

/* ----------------------- Start implementation -----------------------------*/
int
main(void)
{
	ADC_Init();		//Инициализация АЦП
Status_temp = Status;
//	SetBit(DDRD, 5);
//	SetBit(PORTD, 5);
SetBit(DDRD,2);		//MAX3485
//DDRB |= (1<<0)|(1<<1)|(1<<2)|(1<<6)|(1<<7);	
	const UCHAR     ucSlaveID[] = {0xAA,0xBB,0xCC};
	
	eMBErrorCode    eStatus;
	eStatus = eMBInit( MB_RTU, 0x01, 0, 38400, MB_PAR_EVEN/*MB_PAR_NONE*/ );
	eStatus = eMBSetSlaveID( 0x34, TRUE, ucSlaveID, 3);
	
	sei(  );

	/* Enable the Modbus Protocol Stack. */
	eStatus = eMBEnable (  );
	
	PWM_Init();		//Инициализация ШИМа
	OCR2B = 0;
	for(;;)
		{
			(void)eMBPoll();
			/* Here we simply count the number of poll cycles. */
			if (Status != Status_temp) EEPROM_write(0x00, Status_temp);
			if (Status == 0) 
			{
				if (OCR_temp) 
				{
					OCR2B = OCR_temp;
//					TCCR2B |= (1<<CS22)|(1<<CS21)|(0<<CS20);
					SetBit(DDRD,3); 				
				}
				else
				{
//					TCCR2B &= ~(1<<CS22)|(1<<CS21)|(1<<CS20);
					ClearBit(DDRD,3);				
				}
			
			}
			else	
			{
//				TCCR2B &= ~(1<<CS22)|(1<<CS21)|(1<<CS20);
				ClearBit(DDRD,3);
			}  //OCR2B = 0;
//InvertBit(PORTB,7);



		}
}

/*eMBErrorCode
eMBRegInputCB( UCHAR * pucRegBuffer, USHORT usAddress, USHORT usNRegs )
{
eMBErrorCode  eStatus = MB_ENOERR;
	if( ( usAddress == 1 ) && ( usNRegs == 4 ) ) 
	{
		Status = EEPROM_read(0x00);
	 
		*pucRegBuffer++ = 0;
		*pucRegBuffer++ = Status;
	  
		*pucRegBuffer++ = 0;
		*pucRegBuffer++ = illumination_intensity;
  	  
		*pucRegBuffer++ = 0;
		*pucRegBuffer++ = measured_U;
      
		*pucRegBuffer++ = 0;
		*pucRegBuffer++ = measured_I;
	}
	else 
	{
		eStatus = MB_ENOREG;
	}
return eStatus;
}*/

eMBErrorCode
eMBRegInputCB( UCHAR * pucRegBuffer, USHORT usAddress, USHORT usNRegs )
{
eMBErrorCode    eStatus = MB_ENOERR;
int iRegIndex;

if( ( usAddress >= REG_INPUT_START ) && ( usAddress + usNRegs <= REG_INPUT_START + REG_INPUT_NREGS ) )
    {
        iRegIndex = (int)(usAddress - usRegInputStart);
		usRegInputBuf[0] = Status; 
        while(usNRegs > 0)
        {
            *pucRegBuffer++ = (unsigned char)( usRegInputBuf[iRegIndex] >> 8 );
            *pucRegBuffer++ = (unsigned char)( usRegInputBuf[iRegIndex] & 0xFF );
            iRegIndex++;
            usNRegs--;
        }
    }
    else
    {
        eStatus = MB_ETIMEDOUT;
    }

    return eStatus;
}

   
/*eMBErrorCode
eMBRegHoldingCB( UCHAR * pucRegBuffer, USHORT usAddress, USHORT usNRegs,
                 eMBRegisterMode eMode )
{
    eMBErrorCode  eStatus = MB_ENOERR; 
  if( (usAddress == 1 ) && ( usNRegs == 2) ) {
    if( eMode == MB_REG_READ ) {
	  *pucRegBuffer++ = 0;
      *pucRegBuffer++ = Status;
    }  
    else {
      pucRegBuffer++;
      Status_temp = (*pucRegBuffer++);
	  OCR2 = (*pucRegBuffer++);
	  EEPROM_write(0x00, Status_temp);
    }
  }
  else {
    eStatus = MB_ENOREG;
  }
  return eStatus;
}
*/
eMBErrorCode
eMBRegHoldingCB( UCHAR * pucRegBuffer, USHORT usAddress, USHORT usNRegs,
                 eMBRegisterMode eMode )
{
	eMBErrorCode    eStatus = MB_ENOERR;
	int iRegIndex;

	if( ( usAddress >= REG_INPUT_START ) && ( usAddress + usNRegs <= REG_INPUT_START + REG_INPUT_NREGS ) )
	{
   		iRegIndex = (int)(usAddress - usRegInputStart);
		if( eMode == MB_REG_READ ) 
   		{
      
        	usRegInputBuf[0] = Status;
			while(usNRegs > 0)
        	{
            	*pucRegBuffer++ = (unsigned char)( usRegInputBuf[iRegIndex] >> 8 );
            	*pucRegBuffer++ = (unsigned char)( usRegInputBuf[iRegIndex] & 0xFF );
            	iRegIndex++;
            	usNRegs--;
        	}
		}
		else
		{
      		while(usNRegs > 0)
        	{
				pucRegBuffer++;
				usRegInputBuf[iRegIndex] = *pucRegBuffer;
				pucRegBuffer++;
				usNRegs--;
        	}
    	}
	}
    else
    {
        eStatus = MB_ETIMEDOUT;
    }

    return eStatus;
}




eMBErrorCode
eMBRegCoilsCB( UCHAR * pucRegBuffer, USHORT usAddress, USHORT usNCoils,
               eMBRegisterMode eMode )
{
    return MB_ENOREG;
}

eMBErrorCode
eMBRegDiscreteCB( UCHAR * pucRegBuffer, USHORT usAddress, USHORT usNDiscrete )
{
    return MB_ENOREG;
}

/* ----------------------- Handlers interrupts ----------------------------------------*/
//Обработчик прерывания АЦП
ISR(ADC_vect) 
{
switch (State)
{
// нулевой канал
case 0:
//illumination_intensity = ADCH;
illumination_intensity = 255 - ADCH ;
//OCR2 = illumination_intensity;
// выбираем первый канал
ADMUX |= (1<<MUX0);
State++;
StartConvAdc();
break;
//первый канал 
case 1:
measured_U = ADCH;
Status_temp = Status;
if ((measured_U < 110)&&((Status_temp & (1<<1)) == 0)) //1,15V ловим
{

	Status_temp |= (1<<1);
	EEPROM_write(0x00, Status_temp);
}
// выбираем второй канал
ADMUX |= (1<<MUX1);
ADMUX &= (~(1<<MUX0));
State++;
StartConvAdc();
break;
//второй канал
case 2:
measured_I = ADCH;
Status_temp = Status;
if ((measured_I > 75)&&((Status_temp & (1<<0)) == 0))  
{

//	ClearBit(DDRD,3);
	Status_temp |= (1<<0);
	EEPROM_write(0x00, Status_temp);
}
//выбираем нулевой канал
ADMUX &= (~(1<<MUX1));
State = 0;
StartConvAdc();
}

}





