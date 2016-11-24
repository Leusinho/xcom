CC=avr-gcc
CPPFLAGS=-DF_CPU=16000000UL
CFLAGS=-Wall -std=c99 -Os -mmcu=atmega328p -fshort-enums -Ilibpbn
LDFLAGS=-mmcu=atmega328p
DEVICE=/dev/tty.usbmodem1411
SECONDDEVICE=/dev/tty.usbmodem1421

vpath lib% libpbn 
all: main tar
main: main.o frame.o error_morse_avr.o -lpbn
main.o: frame.h -lpbn
frame.o: frame.h error_morse_avr.h -lpbn
error_morse_avr.o: error_morse_avr.h -lpbn

.PHONY: clean veryclean
clean:
	rm -f *~ *.o *.s *.hex

.PHONY: c1 c2 tar
c1:
	avr-objcopy -Oihex main main.hex
	sudo avrdude -c arduino -p atmega328p -P $(DEVICE) -U main.hex
	sudo picocom $(DEVICE)
c2:
	avr-objcopy -Oihex main main.hex
	sudo avrdude -c arduino -p atmega328p -P $(SECONDDEVICE) -U main.hex
	sudo picocom $(SECONDDEVICE)

tar: 
	tar -czvf pr3_xarxes_Kharbouch_Moreno.tar.gz frame.c frame.h error_morse_avr.c error_morse_avr.h main.c Makefile libpbn/

	

	

