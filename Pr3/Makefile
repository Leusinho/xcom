CC=avr-gcc
CPPFLAGS=-DF_CPU=16000000UL
CFLAGS=-Wall -std=c99 -Os -mmcu=atmega328p -fshort-enums -Ilibpbn
LDFLAGS=-mmcu=atmega328p
DEVICE=/dev/tty.usbmodem1411
SECONDDEVICE=/dev/tty.usbmodem1421

vpath lib% libpbn 

main_tx: frame_tx.o error_morse_avr.o -lpbn
main_rx: frame_rx.o error_morse_avr.o -lpbn
main_tx.o: frame_tx.h -lpbn
main_rx.o: frame_rx.h -lpbn
frame_tx.o: frame_tx.h error_morse_avr.h -lpbn
frame_rx.o: frame_rx.h error_morse_avr.h -lpbn
error_morse_avr.o: error_morse_avr.h

.PHONY: clean veryclean
clean:
	rm -f *~ *.o *.s *.hex

.PHONY: tx rx
tx:
	avr-objcopy -Oihex main_tx main_tx.hex
	sudo avrdude -c arduino -p atmega328p -P $(DEVICE) -U main_tx.hex
	sudo picocom $(DEVICE)
rx:
	avr-objcopy -Oihex main_rx main_rx.hex
	sudo avrdude -c arduino -p atmega328p -P $(SECONDDEVICE) -U main_rx.hex
	sudo picocom $(SECONDDEVICE)
	

	

