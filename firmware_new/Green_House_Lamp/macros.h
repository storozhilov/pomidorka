//макросы для работы с битами
#define ClearBit(reg, bit)       reg &= (~(1<<(bit)))
#define SetBit(reg, bit)         reg |= (1<<(bit)) 
#define InvertBit(reg, bit)		 reg ^= (1<<(bit))
