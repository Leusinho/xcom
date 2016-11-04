#include <pbn.h>
#include "error_morse_avr.h"
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/crc16.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#define MAXSTR 254


bool es_valid(char a){
	return ((a >= 'A') && (a <='Z')) || ((a>= '0') && (a<='9'));
}

hex byte2hex(uint8_t value){
	hex convertir;
	convertir.partbaixa = (value & 0b00001111);
	if (convertir.partbaixa >= 0 && convertir.partbaixa <= 9)
		convertir.partbaixa += 48;
	else if(convertir.partbaixa >= 10 && convertir.partbaixa <= 15)
		convertir.partbaixa += 55;
	
	convertir.partalta = (value & 0b11110000);
	convertir.partalta>>=4;
	if (convertir.partalta >= 0 && convertir.partalta <= 9)
		convertir.partalta += 48;
	else if(convertir.partalta >= 10 && convertir.partalta <= 15)
		convertir.partalta += 55;

	return convertir;
}

uint8_t hex2byte(hex value){
	uint8_t valorpartbaixa=0; 
	uint8_t valorpartalta=0;

	uint8_t valortotal;

	
	if (value.partbaixa >= 48 && value.partbaixa <= 57)
		valorpartbaixa = value.partbaixa - 48;
	else if (value.partbaixa >= 65 && value.partbaixa <= 70)
		valorpartbaixa = value.partbaixa - 55;


	if (value.partalta >= 48 && value.partalta <= 57)
		valorpartalta = value.partalta - 48;
	else if (value.partalta >= 65 && value.partalta <= 70)
		valorpartalta = value.partalta - 55;

	valorpartalta<<=4;	
	

	valortotal = valorpartalta | valorpartbaixa;

	return valortotal;
	
}

bool test_check_morse(char paraula[]){
	uint16_t suma=0;
	uint8_t valorfinal;
	hex encript;
	int i;
	
	for(i=0; i < (strlen(paraula)-2);i++){
		if(es_valid(paraula[i])){
			suma+=paraula[i];
		}
		
	}
	//CHECKSUM HA D'ESTAR JUNT AL FINAL, SI NO, NO FUNCIONA
	encript.partalta = paraula[i];
	encript.partbaixa = paraula[i+1];
	
	
	suma+=hex2byte(encript);
	valorfinal = suma + (suma>>8); //Tenim la suma de tots els caracters + checksum
	if (valorfinal == 255)
		return true;
	else
		return false;
	
}


const char * check_morse(char string[]){ 
	static char str[MAXSTR];
	uint16_t suma = 0;
	uint8_t temp;
	int j=0;
	for(int i=0;i < strlen(string);i++){
		if(es_valid(string[i])){
			suma+=string[i];
			str[j]=string[i];
			j++; 
		}
	}
	
	temp = (suma >> 8); // Desplaçem els bits de carry als bits mes significatius
	temp+=suma; // Ho tornem a sumar

	str[j] = byte2hex(~(temp)).partalta;
	str[j+1] = byte2hex(~(temp)).partbaixa;
	str[j+2] = '\0';
	str[j+3] = '\t';
	return str;
	
}

const char * test_crc(char string[]){
	uint8_t value;
	int i;
	static char str2[MAXSTR];
	value=_crc_ibutton_update(0,string[0]);
	str2[0] = string[0];
	for(i=1;i<strlen(string);i++){ //Comencem en 1 per fer la crida amb valor inicial 0
		value=_crc_ibutton_update(value, string[i]);
		str2[i]=string[i];
	}
	str2[strlen(string)]=byte2hex(value).partalta;
	str2[strlen(string)+1]=byte2hex(value).partbaixa;
	str2[strlen(string)+2] = '\0';
	str2[strlen(string)+3] = '\t';
	return str2;

}

hex test_only_crc(char string[]){
	uint8_t value;
	int i;
	value=_crc_ibutton_update(0,string[0]);
	for(i=1;i<strlen(string);i++){ //Comencem en 1 per fer la crida amb valor inicial 0
		value=_crc_ibutton_update(value, string[i]);
	}

	hex prova;
	prova.partalta=byte2hex(value).partalta;
	prova.partbaixa=byte2hex(value).partbaixa;
	return prova;
}


bool test_crc_morse(char mander[]){
	char paraula[254];
	
	hex crparaula;
	hex crparaulaorig;
	uint8_t i=0;

	if(strlen(mander) <= 1)
		return false;
	for(i=0; i < (strlen(mander)-2);i++)
		paraula[i]=mander[i];
		
	

	paraula[i]='\0';
	
	crparaula.partalta = test_crc(paraula)[strlen(test_crc(paraula))-2];
	crparaula.partbaixa = test_crc(paraula)[strlen(test_crc(paraula))-1];
	crparaulaorig.partalta = mander[strlen(mander)-2];
	crparaulaorig.partbaixa = mander[strlen(mander)-1];

	
	if (hex2byte(crparaula) == hex2byte(crparaulaorig))
		return true;
	else
		return false;
	
}


