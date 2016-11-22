#include "frame.h"
#include <avr/interrupt.h>
#include <pbn.h>

static block_morse rx;
static missatge missatge_rx;
static block_morse tx;
static missatge missatge_tx;

void getmessage(void){
	frame_block_get(rx);
	serial_put('R');
	serial_put('|');
	print((char *)rx);
}

void envia_missatge(void){
	serial_put('-');
	serial_put('>');
	uint8_t t,i=0;
	while(i<28){
		if(serial_can_read()){
			t=serial_get();
			if(t==13){
				break;
			}
			else{
				serial_put(t);
				tx[i++]=t;
			}

		}
	}

	tx[i] = '\0';
	serial_put('\n');
	serial_put('\r');
	frame_block_put(tx);

}

int main(void){
  sei();
  frame_init();
  tx=(block_morse)missatge_tx;
  rx=(block_morse)missatge_rx;
  on_frame_received(getmessage);
  while(1){
		while(!frame_can_put());
    serial_get();
		envia_missatge();
	}
  return 0;
}
