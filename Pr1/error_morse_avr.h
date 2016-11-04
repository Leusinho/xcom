//#include <avr/io.h>
#include <stdbool.h>
#include <stdint.h>

typedef struct hex { 
    uint8_t partbaixa;
    uint8_t partalta; 
} hex;

hex byte2hex(uint8_t value);

uint8_t hex2byte(hex value);

const char * check_morse(char string[]);

bool test_check_morse(char paraula[]);

const char * test_crc(char string[]);

bool test_crc_morse(char mander[]);

bool es_valid(char); // Retorna TRUE si el char és valid