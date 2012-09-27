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

//! Запись данных в устройство I2C.
//! \param Addr - адрес устройства.
//! \param Data - данные.
//! \param Len - длина данных.
//! \return 1 в случае успеха, иначе 0.
unsigned char  I2C_WriteData(unsigned char Addr, unsigned char *Data, unsigned char Len);

//! Чтение данных из устройства I2C.
//! \param Addr - адрес устройства.
//! \param Data - данные.
//! \param Len - длина данных.
//! \return 1 в случае успеха, иначе 0.
unsigned char I2C_ReadData(unsigned char Addr, unsigned char *Data, unsigned char Len);

//! Точка входа прерывание для таймера слежения за длиной транзакции.
void I2C_Timeout_ISR(void);

//! Запускает поиск устройств по I2C, и вывода в консоль УАРТ найденных адресов.
void I2C_Sniffer(void);



#endif
