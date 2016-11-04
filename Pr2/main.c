#include "lan.h"
#include <avr/interrupt.h>
#include <pbn.h>

static uint8_t no;
block_morse rx;
missatge missatge_tx;

void get_message(void){
serial_put('R');
serial_put('|');
serial_put(lan_block_get(rx)); //ID del arduino que envia
serial_put('-');
serial_put('>');
serial_put(no); //ID del arduino que rep (actual)
serial_put(':');
print((char *)rx); //Mostro el missatge a la APP
}

void send_message(void){
	bool is_reset=false;
	uint8_t i=0;
	uint8_t t;
	uint8_t nd;
	nd = serial_get();
	serial_put(no);
	serial_put('-');
	serial_put('>');
	serial_put(nd);
	serial_put(serial_get());
	while(i<32){
		if(serial_can_read()){
			t=serial_get();
			if(t==13){
				break;
			}
			else if(t=='r'){
				print("BREAK");
				is_reset=true;
				break;
			}
			else{
				serial_put(t);
				missatge_tx[i++]=t;
				
			}


				
		}
	}
	missatge_tx[i]='\0';

	if(!is_reset){
		serial_put('\n');
		serial_put('\r');
		lan_block_put((block_morse) missatge_tx, nd);
	}
	else
		missatge_tx[0]='\0'; //Hem reiniciat: borrem el missatge anterior
}





int main(void){
	sei(); 
	serial_open();
	print("INSEREIX NODE (LLETRA)");
	no=serial_get();
	//serial_put(no);
	lan_init(no); //Iniciem el mòdul LAN
	print("WAITING MESSAGE");
	on_lan_received(get_message); //Callback (cridem la funció rep missatge quan rebem un missatge), que mostra la informació
	while(1){
		while(!lan_can_put()); //Mentre no poguem enviar, ens esperem
		send_message(); //Enviem el missatge a la capa LAN

	}
	return 0;
}
