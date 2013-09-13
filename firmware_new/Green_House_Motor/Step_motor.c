#include "Step_motor.h"
#include <util/delay.h> 		

void Step_Motor_Init (void)
{
PORT_STEP_MOTOR = 0x00;		
DDR_STEP_MOTOR |= (1<<4)|(1<<5)|(1<<6)|(1<<7);		//порт на выход		
}
/*
void Step_Motor_clockwise (unsigned char rpm)
{
	unsigned char ms;
	ms = 15/rpm;
	PORT_STEP_MOTOR = 0b00000001;
	_delay_ms(ms);
	PORT_STEP_MOTOR = 0b00000011;
	_delay_ms(ms);
	PORT_STEP_MOTOR = 0b00000010;
	_delay_ms(ms);
	PORT_STEP_MOTOR = 0b00000110;
	_delay_ms(ms);
	PORT_STEP_MOTOR = 0b00000100;
	_delay_ms(ms);
	PORT_STEP_MOTOR = 0b00001100;
	_delay_ms(ms);
	PORT_STEP_MOTOR = 0b00001000;
	_delay_ms(ms);
	PORT_STEP_MOTOR = 0b00001001;
	_delay_ms(ms);
}

void Step_Motor_anticlockwise (unsigned char rpm)
{
	unsigned char ms;
	ms = 15/rpm;
	PORT_STEP_MOTOR = 0b00001001;
	_delay_ms(ms);
	PORT_STEP_MOTOR = 0b00001000;
	_delay_ms(ms);
	PORT_STEP_MOTOR = 0b00001100;
	_delay_ms(ms);
	PORT_STEP_MOTOR = 0b00000100;
	_delay_ms(ms);
	PORT_STEP_MOTOR = 0b00000110;
	_delay_ms(ms);
	PORT_STEP_MOTOR = 0b00000010;
	_delay_ms(ms);
	PORT_STEP_MOTOR = 0b00000011;
	_delay_ms(ms);
	PORT_STEP_MOTOR = 0b00000001;
	_delay_ms(ms);
}
*/
