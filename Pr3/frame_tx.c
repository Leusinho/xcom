#include "frame.h"
#include "error_morse_avr.h"
#include <pbn.h>
#define DEBUGGER 1

static maqestats estat_tx;
static missatge missatge_tx;
static block_morse rx;
static missatge missatge_rx;
static uint8_t intents = 0;

static void send_message(void);
static void receive_confirmation(void);

static void receive_confirmation(void){
	if(DEBUGGER){
		print("SENT");
		print("RECEIVING A or B... ");
	}
	rx = (block_morse) missatge_rx;
	
	if(ether_can_get()){
	ether_block_get(rx); // Rebem el valor A o B + checksum
	}
	if(test_crc_morse(rx)){ //Comprovem si el morse és correcte
		switch(estat_tx){
			ca

		}
	}
}

void change_to_conf(void){
	switch(estat_tx){
		case ENVIA0:
			estat_tx=CONFIRA;
			break;
		case ENVIA1:
			estat_tx=CONFIRB;
			break;
	}
}

static void send_message(void){
	if(intents < 3){
		if(ether_can_put()){
			ether_block_put(missatge_tx);
			if(DEBUGGER){
				print("SENDING...");
				print(missatge_tx);
			}
			change_to_conf(); //Cambia l'estat a confirmació depenent de la trama que hem de rebre (0 -> A, 1 -> B)
			intents=0;
			on_finish_transmission((ether_callback_t) receive_confirmation); //Quan acabem la transmissió, hem d'esperar a rebre un valor (A o B)

		}
	}

		else{
			intents++;
			uint8_t r = rand() % 11; // Numero aleatori entre 0 i 10
			timer_after(r*100, send_message); //r*100 son ticks -> Xs * 1000ms / 10 ticks cada ms -> Y ticks
			if(DEBUGGER){
			print("ETHER IS BUSY");
			}
			
		}
}



void frame_block_put(const block_morse b){
	char missatge_net[28];
	switch(estat_tx){
		case ENVIA0: 
			missatge_tx[0] = '0';
			missatge_net[0] = '0';
			break;
		case ENVIA1:
			missatge_tx[0] = '1';
			missatge_net[0] = '1';
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

	if(DEBUGGER){
	print("MISSATGE ENVIAT TX:");
	print(missatge_tx);
	}

	send_missatge(); //Intentem enviar el missatge

		

}

bool frame_can_put(void){
	return (estat_tx == ENVIA0) || (estat_tx == ENVIA1);
}

void frame_init(void){
ether_init();
serial_open();
estat_tx = ENVIA0;
}