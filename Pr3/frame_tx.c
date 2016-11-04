#include "frame.h"
#include "error_morse_avr.h"
#include <pbn.h>

static maqestats estat_tx;
static missatge missatge_tx;
//Inicialitza el modul frame

void frame_block_put(const block_morse b){
	char missatge_net[28];
	switch(estat_tx){
		case ENVIA0: 
			missatge_tx[0] = '0';
			missatge_net[0] = '0';
			break;
		case ENVIA1:
			missatge_tx[0] = '1';
			missatge_net[0] = '0';
			break;
		default: //No hi entrarem mai, però per si de cas
			print("ERROR");
	}
	uint8_t i=0,j=1;
	while(b[i] != '\0'){
		missatge_tx[j] = b[i];
		missatge_net[j++] = b[i++];
	}
	missatge_net[j] = '\0'; //Tenim missatge del tipus '1HOLA o 0ADEU'
	hex get_crc=test_only_crc(missatge_net);
	missatge_tx[j++]=get_crc.partalta;
	missatge_tx[j++]=get_crc.partbaixa;
	missatge_tx[j]='\0';
	print(missatge_tx);

		

}

bool frame_can_put(void){
	return (estat_tx == ENVIA0) || (estat_tx == ENVIA1);
}

void frame_init(void){
ether_init();
serial_open();
estat_tx = ENVIA0;
}