
#include "ds1621.h"
#include "i2c.h"
#include "main.h"

#define AdressRead        0x91
#define AdressWrite       0x90

#define SetControl        0xac
#define StartConversion   0xee
#define ReadTemperature   0xaa


unsigned char d[2];

//------------------------------------------------------------------------
void DS_Init()
{
 d[0] = SetControl;
 d[1] = (1 << 3);
 I2C_Execute(AdressWrite, d, 2);

 d[0] = StartConversion;
 I2C_Execute(AdressWrite, d, 1);
}

//------------------------------------------------------------------------
int DS_ReadValue()
{
// DS_Init();
 BYTE res;
 d[0] = ReadTemperature;
 res = I2C_Execute(AdressWrite, d, 1);
 
 d[0] = StartConversion;
 if (res && I2C_Execute(AdressRead, d, 2))  return (d[0]);

 return (-100);
}
//------------------------------------------------------------------------
