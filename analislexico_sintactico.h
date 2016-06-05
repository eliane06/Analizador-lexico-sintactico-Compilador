/*********** Librerias utilizadas **************/
#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<ctype.h>

/************* Definiciones ********************/

//Codigos
#define L_CORCHETE	0
#define R_CORCHETE	1
#define L_LLAVE		2
#define R_LLAVE		3
#define COMA		4
#define END		5
#define DOS_PUNTOS	6
#define LITERAL_CADENA	7
#define LITERAL_NUM	8
#define PR_TRUE		9
#define PR_FALSE	10
#define PR_NULL		11
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
