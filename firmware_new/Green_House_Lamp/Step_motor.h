#ifndef	STEP_MOTOR_h
#define	STEP_MOTOR_h

#include <avr/io.h>

//порт, к которому подключен мотор
#define PORT_STEP_MOTOR PORTD
#define DDR_STEP_MOTOR 	DDRD
#define PIN_STEP_MOTOR 	PIND

void Step_Motor_Init (void);
void Step_Motor_clockwise (unsigned char rpm);
void Step_Motor_anticlockwise (unsigned char rpm);

#endif 
