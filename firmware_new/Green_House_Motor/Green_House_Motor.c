 
/* ----------------------- AVR includes -------------------------------------*/

#include <avr/io.h>				//в нём содержится описание всех регитстров и портов котроллера
#include <avr/interrupt.h>		// вектора прерываний
#include <util/delay.h> 		// задержка ms или us
#include "util/atomic.h"		// для организации атомарных операций

/* ----------------------- macros includes ---------------------------------*/

#include "macros.h"

/* ----------------------- Step motor includes -----------------------------*/

#include "Step_motor.h"

/* ----------------------- adc includes ------------------------------------*/

#include "adc.h"

/* ----------------------- bcd includes ------------------------------------*/

#include "bcd.h"

/* ----------------------- LCD includes ------------------------------------*/

#include "lcd_lib.h"

/* ----------------------- Modbus includes ---------------------------------*/

#include "mb.h"
#include "mbport.h"

/* ----------------------- Defines -----------------------------------------*/

#define REG_INPUT_START 0
#define REG_INPUT_NREGS 3
#define Motor_position_MAX 32

/* ----------------------- Static variables --------------------------------*/

static USHORT   usRegInputStart = REG_INPUT_START;
static USHORT   usRegInputBuf[REG_INPUT_NREGS];//2 bytes
static unsigned char Massive_Position [8] = {0b00010000, 0b00110000, 0b00100000, 0b01100000, 0b01000000, 0b11000000, 0b10000000, 0b10010000};

/* ----------------------- Dynamic variables -------------------------------*/

unsigned char  Motor_position_modbus = 0, State = 0, i = 0, j = 0, current_state = 0, Status = 0;
unsigned int Motor_position_adc;
unsigned char Motor_position [Motor_position_MAX];

/* ----------------------- Functions ---------------------------------------*/

void T0_Init(void)
{
//счётный регистр 2ms
TCNT0 = 240;
//Частота fclk/1024 
TCCR0 |= (1<<CS02)|(1<<CS00);
//разрешение прерывания по переполнению
//TIMSK |= (1<<TOIE0);
}

eMBErrorCode
eMBRegInputCB( UCHAR * pucRegBuffer, USHORT usAddress, USHORT usNRegs )
{
eMBErrorCode    eStatus = MB_ENOERR;
int iRegIndex;

if( ( usAddress >= REG_INPUT_START ) && ( usAddress + usNRegs <= REG_INPUT_START + REG_INPUT_NREGS ) )
    {
        iRegIndex = (int)(usAddress - usRegInputStart);
		usRegInputBuf[0] = Motor_position_adc; 
        usRegInputBuf[2] = Status; 
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
      
        	usRegInputBuf[0] = Motor_position_adc;
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

/* ----------------------- Prototype tasks----------------------------------*/

/* ----------------------- Tasks--------------------------------------------*/

/* ----------------------- Main---------------------------------------------*/
int
main(void)
{
Step_Motor_Init();
ADC_Init();		
LCD_Init();
T0_Init();

const UCHAR     ucSlaveID[] = {0xAA,0xBB,0xCC};
	
eMBErrorCode    eStatus;
eStatus = eMBInit( MB_RTU, 0x0A, 0, 38400, MB_PAR_EVEN );
eStatus = eMBSetSlaveID( 0x34, TRUE, ucSlaveID, 3);
	
sei(  );

/* Enable the Modbus Protocol Stack. */
eStatus = eMBEnable (  );

for(;;)
	{

(void)eMBPoll();

Motor_position_modbus = usRegInputBuf[1];
Status = usRegInputBuf[2];
Motor_position_adc = 0;
//for (j = 0; j<8; j++) Motor_position_adc += Motor_position[j];
//Motor_position_adc = Motor_position_adc >> 3;
for (j = 0; j<Motor_position_MAX; j++) Motor_position_adc += Motor_position[j];
Motor_position_adc = Motor_position_adc >> 5;

if (Status)
{

	TIMSK |= (1<<TOIE0);

}
	else
	{
		
		TIMSK &= ~(1<<TOIE0);			// Стоп двигателя
	
	}
	
LCD_Goto(1,0);
Bcd3Lcd(Motor_position_adc);
_delay_ms(100);
LCD_Goto(1,1);
Bcd3Lcd(Motor_position_modbus);	
LCD_Goto(10,1);
Bcd3Lcd(Status);		
	
	}

  return 0;
}

/* ----------------------- Handlers interrupts ----------------------------------------*/

//Обработчик прерывания АЦП
ISR(ADC_vect) 
  {
	Motor_position[i] = ADCH;
	i++;
	if (i == Motor_position_MAX) i = 0;

}

ISR(TIMER0_OVF_vect) 
{
//счётный регистр 2ms
TCNT0 = 240;	
if (Motor_position_adc > Motor_position_modbus)	//уменьшает
{
if (Motor_position_adc < 5)			
{	
	TIMSK &= ~(1<<TOIE0);	
	Status = 0;
}	
	if (current_state >= 8) current_state = 0;	
	PORT_STEP_MOTOR &= 0x0F;
	PORT_STEP_MOTOR |= Massive_Position[current_state];
	current_state++;
}

if (Motor_position_adc < Motor_position_modbus) //увеличивает
{	
if (Motor_position_adc > 180) 	
{	
	TIMSK &= ~(1<<TOIE0);	
	Status = 0;
}	
	if (current_state == 0) current_state = 8;	
	PORT_STEP_MOTOR &= 0x0F;
	PORT_STEP_MOTOR |= Massive_Position[current_state];
	current_state--;
}

if ((Motor_position_adc+5 > Motor_position_modbus)&&(Motor_position_adc < Motor_position_modbus+5))		//Стоп двигателя 
{	

	TIMSK &= ~(1<<TOIE0);	
	Status = 0;
}




}
