#ifndef FRAMERX_H
#define FRAMERX_H
#include <inttypes.h>
#include <stdbool.h>
#include <pbn.h>

typedef void (*frame_callback_t)(void);

void frame_init(void);

void frame_block_get(block_morse b);
void on_frame_received(frame_callback_t l);

typedef char missatge[32];
typedef enum {REP0,REP1} maqestats;

#endif