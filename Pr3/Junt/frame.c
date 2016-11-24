#define TIMEOUT 5000
#define DEBUGGER 0
#include "frame.h"
#include "error_morse_avr.h"
#include <stdlib.h>
#include <stdbool.h>
#include <pbn.h>

static missatge missatge_rx;
static maqestatsrx estat_rx;
static missatge missatge_tx;
static maqestatstx estat_tx;
static char tramarx[4];
static frame_callback_t frame_callback;
static block_morse rx;
static uint8_t intents = 0;
//static uint8_t try_intents = 0;
static block_morse trama;

static timer_handler_t timer_timeout;

static void message_received(void);
static void convert_trama(char * to_convert,char letter);
static void make_trama(const block_morse b,char posicio);
static void start_timer(void);
static void try_to_send(void);
static void maquinaestatstx(event function);
static void maquinaestatsrx();
static void timer();
static void timeout();

static void maquinaestatstx(event function){
  switch(estat_tx){
    case WAIT0:
      if(function == send){ //Volem enviar
        make_trama(trama,'0'); //Fem la trama. Es guarda a missatge_tx
        if(ether_can_put()){
          #if DEBUGGER
            print("TRAMA TX:");
            print(missatge_tx);
          #endif
          ether_block_put((block_morse) missatge_tx);
          on_finish_transmission(start_timer);
          estat_tx=WAITACK0;
        }

        else{
          try_to_send();
        }
      }
    break;

    case WAITACK0:
      if(function == wait){
        if(test_crc_morse((char *)rx) && rx[0] == 'A'){
          #if DEBUGGER
            print("RECEIVED-> A");
          #endif
          timer_cancel(timer_timeout);
          estat_tx=WAIT1;
          on_finish_transmission(NULL);
          intents=0;
        }
      }

      else if(function == send){
        if(ether_can_put()){
          ether_block_put((block_morse) missatge_tx);
          on_message_received(message_received);
          on_finish_transmission(start_timer);
        }
      }

    break;

    case WAIT1:
      if(function == send){ //Volem enviar
        make_trama(trama,'1'); //Fem la trama. Es guarda a missatge_tx
        if(ether_can_put()){
          #if DEBUGGER
            print("TRAMA TX:");
            print(missatge_tx);
          #endif
          ether_block_put((block_morse) missatge_tx);
          on_finish_transmission(start_timer);
          estat_tx=WAITACK1;
        }

        else{
          try_to_send();
        }

      }
      break;

    case WAITACK1:
      if(function == wait){
        if(test_crc_morse((char *)rx) && rx[0] == 'B'){
          timer_cancel(timer_timeout);
          #if DEBUGGER
            print("RECEIVED-> B");
          #endif
          estat_tx=WAIT0;
          on_finish_transmission(NULL);
          intents=0;
        }
      }

      else if(function == send){
        if(ether_can_put()){
          ether_block_put((block_morse) missatge_tx);
          on_finish_transmission(start_timer);
        }
      }
      break;
    }
}

static void maquinaestatsrx(){
  #if DEBUGGER
    serial_put('R');
    serial_put('-');
    serial_put('>');
    print((char *)rx);
  #endif
  switch(estat_rx){
    case REP0:
        if(test_crc_morse((char *)rx) && rx[0] == '0'){
          convert_trama(tramarx,'A');
          if(ether_can_put()){
    			   ether_block_put((block_morse) tramarx);
             #if DEBUGGER
             serial_put('S');
             serial_put('E');
             serial_put('N');
             serial_put('T');
             serial_put('-');
             serial_put('>');
             serial_put('A');
             serial_put('\n');
             serial_put('\r');
             #endif
           }
          estat_rx=REP1;
          frame_callback();
        }
        else{
          convert_trama(tramarx,'B');
          if(ether_can_put()){
    			   ether_block_put((block_morse) tramarx);
             #if DEBUGGER
             serial_put('S');
             serial_put('E');
             serial_put('N');
             serial_put('T');
             serial_put('-');
             serial_put('>');
             serial_put('B');
             serial_put('\n');
             serial_put('\r');
             #endif
           }
        }

    break;

    case REP1:
      if(test_crc_morse((char *)rx) && rx[0] == '1'){
        convert_trama(tramarx,'B');
        if(ether_can_put()){
          #if DEBUGGER
          serial_put('S');
          serial_put('E');
          serial_put('N');
          serial_put('T');
          serial_put('-');
          serial_put('>');
          serial_put('B');
          serial_put('\n');
          serial_put('\r');
          #endif
        ether_block_put((block_morse) tramarx);
        }
        estat_rx=REP0;
        frame_callback();
      }
      else{
        convert_trama(tramarx,'A');
        if(ether_can_put()){
          #if DEBUGGER
          serial_put('S');
          serial_put('E');
          serial_put('N');
          serial_put('T');
          serial_put('-');
          serial_put('>');
          serial_put('A');
          serial_put('\n');
          serial_put('\r');
          #endif
        ether_block_put((block_morse) tramarx);
        }
      }
    break;
  }
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

/* PART TX */

static void make_trama(const block_morse b,char posicio){
  char missatge_net[28];
  missatge_tx[0] = posicio;
  missatge_net[0] = posicio;
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


}

static void timeout(void){
  if(intents<3){
    maquinaestatstx(send); //Tornem a enviar
  }
  timer_cancel(timer_timeout);
  intents++;
}

static void message_received(void){
  ether_block_get(rx);
  if(rx[0] == 'A' || rx[0] == 'B'){ //Missatge de confirmacio. Cridem TX
    maquinaestatstx(wait);
  }
  else if(rx[0] == '0' || rx[0] == '1'){
    maquinaestatsrx();
  }
}

static void timer(){
  #if DEBUGGER
  serial_put('T');
  serial_put('Y');
  serial_put('\n');
  serial_put('\r');
  #endif
  maquinaestatstx(send);
}
static void start_timer(void){
  timer_timeout = timer_after(TIMER_MS(TIMEOUT),timeout); //Encenem el timer
}
static void try_to_send(void){
  uint8_t r = rand() % 11; // Numero aleatori entre 0 i 10
  timer_timeout=timer_after(r*100, timer); //r*100 son ticks -> Xs * 1000ms / 10 ticks cada ms -> Y ticks
}


void frame_block_put(const block_morse b){
  trama = b;
  maquinaestatstx(send); //Intentem enviar

}

bool frame_can_put(void){
  return (estat_tx == WAIT0) || (estat_tx == WAIT1);
}

void frame_init(void) {
  ether_init();
  serial_open();
  estat_tx = WAIT0;
  estat_rx = REP0;
  rx=(block_morse) missatge_rx;
  on_message_received(message_received);
}
