#include <avr/io.h>
#include <avr/iom88.h>
#include <avr/interrupt.h>

#include "ADC.h"

//! Максимальное кол-во каналов.
#define ADC_MAX_CHANNELS 8

//! Кол-во замеров для усреднения.
#define ADC_NUMBER_OF_MEASUREMENTS 16

//! Маска активных каналов для преобразования.
BYTE adc_channel_mask;

//! Номер текущего канала.
BYTE adc_index;

//! Указатель на список каналов.
BYTE *adc_channels;

//! Кол-во каалов в списке adc_channels
//! \sa adc_channels
BYTE adc_count;

//! Список результатов замеров АЦП.
WORD adc_values[ADC_MAX_CHANNELS][ADC_NUMBER_OF_MEASUREMENTS];

//! Номер следующего замера в adc_values для канала.
BYTE adc_index_measurement[ADC_MAX_CHANNELS];


//! Устанавливает следующий канал для АЦП.
void adc_SetNextChannel();

//------------------------------------------------------------------------
void ADC_Init()
{
    ADCSRB = (0 << ADPS0) | (0 << ADPS1);
// DIDR0  = (0 << ADC0D) | (0 << ADC1D);

    adc_channel_mask = 0;
    adc_index = 0;
}
//------------------------------------------------------------------------

void ADC_Start(BYTE *channels, BYTE count)
{
    ADC_Disable;

    adc_channels = channels;
    adc_count    = count;
    adc_index    = 0;

    adc_SetNextChannel();
    // Получаем значение выбранного канала
    ADC_Enable;
    ADCSRA |= (1 << ADSC);  // Start Conversion
}
//------------------------------------------------------------------------
void ADC_Stop()
{
    adc_channel_mask = 0;
    ADC_Disable;
}

//------------------------------------------------------------------------
WORD ADC_GetValue(BYTE channel)
{
//    return adc_values[channel][0];
	if (channel < ADC_MAX_CHANNELS) 
	{
		// Высчитываем среднее значение.
		WORD res = 0;
		BYTE b;
		for (b = 0; b < ADC_NUMBER_OF_MEASUREMENTS; b++)
			res +=  adc_values[channel][b];

		return (res / ADC_NUMBER_OF_MEASUREMENTS);
	}
    return 0;
}

//------------------------------------------------------------------------
void adc_SetNextChannel()
{
    if (adc_index++ > adc_count) adc_index = 0;
    BYTE b = adc_channels[adc_index];
    if (b < ADC_MAX_CHANNELS) ADMUX = b;
}

ISR(ADC_vect)
{	
	BYTE mux = ADMUX;

    adc_values[mux][adc_index_measurement[mux]] = ADC;
//    adc_values[mux][0] = ADC;

	if (++adc_index_measurement[mux] == ADC_NUMBER_OF_MEASUREMENTS)
	{
		adc_index_measurement[mux] = 0;
		// Следующий канал замеряем только после ADC_NUMBER_OF_MEASUREMENTS замеров текущего.
	    adc_SetNextChannel();
	}
    ADCSRA |= (1 << ADSC);  // Start Conversion
}


//------------------------------------------------------------------------
//WORD ADC_To_R(WORD Val) //
//{
// long t = Val;
// t = t * 232;
// t =  (t / 100) - 15;
// if (t < 0) t = 0;
// return (WORD) t;
//}

//------------------------------------------------------------------------
//WORD R_To_Themp(WORD Val)
//{
//// long t = -Val;
//// t = t * 2;
//// t = (t / 100) + 64;
// return Val;
//}
