//������������ ����� ����� ����������
#ifndef eeprom_h
#define eeprom_h

#include <avr/io.h>//� ��� ���������� �������� ���� ��������� � ������ �����������
#include <util/delay.h>


void EEPROM_write(unsigned int uiAddress, unsigned char ucData);

unsigned char EEPROM_read(unsigned int uiAddress);

#endif
