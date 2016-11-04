#include <pbn.h>
#include <stdbool.h>
#include <stdlib.h>
#include <avr/interrupt.h>
#include "lan.h"
#include "error_morse_avr.h"
#define DEBUGGER 1

static estat EstatActual = ESPERANT;
static uint8_t idnode;
block_morse rx;
static missatge enviar_missatge;
static block_morse tx;
static missatge missatge_rebut;
static lan_callback_t lan_callback=NULL;

static uint8_t intents=0;

static void enviamessage(void);
static void rep_missatge_ether(void);

static void enviamessage(void){
	if(intents < 3){
		if(ether_can_put()){
			if(DEBUGGER){
			print("SENDING MESSAGE...");	
			}
			ether_block_put(tx);
			EstatActual = ESPERANT;
			intents=0; //Missatge enviat, reiniciem intents
		}
		else{
			if(DEBUGGER){
			print("[Trying to send again...]");	
			}
			intents++;
			uint8_t r = rand() % 11; // Numero aleatori entre 0 i 10
			timer_after(r*100, enviamessage); //r*100 son ticks -> Xs * 1000ms / 10 ticks cada ms -> Y ticks
			EstatActual = PENDENT;
		
		}

	}

	else{
		if(DEBUGGER){
			print("MESSAGE FAILED");	
		}
		
		intents=0;
		EstatActual=ESPERANT;
	}

}

bool comprova_destinatari(void){
	return rx[1] == idnode;

}


uint8_t lan_block_get(block_morse b){ 
	uint8_t a;
	a=b[0];
	b=(block_morse) missatge_rebut;
	uint8_t i=0;
	for(i=0;missatge_rebut[i+2] != '\0';i++){
		b[i] = missatge_rebut[i+2];
	}
	missatge_rebut[i-2] = '\0';
	return a;
}

void crea_trama(const block_morse b,uint8_t nd){
	uint8_t i=0;
	uint8_t j=0;
	char missatgecomplet[28];
	tx=(block_morse)enviar_missatge;

	missatgecomplet[i]=idnode;
	tx[i++] = idnode;
	missatgecomplet[i]=nd;
	tx[i++] = nd;

	for(j=0;b[j]!='\0';j++){
		tx[i] = b[j];
		missatgecomplet[i] = b[j];
		i++;
	}

	missatgecomplet[i]='\0';
	hex crc=test_only_crc((char *)missatgecomplet);
	tx[i]=crc.partalta;
	tx[i+1]=crc.partbaixa;
	tx[i+2]='\0';
	if(DEBUGGER){
	print((char *)tx);
	}
}

void on_lan_received(lan_callback_t l){
  lan_callback = l;
}

static void rep_missatge_ether(void){
	//Funcio que es dispara quan rebem del ether
	
	rx=(block_morse)missatge_rebut;
	ether_block_get(rx); //Guardem el missatge a RX
	if(comprova_destinatari()){
		if(test_crc_morse(missatge_rebut)){
			lan_callback();
		}

		else{
			if(DEBUGGER)
				print("CRC INCORRECT");
		}
	}

	else{
		if(DEBUGGER)
			print("SKIPPING MESSAGE...");

	}
}

void lan_init(uint8_t no){
	idnode=no;
	ether_init();
	on_message_received(rep_missatge_ether); //DISPARARÀ QUAN REBEM UN MISSATGE PEL ETHER
	print("Node ACTUAL");
	serial_put(no);
	serial_put('\n');
	serial_put('\r');

}

bool lan_can_put(void){
	return EstatActual == ESPERANT;
}


void lan_block_put(const block_morse b,uint8_t nd){
	EstatActual = PENDENT;
	crea_trama(b,nd);
	enviamessage();
}
