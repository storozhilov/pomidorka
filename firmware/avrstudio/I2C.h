#ifndef __I2C_H
#define __I2C_H

 
 #define Get PORTC &= ~(1 << DDC4);
 #define Put PORTC |= (1 << DDC4);
 
 #define I2C_START   TWCR = ((1 << TWINT) | (1 << TWSTA) | (1 << TWEN) | (1 << TWIE));
 #define I2C_ST0P    TWCR = ((1 << TWINT) | (1 << TWSTO) | (1 << TWEN) | (1 << TWIE));
// #define WaitInt()   while (!(TWCR & (1 << TWINT))) ;
// #define IsStart()   if ((TWSR & 0xF8) != TW_START) goto Error;
 #define AckNo(x)     ((TWSR & 0xF8) != x)
 #define AckIs(x)     ((TWSR & 0xF8) == x)



 #define DEVICE_START        0x00
 #define DEVICE_EXECUTE      0x01
 #define DEVICE_COMPLETE     0x02
 #define DEVICE_ERROR        0x03

 //unsigned char i2c_Execute(unsigned char Addr, unsigned char *val, unsigned char Len);

//! ������ ������ � ���������� I2C.
//! \param Addr - ����� ����������.
//! \param Data - ������.
//! \param Len - ����� ������.
//! \return 1 � ������ ������, ����� 0.
unsigned char  I2C_WriteData(unsigned char Addr, unsigned char *Data, unsigned char Len);

//! ������ ������ �� ���������� I2C.
//! \param Addr - ����� ����������.
//! \param Data - ������.
//! \param Len - ����� ������.
//! \return 1 � ������ ������, ����� 0.
unsigned char I2C_ReadData(unsigned char Addr, unsigned char *Data, unsigned char Len);

//! ����� ����� ���������� ��� ������� �������� �� ������ ����������.
void I2C_Timeout_ISR(void);

//! ��������� ����� ��������� �� I2C, � ������ � ������� ���� ��������� �������.
void I2C_Sniffer(void);



#endif
