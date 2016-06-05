/*
 *	Analizador Léxico	
 *	Curso: Compiladores y Lenguajes de Bajo de Nivel
 *	Descripcion:
 *	Implementa un analizador léxico que reconoce números, literales en cadena y signos de puntuación para un lenguaje
 * 	con sintaxis tipo Json.
 *	
 */

/*********** Inclusión de cabecera **************/
#include "analislexico.h"

/************* Variables globales **************/
	
char cad[5*TAMLEX];		// string utilizado para cargar mensajes de error
token t;// token global para recibir componentes del Analizador Lexico

// variables para el analizador lexico
char buff[2*TAMBUFF];	// Buffer para lectura de archivo fuente
char id[TAMLEX];		// Utilizado por el analizador lexico
int numLinea=1;			// Numero de Linea
int i = 0;
int cantErrores = 0;
/**************** Funciones **********************/

// Rutinas del analizador lexico
void error(const char* mensaje) {
	cantErrores++;
	if (cantErrores == 1 )
		printf("Se encontraron lexemas no válidos");
	printf("Lin %d: Error Lexico. %s.\n",numLinea,mensaje);	
	
}

void sigLex(FILE* fuente, FILE* salida) {
	
	char c=0;
	int acepto=0;
	int estado=0;
	char msg[50];
	

	while((c = fgetc(fuente)) != EOF) {
		
		if (c==' ' || c=='\t') {
			fputc(' ', salida);
			continue;
		}
		else if(c=='\n') {
			//incrementar el numero de linea
			numLinea++;
			fputc('\n', salida);
			break;
		}
		
		else if (c=='[') 
		{
			t.compLex = L_CORCHETE;
			strcpy(t.lexema, "[");
                        fputs(" L_CORCHETE ", salida);
			break;
		}
		else if (c==']')
		{
			
			t.compLex = R_CORCHETE;
			strcpy(t.lexema, "]");
			fputs(" R_CORCHETE ", salida);
			break;
			
		}

		else if (isdigit(c))
		{
				//es un numero
				i=0;
				estado=0;
				acepto=0;
				id[i]=c;
				
				while(!acepto)
				{
					switch(estado){
					case 0: //una secuencia netamente de digitos, puede ocurrir . o e
						c=fgetc(fuente);
						if (isdigit(c)) {
							id[++i]=c;
							estado=0;
						}
						else if(c=='.'){
							id[++i]=c;
							estado=1;
						}
						else if(tolower(c)=='e'){
							id[++i]=c;
							estado=3;
						}
						else{
							estado=6;
						}
						break;
					
					case 1:
						c=fgetc(fuente);						
						if (isdigit(c))
						{
							id[++i]=c;
							estado=2;
						}

						else if(c=='.')
						{
							i--;
							fseek(fuente,-1,SEEK_CUR);
							estado=6;
						} 

						else{
							sprintf(msg,"No se esperaba '%c'",c);
							error(msg);
							estado=-1;
						}
						break;
					case 2://la fraccion decimal, pueden seguir los digitos o e
						c=fgetc(fuente);
						if (isdigit(c))
						{
							id[++i]=c;
							estado=2;
						}
						else if(tolower(c)=='e')
						{
							id[++i]=c;
							estado=3;
						}
						else
							estado=6;
						break;
					case 3://una e, puede seguir +, - o una secuencia de digitos
						c=fgetc(fuente);
						if (c=='+' || c=='-')
						{
							id[++i]=c;
							estado=4;
						}
						else if(isdigit(c))
						{
							id[++i]=c;
							estado=5;
						}
						else{
							sprintf(msg,"No se esperaba '%c'",c);
							error(msg);
							estado=-1;
						}
						break;
					case 4://necesariamente debe venir por lo menos un digito
						c=fgetc(fuente);
						if (isdigit(c))
						{
							id[++i]=c;
							estado=5;
						}
						else{
							sprintf(msg,"No se esperaba '%c'",c);
							error(msg);
							estado=-1;
						}
						break;
					case 5://una secuencia de digitos correspondiente al exponente
						c=fgetc(fuente);
						if (isdigit(c))
						{
							id[++i]=c;
							estado=5;
						}
						else{
							estado=6;
						}break;
					case 6://estado de aceptacion, devolver el caracter correspondiente a otro componente lexico
						if (c!=EOF)
							ungetc(c,fuente);
						else
							c=0;
						id[++i]='\0';
						acepto=1;
						strcpy(t.lexema, id);
						t.compLex = LITERAL_NUM;
						fputs("LITERAL_NUM", salida);
						break;
					case -1:
						if (c == EOF)
							error("No se esperaba el fin de archivo");
						else
							error(msg);
					}
				}
			break;
		}

		
		else if (isalpha(c)) { //es una palabra reservada 
			i = 0;
			while (isalpha(c)) {
				id[i] = toupper(c);
				i++;
				c = fgetc(fuente);
				if (i >= TAMLEX) 
					error("Excede tamaño de buffer");	
			} 
			id[i]='\0';
			if (c != EOF)
				ungetc(c,fuente);
			else 		
			   	c= 0;
			strcpy(t.lexema, id);
			if (strcmp(id, "TRUE") == 0) {
				t.compLex = PR_TRUE;
				fputs("PR_TRUE", salida);
			}
			else if (strcmp(id, "FALSE") == 0) {
				t.compLex = PR_FALSE;
				fputs("PR_FALSE", salida);
			}
			else if (strcmp(id, "NULL") == 0) {
				t.compLex = PR_NULL;
				fputs("PR_NULL", salida);
			}
			else {
				error("No es posible reconocer el caracter");
			}
			break;
			
		}

		
		else if (c == '\"') {
			i = 0;
			c = fgetc(fuente);
			
			while (c != '\"') {
				if (c == EOF) { 
					error("Acabo la lectura de archivo sin terminar la cadena");
					break;
				} else {
					id[i] = c;
					c = fgetc(fuente);
					i++; 
				}
			}
			if (c == '\"') {	
				id[++i] = '\0';
				strcpy(t.lexema, id);
				t.compLex = LITERAL_CADENA;
				fputs(" LITERAL_CADENA ", salida);
			}
			break;
				
		}

		else if (c==',')
		{
			t.compLex = COMA ;
			strcpy(t.lexema, ",");
			fputs(" COMA ", salida);
			break;
		}
		else if (c == ':')
		{
			t.compLex = DOS_PUNTOS;
			strcpy(t.lexema, ":");
			fputs(" DOS_PUNTOS ", salida);
			break;
		}
		
		else if (c=='{')
		{
			t.compLex = L_LLAVE;
			strcpy(t.lexema, "{");
                        fputs(" L_LLAVE ", salida);
			break;
		}
		else if (c=='}')
		{
			t.compLex= R_LLAVE;
			strcpy(t.lexema, "}");
			fputs(" R_LLAVE ", salida);
			break;
		}
		else if (c!=EOF)
		{
			sprintf(msg,"%c no esperado",c);
			error(msg);
		}
	}
	if (c==EOF)
	{
		t.compLex=EOF;
		// strcpy(e.lexema,"EOF");
		fputs("EOF", salida);
		if (cantErrores == 0 ) {
			printf(".El analizador lexico acabó sin errores.\n");
		
		}
	}
}

int main()
{
	// inicializar analizador lexico
	FILE *archivo;
	FILE *salida;
	
	if (!((archivo = fopen("fuente.txt","rt")) && (salida = fopen("output.txt", "w"))))
	{
			printf("Archivo no encontrado.\n");
			exit(1);
	} else {
		do{
			sigLex(archivo, salida);
			
		} while(t.compLex != EOF);
		fclose(archivo);
		fclose(salida);
	}
	return 0;
}
