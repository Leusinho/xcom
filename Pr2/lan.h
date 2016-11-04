#ifndef LAN_H
#define LAN_H
#include <inttypes.h>
#include <stdbool.h>
#include <pbn.h>

typedef enum {ESPERANT, PENDENT} estat;
typedef char missatge[32];
typedef void(*lan_callback_t)(void);

const char * missatge_net(block_morse p);

bool comprova_destinatari(void);
void crea_trama(const block_morse b,uint8_t nd);


void lan_init(uint8_t no);


bool lan_can_put(void);
void lan_block_put(const block_morse b, uint8_t nd);


uint8_t lan_block_get(block_morse b);
void on_lan_received(lan_callback_t l);

extern block_morse rx;

#endif
