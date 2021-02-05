DEVICE     = atmega328p
CLOCK      = 16000000UL
PROGRAMMER = -c arduino -P /dev/tty.usb* -b 19200 
OBJECTS    = termostato.o
HEADERS    = eeprom.h\
			 serial.h\
		     termostato.h
FUSES      = -U lfuse:w:0x64:m -U hfuse:w:0xdd:m -U efuse:w:0xff:m

AVRDUDE = avrdude $(PROGRAMMER) -p $(DEVICE)
COMPILE = avr-gcc -Wall -Os -DF_CPU=$(CLOCK) -mmcu=$(DEVICE)

all:	termostato.hex

.c.o:
	$(COMPILE) -c $< -o $@

.S.o:
	$(COMPILE) -x assembler-with-cpp -c $< -o $@

.c.s:
	$(COMPILE) -S $< -o $@

flash:	all
	$(AVRDUDE) -U flash:w:termostato.hex:i

fuse:
	$(AVRDUDE) $(FUSES)

install: flash fuse

clean:
	rm -f termostato.hex termostato $(OBJECTS)

termostato: $(OBJECTS)
	$(COMPILE) -o termostato $(OBJECTS)

termostato.hex: termostato
	rm -f termostato.hex
	avr-objcopy -j .text -j .data -O ihex termostato termostato.hex

disasm:	termostato
	avr-objdump -d termostato

cpp:
	$(COMPILE) -E termostato.c
