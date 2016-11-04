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
	int j=0;
	while(1){
		i=0;
		j=0;
		while((a=serial_get()) != 13){
			serial_put(a);
			huehue[i++] = a;
		}
		huehue[i]='\0';
	
		while(test_crc(huehue)[j] != '\0')
			serial_put(test_crc(huehue)[j++]);
		serial_put('\n');
		serial_put('\r');
		
		
	}
	return 0;
}
