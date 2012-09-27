#include <avr/io.h>
#include <avr/interrupt.h>

#include "DS18B20.h"

#include "1-wire/OWIPolled.h"
#include "1-wire/OWIHighLevelFunctions.h"
#include "1-wire/OWIBitFunctions.h"
#include "1-wire/common_files/OWIcrc.h"

BYTE ds18b20_rom[8];
OWI_device ds18b20_AllDevices[MAX_DEVICES];

WORD ds18b20_temperature[MAX_DEVICES];

unsigned char ds18b20_searchFlag = SEARCH_SENSORS;
unsigned char ds18b20_crcFlag = 0;
unsigned char ds18b20_num = 0;

BYTE DS18B20_ReadTemperature(BYTE bus, BYTE *id, WORD* ds18b20_temperature);

void DS18B20_Init()
{
      OWI_Init(BUS);
}

WORD DS18B20_GetTemparature(BYTE val)
{
    if (val < MAX_DEVICES) return ds18b20_temperature[val];
    return 0;
}

void DS18B20_Execute()
{
    /*
    -���� ������� - ��������� ����� 1Wire ���������
    -���� ���������� �������� �������� ��������� �
    ������������ ��������� - ������������� ����,
    ����� ������� ������ ������ �� �����������
    */

    if (ds18b20_searchFlag == SEARCH_SENSORS){
          ds18b20_num = 0;
          ds18b20_crcFlag = OWI_SearchDevices(ds18b20_AllDevices, MAX_DEVICES, BUS, &ds18b20_num);
          if ((ds18b20_num == MAX_DEVICES)&&(ds18b20_crcFlag != SEARCH_CRC_ERROR)){
             ds18b20_searchFlag = SENSORS_FOUND;
          }
    }

    /*
       ������� ����������� ������� ���������� �������
       ���� ���������� ������ CRC, ������� ������ "not"
       � �������� ����, ����� ������� ������ ����� ���������
       ����� ����
    */

    ds18b20_crcFlag = DS18B20_ReadTemperature(BUS, ds18b20_AllDevices[0].id, &(ds18b20_temperature[0]));
    if (ds18b20_crcFlag != READ_CRC_ERROR){
//        DS18B20_PrintTemperature(ds18b20_temperature);
    }
    else{
        ds18b20_searchFlag = SEARCH_SENSORS;
    }

    /*
        ������� ����������� ������� ���������� �������
        ���� ���������� ������ CRC, ������� ������ "not"
        � �������� ����, ����� ������� ������ ����� ���������
        ����� ����
    */

//    LCD_Goto(7,1);
    ds18b20_crcFlag = DS18B20_ReadTemperature(BUS, ds18b20_AllDevices[1].id, &(ds18b20_temperature[1]));
    if (ds18b20_crcFlag != READ_CRC_ERROR){
//        DS18B20_PrintTemperature(ds18b20_temperature);
    }
    else{
//        LCD_SendString("  not  ");
        ds18b20_searchFlag = SEARCH_SENSORS;
    }

}


/*****************************************************************************
*   Function name :   DS18B20_ReadTemperature
*   Returns :       ���� - READ_CRC_ERROR, ���� ��������� ������ �� ������ ��������
*                          READ_SUCCESSFUL, ���� ������ ������ ��������
*   Parameters :    bus - ����� ����������������, ������� ��������� ���� 1WIRE ����
*                   *id - ��� ������� �� 8-�� ���������, � ������� ��������
*                         ����� ������� DS18B20
*                   *ds18b20_temperature - ��������� �� ����������� ��������� ����������
*                                � ������� ����� ��������� ���������� ��. �����������
*   Purpose :      �������� ������ DS18B20, ���� ������� �� �������������� �����������
*                  ����, ��������� ��� ������ - scratchpad, ��������� CRC,
*                  ��������� �������� ����������� � ����������, ���������� ��� ������
*****************************************************************************/
BYTE DS18B20_ReadTemperature(BYTE bus, BYTE * id, WORD* ds18b20_temperature)
{
    unsigned char scratchpad[9];
    unsigned char i;

    /*������ ������ ������
    ������� ��� ��������� ���������� �� ����
    ������ ������� - ����� �������������� */
    OWI_DetectPresence(bus);
    OWI_MatchRom(id, bus);
    OWI_SendByte(DS18B20_CONVERT_T ,bus);

    /*����, ����� ������ �������� ��������������*/
    while (!OWI_ReadBit(bus));

    /*������ ������ ������
    ������� ��� ��������� ���������� �� ����
    ������� - ������ ���������� ������
    ����� ��������� ���������� ������ ������� � ������
    */
    OWI_DetectPresence(bus);
    OWI_MatchRom(id, bus);
    OWI_SendByte(DS18B20_READ_SCRATCHPAD, bus);
    for (i = 0; i<=8; i++){
      scratchpad[i] = OWI_ReceiveByte(bus);
    }

    if(OWI_CheckScratchPadCRC(scratchpad) != OWI_CRC_OK){
      return READ_CRC_ERROR;
    }


    *ds18b20_temperature = MAKEWORD(scratchpad[0], scratchpad[1]);


    return READ_SUCCESSFUL;
}


