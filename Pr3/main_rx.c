#include "frame_rx.h"
#include <avr/interrupt.h>
#include <pbn.h>

static block_morse rx;
static missatge missatge_rx;

int main(void){
	sei();
	frame_init();
	rx=(block_morse)missatge_rx;
	while(1){
		while(!frame_can_put());
		envia_missatge();
	}
	return 0;
}
