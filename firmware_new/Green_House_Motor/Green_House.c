
/* ----------------------- AVR includes ------------------------------------*/

#include <avr/io.h>				//� �� ���������� �������� ���� ���������� � ������ ����������
#include <avr/interrupt.h>		// ������� ����������
#include <util/delay.h> 		// �������� ms ��� us
#include "util/atomic.h"		// ��� ����������� ��������� ��������

/* ----------------------- Modbus includes ---------------------------------*/
#include "mb.h"
#include "mbport.h"

/* ----------------------- Step motor includes -----------------------------*/

#include "Step_motor.h"

/* ----------------------- ADC includes ------------------------------------*/
#include "adc.h"

/* ----------------------- macros includes ---------------------------------*/
#include "macros.h"

/* ----------------------- EEPROM includes ---------------------------------*/
#include "eeprom.h"

/* ----------------------- Defines -----------------------------------------*/

#define REG_INPUT_START 		0
#define REG_INPUT_NREGS 		5
#define Status	 				EEPROM_read(0x00)
#define Motor_position_adc		usRegInputBuf[0]
#define Motor_position_modbus	usRegInputBuf[1]
#define Status_temp				usRegInputBuf[2]
#define measured_U 				usRegInputBuf[3]
#define measured_I 				usRegInputBuf[4]
#define Motor_position_MAX 		32
#define StartConvAdc()  		ADCSRA |= (1<<ADSC)

//Status = (X)_(X)_(X)_(X)_(Motor_run_or_stop)_(measured_U)_(measured_I)

/* ----------------------- Static variables --------------------------------*/

static USHORT					usRegInputStart = REG_INPUT_START;
static USHORT					usRegInputBuf[REG_INPUT_NREGS];//2 bytes
static unsigned char			Massive_Position [8] = {0b00010000, 0b00110000, 0b00100000, 0b01100000, 0b01000000, 0b11000000, 0b10000000, 0b10010000};

/* ----------------------- Dynamic variables --------------------------------*/

unsigned char					State = 0, current_state = 0, i = 0, j = 0, EEPROM_value = 0;
//unsigned int					Motor_position_adc;
unsigned char					Motor_position [Motor_position_MAX];

/* ----------------------- Functions ----------------------------------------*/

