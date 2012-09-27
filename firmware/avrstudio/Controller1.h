#ifndef __controller_h
#define __controller_h


#define LOAD1               (1 << DDB1) //!< �������� 1.
#define LOAD1_DIRECTION     (1 << DDB0) //!< ���������� ������������ �������� 1.
#define LOAD2               (1 << DDC1) //!< �������� 2.    // Led Green
#define LOAD3               (1 << DDC2) //!< �������� 3.    // Led Red
#define LOAD4

#define LOAD1_ON            PORTB |= LOAD1
#define LOAD1_OFF           PORTB &= ~LOAD1
#define LOAD1_DIRECTION_ON  PORTB |= LOAD1_DIRECTION
#define LOAD1_DIRECTION_OFF PORTB &= ~LOAD1_DIRECTION
#define LOAD1_STATUS        PORTB & LOAD1

#define LOAD2_ON            PORTC |= LOAD2
#define LOAD2_OFF           PORTC &= ~LOAD2
#define LOAD2_STATUS        PORTC & LOAD2

#define LOAD3_ON            PORTC |= LOAD3
#define LOAD3_OFF           PORTC &= ~LOAD3
#define LOAD3_STATUS        PORTC & LOAD3

#define LOAD4_ON            PORTn |= LOAD4
#define LOAD4_OFF           PORTn &= ~LOAD4
#define LOAD4_STATUS        PORTC & LOAD4


#define IN1                 (1 << DDD2) //!< ���� 1, �������� 1.
#define IN2                 (1 << DDC3) //!< ���� 2, �������� 2.
#define IN3                 (1 << DDD3) //!< ���� 3, �������� ���������� 220.
#define IN4                 (1 << DDD4) //!< ���� 4, ������ System.
#define IN5                 (1 << DDD6) //!< ���� 5. ������ UP.
#define IN6                 (1 << DDD5) //!< ���� 6. ������ DOWN.

#define IS_IN1              (PIND & IN1) //!< ��������� ����� 1
#define IS_IN2              (PINC & IN2) //!< ��������� ����� 2
#define IS_IN3              (PIND & IN3) //!< ��������� ����� 3
#define IS_IN4              (PIND & IN4) //!< ��������� ����� 4
#define IS_IN5              (PIND & IN5) //!< ��������� ����� 5
#define IS_IN6              (PIND & IN6) //!< ��������� ����� 6

#define TX_RX               (1 << DDD7)  //!< ���������� ��������� � RS-485.
#define TX_ON               PORTD |= TX_RX  //!< �������� ��������.
#define RX_ON               PORTD &= ~TX_RX //!< ��������� ��������.



//#define BEEPER              (1 << DDB0)
//#define STOP_RELE           (1 << DDB1)
//#define LED_GREEN           (1 << DDC1)   // Green
//#define LED_RED             (1 << DDC2)   // Red

//#define TYPE                (1 << DDC3)
//#define Gerkon              (1 << DDD2)
//#define Start               (1 << DDD3)

//#define BUT1                (1 << DDD4) // System
//#define BUT_Up              (1 << DDD6) // Up
//#define BUT_Down            (1 << DDD5) // Down


//#define BEEP_ON             PORTB |= BEEPER
//#define BEEP_OFF            PORTB &= ~BEEPER
//#define STOP_ON             PORTB &= ~STOP_RELE;
//#define STOP_OFF            PORTB |= STOP_RELE;

//#define LED_GREEN_ON        PORTC &= ~LED_GREEN
//#define LED_GREEN_OFF       PORTC |= LED_GREEN
//#define LED_GREEN_CHANGE    if (PORTC & LED_GREEN) PORTC &= ~LED_GREEN; else PORTC |= LED_GREEN

//#define LED_RED_ON          PORTC &= ~LED_RED
//#define LED_RED_OFF         PORTC |= LED_RED
//#define LED_RED_CHANGE      if (PORTC & LED_RED) PORTC |= LED_RED; else PORTC &= ~LED_RED


//#define BUT1_PRESSED        (!(PIND & BUT1))
//#define BUT_Up_PRESSED      (!(PIND & BUT_Up))
//#define BUT_Down_PRESSED    (!(PIND & BUT_Down))
//#define IS_START            (!(PIND & Start))



#endif

