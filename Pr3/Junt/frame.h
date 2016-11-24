#ifndef FRAME
#define FRAME
#include <inttypes.h>
#include <stdbool.h>
#include <pbn.h>

typedef void (*frame_callback_t)(void);
void frame_block_get(block_morse b);
void on_frame_received(frame_callback_t l);

/* PART TX */

typedef enum {send,wait} event;
void frame_init(void);

bool frame_can_put(void);
void frame_block_put(const block_morse b);

typedef char missatge[32];
typedef enum {WAIT0,WAIT1,WAITACK0,WAITACK1} maqestatstx;

/* ************************************************** */

/* PART RX */

void getmessage(void);
void frame_init(void);

typedef char missatge[32];
typedef enum {REP0,REP1} maqestatsrx;

/*****************/

#endif