void T0_Init(void)
{
//�������� ����� ���������� ����������� �������� 2ms
//rpm = 15/ms ~7rpm 
//������� ������� 2ms 
TCNT0 = 240;
//������� fclk/1024 
TCCR0B |= (1<<CS02)|(1<<CS00);
//���������� ���������� �� ������������
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

/* ----------------------- Start implementation -----------------------------*/
int
main(void)
{
Step_Motor_Init();
ADC_Init();		
T0_Init();

SetBit(DDRD,2); //MAX3485 �� �����

const UCHAR     ucSlaveID[] = {0xAA,0xBB,0xCC};
	
eMBErrorCode    eStatus;
eStatus = eMBInit( MB_RTU, 0x01, 0, 38400, MB_PAR_EVEN );
eStatus = eMBSetSlaveID( 0x34, TRUE, ucSlaveID, 3);
	
sei(  );

/* Enable the Modbus Protocol Stack. */
eStatus = eMBEnable (  );
	for(;;)
		{

		(void)eMBPoll();

		Motor_position_modbus = usRegInputBuf[1];
		Status_temp = usRegInputBuf[2];
		
		Motor_position_adc = 0;
		for (j = 0; j<Motor_position_MAX; j++) Motor_position_adc += Motor_position[j];
		Motor_position_adc = Motor_position_adc >> 5;
		
		if (Status != Status_temp) EEPROM_write(0x00, Status_temp);
		
			if ((Status_temp & (1<<2)) != 0) 		// ���� ������ ��� ����� 1, �� ��������� ����������, � ����������� �� motor_position_modbus 
				{
					TIMSK0 |= (1<<TOIE0);
				}
			else
				{
					TIMSK0 &= ~(1<<TOIE0);			// ���� ������ ��� ����� 0, �� ��������� �� ����������
				}
	
		}
}

/* ----------------------- Handlers interrupts ----------------------------------------*/
//���������� ���������� ���
ISR(ADC_vect) 
{
	
	switch (State)
		{
		//������� �����
		case 0:
		Motor_position[i] = ADCH;
		i++;
		if (i == Motor_position_MAX) i = 0;
		// �������� ������ �����
		ADMUX |= (1<<MUX0);
		State++;
		StartConvAdc();
		break;

		//������ ����� 
		case 1:
		measured_U = ADCH;
		Status_temp = Status;
		if ((measured_U < 135)&&((Status_temp & (1<<1)) == 0)) //3,5V �����; ����� 2,46V ��� 190��.���
		{
			Status_temp |= (1<<1);
			EEPROM_write(0x00, Status_temp);
		}
		// �������� ������ �����
		ADMUX |= (1<<MUX1);
		ADMUX &= (~(1<<MUX0));
		State++;
		StartConvAdc();
		break;

		//������ �����
		case 2:
		measured_I = ADCH;
		Status_temp = Status;
		if ((measured_I > 30)&&((Status_temp & (1<<0)) == 0)) // 350mA �����; ��� ����� 12 ��� 22 ��.��� ��� 140mA, � ������� ��� 15��.��� ��� 163mA 
			{
				TIMSK0 &= ~(1<<TOIE0);							//���� ��������� 				
				Status_temp |= (1<<0);
				EEPROM_write(0x00, Status_temp);
 			}
		//�������� ������� �����
		ADMUX &= (~(1<<MUX1));
		State = 0;
		StartConvAdc();
		}

}

ISR(TIMER0_OVF_vect) 
{
//������� ������� 2ms
TCNT0 = 240;	
Status_temp = Status;

if (Motor_position_adc > Motor_position_modbus)		//���������
	{
		if (Motor_position_adc < 5)						
			{	
				TIMSK0 &= ~(1<<TOIE0);				//���� ��������� 
				Status_temp &= ~(1<<2);				
				EEPROM_write(0x00, Status_temp);	//������ ����, ��� ��������� �������� � eeprom
			}	
	if (current_state >= 8) current_state = 0;	
	
	PORT_STEP_MOTOR &= 0x0F;
	PORT_STEP_MOTOR |= Massive_Position[current_state];
	current_state++;
//	if (current_state == 0) current_state = 8;	
//	PORT_STEP_MOTOR &= 0x0F;
//	PORT_STEP_MOTOR |= Massive_Position[current_state];
//	current_state--;



	}

if (Motor_position_adc < Motor_position_modbus) 	//�����������
	{	
		if (Motor_position_adc > 100) 	
			{	
				TIMSK0 &= ~(1<<TOIE0);				//���� ��������� 	
				Status_temp &= ~(1<<2);
				EEPROM_write(0x00, Status_temp);	//������ ����, ��� ��������� �������� � eeprom
			}	

	
	if (current_state == 0) current_state = 8;		
	
	PORT_STEP_MOTOR &= 0x0F;
	PORT_STEP_MOTOR |= Massive_Position[current_state];
	current_state--;

//	if (current_state >= 8) current_state = 0;
//	PORT_STEP_MOTOR &= 0x0F;
//	PORT_STEP_MOTOR |= Massive_Position[current_state];
//	current_state++;



	}

if ((Motor_position_adc+5 > Motor_position_modbus)&&(Motor_position_adc < Motor_position_modbus+5))		//���� ��������� 
{	

	TIMSK0 &= ~(1<<TOIE0);							//���� ��������� 
	Status_temp &= ~(1<<2);
	EEPROM_write(0x00, Status_temp);				//������ ����, ��� ��������� �������� � eeprom
}

}



