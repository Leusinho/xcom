#include "frame_tx.h"
#include "error_morse_avr.h"
#include <stdbool.h>
#include <stdlib.h>
#include <pbn.h>
#define TIMEOUT 5000
#define DEBUGGER 0

static maqestats estat_tx;
static missatge missatge_tx;
static block_morse rx;
static missatge missatge_rx;
static uint8_t intents = 0;

static timer_handler_t timer_timeout;
static bool check_missatge_confirmacio(void){
  switch(estat_tx){
    case CONFIRA:
    if(rx[0] == 'A')
    return true;
    else
    return false;
    break;
    case CONFIRB:
    if(rx[0] == 'B')
    return true;
    else
    return false;
    break;
    default:
    return false;
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
    default:
    break;
  }
}


static void timeout(void){
  if(intents<3){
    maquinaestats(envia); //Tornem a enviar
  }
  timer_cancel(timer_timeout);
  intents++;
}

static void got_something(void){
  rx = (block_morse) missatge_rx;
  ether_block_get(rx);
  #if DEBUGGER
  print((char *)rx);
  #endif
  if(test_crc_morse((char *)rx) && check_missatge_confirmacio()){
    print("OK");
    maquinaestats(confirma);
    on_message_received(NULL);
  }
  else{
    maquinaestats(envia);
  }
}

void start_timeout(void){
  timer_timeout = timer_after(TIMER_MS(TIMEOUT),timeout);
  on_message_received(got_something);
}

void maquinaestats(event funcio){
  switch(estat_tx){
    case ENVIA0:
      switch(funcio){
        case envia:
          if(ether_can_put()){
            ether_block_put((block_morse)missatge_tx);
          }
          estat_tx=CONFIRA;
          on_finish_transmission(start_timeout);

          break;
        case confirma: //Missatge correcte, canviem estat
          estat_tx = ENVIA1;
          timer_cancel(timer_timeout);
          break;
      }
    break;

    case ENVIA1:
      switch(funcio){
        case envia:
          if(ether_can_put()){
            ether_block_put((block_morse)missatge_tx);
          }
          estat_tx=CONFIRB;
          on_finish_transmission(start_timeout);
          break;

        case confirma: //Missatge correcte, canviem estat
          estat_tx = ENVIA0;
          timer_cancel(timer_timeout);
          break;
        }
      break;

    case CONFIRA:
      switch(funcio){
        case envia:
          if(ether_can_put()){
            ether_block_put((block_morse)missatge_tx);
          }
          break;

        case confirma:
          estat_tx = ENVIA1;
          timer_cancel(timer_timeout);
          break;

      }
      break;

    case CONFIRB:
        switch(funcio){
          case envia:
            if(ether_can_put()){
              ether_block_put((block_morse)missatge_tx);
            }
            break;
          case confirma:
            estat_tx = ENVIA0;
            timer_cancel(timer_timeout);
            break;
        }
        break;
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
    print("TRAMA TX:");
    print(missatge_tx);
  }

  maquinaestats(envia);

  //send_message(); //Intentem enviar el missatge

}

bool frame_can_put(void){
  return (estat_tx == ENVIA0) || (estat_tx == ENVIA1);
}

void frame_init(void){
  ether_init();
  serial_open();
  estat_tx = ENVIA0;
}
