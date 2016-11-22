#define TIMEOUT 5000
#define DEBUGGER 0
#include "frame.h"
#include "error_morse_avr.h"
#include <pbn.h>

static missatge missatge_rx;
static maqestatsrx estat_rx;
static char tramarx[4];
static frame_callback_t frame_callback;
static void receive_trama(void);
block_morse rx;

static maqestats estat_tx;
static missatge missatge_tx;
static block_morse tx_rx;
static missatge missatge_tx_rx;
static uint8_t intents = 0;
static block_morse trama;

static timer_handler_t timer_timeout;

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
			convert_trama(tramarx,'A');
			ether_block_put((block_morse) tramarx);
			break;
		case REP1:
			convert_trama(tramarx,'B');
			ether_block_put((block_morse) tramarx);
			break;
	}
}
else{
	switch(estat_rx){
		case REP0:
			convert_trama(tramarx,'B');
			ether_block_put((block_morse) tramarx);
			break;
		case REP1:
			convert_trama(tramarx,'A');
			ether_block_put((block_morse) tramarx);
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
  			serial_put(tramarx[0]);
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

/* PART TX */

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
  tx_rx = (block_morse) missatge_tx_rx;
  ether_block_get(tx_rx); //Obtenem el valor
  #if DEBUGGER
  print((char *)tx_rx);
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
        if(test_crc_morse((char *)tx_rx) && tx_rx[0] == 'A'){
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
        if(test_crc_morse((char *)tx_rx) && tx_rx[0] == 'B'){
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
  on_message_received(receive_trama);
}
