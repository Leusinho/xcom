#include "error_morse_avr.h"
#include <pbn.h>
#include <stdint.h>
#include <avr/interrupt.h>

int main() {
	char huehue[200];
	uint8_t a;
	sei();
	serial_open();
	int i=0;
	while(1){
		i=0;
		while((a=serial_get()) != 13){
			serial_put(a);
			huehue[i++] = a;
		}
		huehue[i]='\0';
		serial_put(' ');
		if(test_crc_morse(huehue))
			print("CHECKSUM CORRECTE");
		else
			print("CHECKSUM NO CORRECTE");
	}
	return 0;
}