/*********** Librerias utilizadas **************/
#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<ctype.h>

/************* Definiciones ********************/

//Codigos
#define L_CORCHETE	256
#define R_CORCHETE	257
#define L_LLAVE		258
#define R_LLAVE		259
#define COMA		260
#define END		261
#define DOS_PUNTOS	262
#define LITERAL_CADENA	263
#define LITERAL_NUM	264
#define PR_TRUE		265
#define PR_FALSE	266
#define PR_NULL		267
// Fin Codigos
#define TAMBUFF 	10
#define TAMLEX 		50
#define TAMHASH 	101

/************* Estructuras ********************/

typedef struct token{
	//definir los campos de 1 entrada de la tabla de simbolos
	int compLex;
	char lexema[TAMLEX];	
		
} token;


/************* Prototipos ********************/
