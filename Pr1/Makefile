CC=avr-gcc
CPPFLAGS=-DF_CPU=16000000UL
CFLAGS=-Wall -std=c99 -Os -mmcu=atmega328p -fshort-enums -Ilibpbn
LDFLAGS=-mmcu=atmega328p
DEVICE=/dev/ttyACM0

vpath lib% libpbn 


main_checkmorse: error_morse_avr.o -lpbn
main_testcheckmorse: error_morse_avr.o -lpbn
main_crcmorse: error_morse_avr.o -lpbn
main_testcrcmorse: error_morse_avr.o -lpbn
error_morse_avr.o: error_morse_avr.c -lpbn


.PHONY: clean veryclean
clean:
	rm -f *~ *.o *.s *.hex

.PHONY: compile_checkmorse compile_testcheckmorse compile_crcmorse compile_testcrcmorse
compile_checkmorse:
	avr-objcopy -Oihex main_checkmorse main_checkmorse.hex
	sudo avrdude -c arduino -p atmega328p -P $(DEVICE) -U main_checkmorse.hex
	sudo picocom $(DEVICE)
compile_testcheckmorse:
	avr-objcopy -Oihex main_testcheckmorse main_testcheckmorse.hex
	sudo avrdude -c arduino -p atmega328p -P $(DEVICE) -U main_testcheckmorse.hex
	sudo picocom $(DEVICE)
compile_crcmorse:
	avr-objcopy -Oihex main_crcmorse main_crcmorse.hex
	sudo avrdude -c arduino -p atmega328p -P $(DEVICE) -U main_crcmorse.hex
	sudo picocom $(DEVICE)
compile_testcrcmorse:
	avr-objcopy -Oihex main_testcrcmorse main_testcrcmorse.hex
	sudo avrdude -c arduino -p atmega328p -P $(DEVICE) -U main_testcrcmorse.hex
	sudo picocom $(DEVICE)
	

	

