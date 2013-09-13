//файл реализации
#include "eeprom.h"

//запись в eeprom
void EEPROM_write(unsigned int uiAddress, unsigned char ucData)
{
/* Ждём готовности EEPROM для записи, бит EEPE должен стать 0*/
while(EECR & (1<<EEPE));
/* Выбираем адрес и устанавливаем данные */
EEAR = uiAddress;
EEDR = ucData;
/* Взводится предохранительный бит EEMPE, записав в него лог. 1. нужно для защиты от случайной записи*/
EECR |= (1<<EEMPE);
/* В течение 4 тактов, нужно установить бит EEPE для записи в EEPROM */
EECR |= (1<<EEPE);
}


//чтение из eeprom
unsigned char EEPROM_read(unsigned int uiAddress)
{
/* Ждём готовность памяти */
while(EECR & (1<<EEPE))
;
/* Заносим в регистр нужный адрес */
EEAR = uiAddress;
/* Выставляем бит чтения EERE */
EECR |= (1<<EERE);
/* Возвращаем полученные данные из регистра */
return EEDR;
}


