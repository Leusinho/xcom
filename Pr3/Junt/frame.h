#ifndef FRAME_H
#define FRAME_H
#include <inttypes.h>
#include <stdbool.h>
#include <pbn.h>

typedef void (*frame_callback_t)(void);
void frame_init(void);
void frame_block_get(block_morse b);
void on_frame_received(frame_callback_t l);

/* PART TX */

typedef enum {send,wait} event;


bool frame_can_put(void);
void frame_block_put(const block_morse b);

typedef char missatge[32];
typedef enum {WAIT0,WAIT1,WAITACK0,WAITACK1} maqestatstx;
typedef enum {REP0,REP1} maqestatsrx;

/* ************************************************** */

#endif
