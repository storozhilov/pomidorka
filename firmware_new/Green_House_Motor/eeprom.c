//���� ����������
#include "eeprom.h"

//������ � eeprom
void EEPROM_write(unsigned int uiAddress, unsigned char ucData)
{
/* ��� ���������� EEPROM ��� ������, ��� EEPE ������ ����� 0*/
while(EECR & (1<<EEPE));
/* �������� ����� � ������������� ������ */
EEAR = uiAddress;
EEDR = ucData;
/* ��������� ����������������� ��� EEMPE, ������� � ���� ���. 1. ����� ��� ������ �� ��������� ������*/
EECR |= (1<<EEMPE);
/* � ������� 4 ������, ����� ���������� ��� EEPE ��� ������ � EEPROM */
EECR |= (1<<EEPE);
}


//������ �� eeprom
unsigned char EEPROM_read(unsigned int uiAddress)
{
/* ��� ���������� ������ */
while(EECR & (1<<EEPE))
;
/* ������� � ������� ������ ����� */
EEAR = uiAddress;
/* ���������� ��� ������ EERE */
EECR |= (1<<EERE);
/* ���������� ���������� ������ �� �������� */
return EEDR;
}


