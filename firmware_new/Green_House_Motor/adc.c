//���� ����������
#include "adc.h"

////������ ��� ������� �������������� 
#define StartConvAdc()  ADCSRA |= (1<<ADSC)

void ADC_Init(void)
{
//��� - ���������� Avcc, ������������ �����, ������� �����
ADMUX = (0<<REFS1)|(1<<REFS0)|(1<<ADLAR)/*|(0<<MUX4)*/|(0<<MUX3)|(0<<MUX2)|(0<<MUX1)|(0<<MUX0);
//���. ���, ����� ������������ ������., ���������� ����������,������� ������. = FCPU/128
ADCSRA = (1<<ADEN)|(0<<ADSC)|(0<<ADATE)|(1<<ADIE)|(1<<ADPS2)|(1<<ADPS1)|(1<<ADPS0);
StartConvAdc();
}
