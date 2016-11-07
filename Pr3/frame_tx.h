#ifndef FRAMETX_H
#define FRAMETX_H
#include <inttypes.h>
#include <stdbool.h>
#include <pbn.h>

typedef void (*frame_callback_t)(void);
typedef enum {envia,confirma} event;

void frame_init(void);

bool frame_can_put(void);
void frame_block_put(const block_morse b);

void frame_block_get(block_morse b);
void on_frame_received(frame_callback_t l);
void maquinaestats(event funcio);


typedef char missatge[32];
typedef enum {ENVIA0,ENVIA1,CONFIRA,CONFIRB} maqestats;

#endif
