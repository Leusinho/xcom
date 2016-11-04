CC=avr-gcc
CPPFLAGS=-DF_CPU=16000000UL
CFLAGS=-Wall -std=c99 -Os -mmcu=atmega328p -fshort-enums -Ilibpbn
LDFLAGS=-mmcu=atmega328p
DEVICE=/dev/ttyACM0
SECONDDEVICE=/dev/ttyACM1

vpath lib% libpbn 

main: lan.o error_morse_avr.o -lpbn
main.o: lan.h -lpbn
lan.o: lan.h error_morse_avr.h
error_morse_avr.o: error_morse_avr.h


.PHONY: clean veryclean
clean:
	rm -f *~ *.o *.s *.hex

.PHONY: c1 c2
c1:
	avr-objcopy -Oihex main main.hex
	sudo avrdude -c arduino -p atmega328p -P $(DEVICE) -U main.hex
	sudo picocom $(DEVICE)
c2:
	avr-objcopy -Oihex main main.hex
	sudo avrdude -c arduino -p atmega328p -P $(SECONDDEVICE) -U main.hex
	sudo picocom $(SECONDDEVICE)
	

	

