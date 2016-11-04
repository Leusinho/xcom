#ifndef BLCKSERIAL_H
#define BLCKSERIAL_H

#include <inttypes.h>

/* readline(char s[], uint8_t m )
 * Read a line until a non graphic character is found or lenth > `m`.
 * Returns the length.
 */

void print(char s[]);
int readline(char s[], uint8_t m );


#endif
