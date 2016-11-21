#include "frame_tx.h"
#include "error_morse_avr.h"
#include <stdbool.h>
#include <stdlib.h>
#include <pbn.h>
#define TIMEOUT 5000
#define DEBUGGER 1

static maqestats estat_tx;
static missatge missatge_tx;
static block_morse rx;
static missatge missatge_rx;
static uint8_t intents = 0;
static block_morse trama;

static timer_handler_t timer_timeout;


void make_trama(const block_morse b,char posicio){
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

  if(DEBUGGER){
    print("TRAMA TX:");
    print(missatge_tx);
  }
}


static void timeout(void){
  if(intents<3){
    maquinaestats(send); //Tornem a enviar
  }
  timer_cancel(timer_timeout);
  intents++;
}

static void message_received(void){
  rx = (block_morse) missatge_rx;
  ether_block_get(rx); //Obtenem el valor
  #if DEBUGGER
  print((char *)rx);
  #endif
  maquinaestats(wait);
}

void start_timer(void){
  timer_timeout = timer_after(TIMER_MS(TIMEOUT),timeout); //Encenem el timer
  on_message_received(message_received);
}

void maquinaestats(event function){
  switch(estat_tx){
    case WAIT0:
      if(function == send){ //Volem enviar
        make_trama(trama,'0'); //Fem la trama. Es guarda a missatge_tx
        if(ether_can_put()){
          ether_block_put((block_morse) missatge_tx);
          on_finish_transmission(start_timer);
          estat_tx=WAITACK0;
        }
      }
    break;

    case WAITACK0:
      if(function == wait){
        if(test_crc_morse((char *)rx) && rx[0] == 'A'){
          timer_cancel(timer_timeout);
          estat_tx=WAIT1;
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

    case WAIT1:
      if(function == send){ //Volem enviar
        make_trama(trama,'1'); //Fem la trama. Es guarda a missatge_tx
        if(ether_can_put()){
          ether_block_put((block_morse) missatge_tx);
          on_finish_transmission(start_timer);
          estat_tx=WAITACK1;
        }
      }
      break;

    case WAITACK1:
      if(function == wait){
        if(test_crc_morse((char *)rx) && rx[0] == 'B'){
          timer_cancel(timer_timeout);
          estat_tx=WAIT0;
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


void frame_block_put(const block_morse b){
  trama = b;
  maquinaestats(send); //Intentem enviar

  //send_message(); //Intentem enviar el missatge

}

bool frame_can_put(void){
  return (estat_tx == WAIT0) || (estat_tx == WAIT1);
}

void frame_init(void){
  ether_init();
  serial_open();
  estat_tx = WAIT0;
}
