#ifndef __SPI_H
#define __SPI_H

 #define SS   (1 << DDB2)
 #define SCK  (1 << DDB5)
 #define MOSI (1 << DDB3)
 
 
 void SPI_Init();
 void SPI_Send(unsigned char data); 


 

#endif
