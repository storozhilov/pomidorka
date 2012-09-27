#ifndef __ADC_H
#define __ADC_H

#include "defines.h"

//! ������������� ������ ADC.
void ADC_Init();

//! �������� ������� �������������� ��� ��� ������� �� �����.
//! \param channels - ��������� �� ������ �������.
//! \param cnt - ���-�� ������� � ������.
void ADC_Start(BYTE *channels, BYTE count);

//! ������������� ������� �������������� ���.
void ADC_Stop();

//! ���������� ��������� �������������� ��� ������.
//! \param channel - ����� ������.
//! \return ��������� ��������������.
WORD ADC_GetValue(BYTE channel);


#define ADC_Enable  (ADCSRA = (1 << ADPS0) | (1 << ADPS1) | (1 << ADPS2) | (1 << ADEN) | (1 << ADIE) | (0 << ADATE) | (1 << ADIF)); // CK / 128
#define ADC_Disable (ADCSRA = (0 << ADPS0) | (0 << ADPS1) | (0 << ADPS2) | (0 << ADEN) | (0 << ADIE) | (0 << ADATE) | (1 << ADIF));


#endif
