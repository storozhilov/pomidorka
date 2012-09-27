
#include "24lc64.h"
#include "i2c.h"
#include "defines.h"

//------------------------------------------------------------------------
BYTE EE_ReadPage(BYTE NumPage)
{
 int offz = NumPage * df_EE_PAGE_SIZE;
 EE_PAGE[0] = HIBYTE (offz);
 EE_PAGE[1] = LOBYTE (offz);
 
 if ((I2C_Execute(EEAdressWrite, EE_PAGE, 2)) && (I2C_Execute(EEAdressRead, &(EE_PAGE[2]), df_EE_PAGE_SIZE)))
   {
	return 1;
   }
 return 0;
}
//------------------------------------------------------------------------
unsigned char EE_WritePage(BYTE NumPage)
{ 
 int offz = NumPage * df_EE_PAGE_SIZE;
 EE_PAGE[0] = HIBYTE (offz);
 EE_PAGE[1] = LOBYTE (offz);
 if (I2C_Execute(EEAdressWrite, EE_PAGE, 2 + df_EE_PAGE_SIZE)) return 1;
 return 0;
}
//------------------------------------------------------------------------
BYTE EE_ReadConfig()
{
 BYTE *b = (BYTE *) &EE_24lc64;
 EE_24lc64.AddrH = 0x00;
 EE_24lc64.AddrL = 0x00;
 
 if ((I2C_Execute(EEAdressWrite, b, 2)) && (I2C_Execute(EEAdressRead, &(b[2]), sizeof(EE_24lc64)-2)))
   {
	
	// Если это первый запуск - стираем все данные
	 if ((EE_24lc64.EE_TotalGerkon == 0xffffffff) &&  (EE_24lc64.EE_ParameterMask == 0xff) && EE_24lc64.EE_lcdParameter == 0xff)
	  {
       // Делаем полную очистку блока 0.
        BYTE t;
		for (t = 2; t < df_EE_PAGE_SIZE+2; t++)
		 {
		  EE_PAGE[t] = 0x00;
		 } 
		 EE_WritePage(0);
	  }
	
	return 1;
   }
 return 0;
}
//------------------------------------------------------------------------
BYTE EE_WriteConfig()
{
 EE_24lc64.AddrH = 0x00;
 EE_24lc64.AddrL = 0x00;
 EE_24lc64.EE_TotalRewriteEE++;
 BYTE *b = (BYTE *) &EE_24lc64;

 if (I2C_Execute(EEAdressWrite, b, sizeof(EE_24lc64))) return 1;

 return 0;
}
//------------------------------------------------------------------------
//------------------------------------------------------------------------
