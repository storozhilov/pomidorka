#ifndef __ADC_H
#define __ADC_H

#include "defines.h"

//! Инициализация модуля ADC.
void ADC_Init();

//! Запусает процесс преобразования АЦП для каналов по маске.
//! \param channels - указатель на список каналов.
//! \param cnt - кол-во каналов в списке.
void ADC_Start(BYTE *channels, BYTE count);

//! Останавливает процесс преобразования АЦП.
void ADC_Stop();

//! Возвращает результат преобразования для канала.
//! \param channel - номер канала.
//! \return результат преобразования.
WORD ADC_GetValue(BYTE channel);


#define ADC_Enable  (ADCSRA = (1 << ADPS0) | (1 << ADPS1) | (1 << ADPS2) | (1 << ADEN) | (1 << ADIE) | (0 << ADATE) | (1 << ADIF)); // CK / 128
#define ADC_Disable (ADCSRA = (0 << ADPS0) | (0 << ADPS1) | (0 << ADPS2) | (0 << ADEN) | (0 << ADIE) | (0 << ADATE) | (1 << ADIF));


#endif
