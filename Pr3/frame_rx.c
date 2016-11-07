#include "frame_rx.h"
#include "error_morse_avr.h"
#include <stdbool.h>
#include <stdlib.h>
#include <pbn.h>
#define DEBUGGER 0


static missatge missatge_rx;
static maqestatsrx estat_rx;
static char trama[4];
static frame_callback_t frame_callback = NULL;
static void receive_trama(void);
block_morse rx;

static bool check_trama(void){ //Comprova si la trama és la correcta
	switch(estat_rx){
		case REP0:
			if(rx[0] == '0'){
				return true;
			}
			else{
				return false;
			}
			break;
		case REP1:
			if(rx[0] == '1'){
				return true;
			}
			else{
				return false;
			}
			break;
	}
	return false;
}

static void convert_trama(char * to_convert,char letter){
	switch(letter){
		case 'A':
			to_convert[0] = 'A';
			to_convert[1] = '1';
			to_convert[2] = '8';
			to_convert[3] = '\0';
			break;

		case 'B':
			to_convert[0] = 'B';
			to_convert[1] = 'F';
			to_convert[2] = 'A';
			to_convert[3] = '\0';
			break;

		default:
			break;

	}
}

void send_trama(bool sendcorrect){ //Envia la trama adequada depenent de l'estat. True -> Enviem la que toca. False -> Enviem l'altra
if(sendcorrect){
	switch(estat_rx){
		case REP0:
			convert_trama(trama,'A');
			ether_block_put((block_morse) trama);
			break;
		case REP1:
			convert_trama(trama,'B');
			ether_block_put((block_morse) trama);
			break;
	}
}
else{
	switch(estat_rx){
		case REP0:
			convert_trama(trama,'B');
			ether_block_put((block_morse) trama);
			break;
		case REP1:
			convert_trama(trama,'A');
			ether_block_put((block_morse) trama);
			break;
	}
}
}

static void change_estat(void){
  switch(estat_rx){
    case REP0:
      estat_rx=REP1;
      break;
    case REP1:
      estat_rx=REP0;
      break;
  }
}

void receive_trama(void){
	if(ether_can_get()){
    ether_block_get(rx);
    #if DEBUGGER
      print((char *)rx);
    #endif
    if(test_crc_morse((char *)rx)){ //Comprovem el CRC de la trama

		  if(check_trama()){ //Comprovem si la trama que rebem és la que pertoca
        send_trama(true);

        #if DEBUGGER
  			serial_put(trama[0]);
  			serial_put('\n');
  			serial_put('\r');
  			#endif

			change_estat();
			frame_callback();


		}
		else{
			send_trama(false);
		}


	}


	}
}

void frame_block_get(block_morse b){ //Agafem la paraula que hem rebut
	uint8_t i=0;
	for(int j=1;rx[j] != '\0';j++){
		b[i++] = rx[j];
	}
	b[i-2] = '\0'; //Borrem el CRC
}

void on_frame_received(frame_callback_t l){
	frame_callback = l;
}

void frame_init(void){
ether_init();
serial_open();
estat_rx = REP0;
rx=(block_morse) missatge_rx;
on_message_received(receive_trama);
}
