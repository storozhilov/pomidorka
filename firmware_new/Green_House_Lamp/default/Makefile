###############################################################################
# Makefile for the project Green_House
###############################################################################

## General Flags
PROJECT = Green_House
MCU = atmega168p
TARGET = Green_House.elf
CC = avr-gcc

CPP = avr-g++

## Options common to compile, link and assembly rules
COMMON = -mmcu=$(MCU)

## Compile options common for all C compilation units.
CFLAGS = $(COMMON)
CFLAGS += -Wall -gdwarf-2 -std=gnu99               -DF_CPU=8000000UL -Os -funsigned-char -funsigned-bitfields -fpack-struct -fshort-enums
CFLAGS += -MD -MP -MT $(*F).o -MF dep/$(@F).d 

## Assembly specific flags
ASMFLAGS = $(COMMON)
ASMFLAGS += $(CFLAGS)
ASMFLAGS += -x assembler-with-cpp -Wa,-gdwarf2

## Linker flags
LDFLAGS = $(COMMON)
LDFLAGS +=  -Wl,-Map=Green_House.map


## Intel Hex file production flags
HEX_FLASH_FLAGS = -R .eeprom -R .fuse -R .lock -R .signature

HEX_EEPROM_FLAGS = -j .eeprom
HEX_EEPROM_FLAGS += --set-section-flags=.eeprom="alloc,load"
HEX_EEPROM_FLAGS += --change-section-lma .eeprom=0 --no-change-warnings


## Objects that must be built in order to link
OBJECTS = Green_House.o adc.o eeprom.o mb.o mbascii.o mbcrc.o mbfunccoils.o mbfuncdiag.o mbfuncdisc.o mbfuncholding.o mbfuncinput.o mbfuncother.o mbrtu.o mbtcp.o mbutils.o portevent.o portserial.o porttimer.o Step_motor.o 

## Objects explicitly added by the user
LINKONLYOBJECTS = 

## Build
all: $(TARGET) Green_House.hex Green_House.eep Green_House.lss size

## Compile
Green_House.o: ../Green_House.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

adc.o: ../adc.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

eeprom.o: ../eeprom.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

mb.o: ../mb.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

mbascii.o: ../mbascii.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

mbcrc.o: ../mbcrc.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

mbfunccoils.o: ../mbfunccoils.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

mbfuncdiag.o: ../mbfuncdiag.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

mbfuncdisc.o: ../mbfuncdisc.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

mbfuncholding.o: ../mbfuncholding.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

mbfuncinput.o: ../mbfuncinput.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

mbfuncother.o: ../mbfuncother.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

mbrtu.o: ../mbrtu.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

mbtcp.o: ../mbtcp.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

mbutils.o: ../mbutils.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

portevent.o: ../portevent.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

portserial.o: ../portserial.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

porttimer.o: ../porttimer.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

Step_motor.o: ../Step_motor.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

##Link
$(TARGET): $(OBJECTS)
	 $(CC) $(LDFLAGS) $(OBJECTS) $(LINKONLYOBJECTS) $(LIBDIRS) $(LIBS) -o $(TARGET)

%.hex: $(TARGET)
	avr-objcopy -O ihex $(HEX_FLASH_FLAGS)  $< $@

%.eep: $(TARGET)
	-avr-objcopy $(HEX_EEPROM_FLAGS) -O ihex $< $@ || exit 0

%.lss: $(TARGET)
	avr-objdump -h -S $< > $@

size: ${TARGET}
	@echo
	@avr-size -C --mcu=${MCU} ${TARGET}

## Clean target
.PHONY: clean
clean:
	-rm -rf $(OBJECTS) Green_House.elf dep/* Green_House.hex Green_House.eep Green_House.lss Green_House.map


## Other dependencies
-include $(shell mkdir dep 2>/dev/null) $(wildcard dep/*)

