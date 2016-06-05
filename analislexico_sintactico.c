/*
 *	Analizador Léxico 
 *	Curso: Compiladores y Lenguajes de Bajo de Nivel
 *	Descripcion:
 *	Implementa un analizador léxico que reconoce números, literales en cadena y signos de puntuación para un lenguaje
 * 	con sintaxis tipo Json.
 *  
 *  Analizador Sintactico
 *	Descripcion:
 *Implementar un Analizador Sintáctico que reconozca códigos 
 *fuentes escritos en el lenguaje Json simplificado propuesto
 *	
 */

/*********** Inclusión de cabecera **************/
#include "analislexico_sintactico.h"

/************* Variables globales **************/
	
char cad[5*TAMLEX];		// string utilizado para cargar mensajes de error
token t;// token global para recibir componentes del Analizador Lexico

// variables para el analizador lexico
char buff[2*TAMBUFF];	// Buffer para lectura de archivo fuente
char id[TAMLEX];		// Utilizado por el analizador lexico
int numLinea=1;			// Numero de Linea
int i = 0;
int cantErrores = 0, e=0;
FILE *fuente;
FILE *salida;
/**************** Funciones **********************/

// Rutinas del analizador lexico
void error(const char* mensaje) {
	cantErrores++;
	if (cantErrores == 1 )
		printf("Se encontraron lexemas no válidos ");
	printf("Linea %d: Error Lexico. %s.\n",numLinea,mensaje);	
	
}

void sigLex() {
	
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
			continue;
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
							//printf
							sprintf(msg,"No se esperaba '%c'",c);
							error(msg);
							estado=-1;
							exit(1);
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
							exit(1);
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
							exit(1);
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
					error("Acabo la lectura de archivo sin terminar la cadena ");
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
		strcpy(t.lexema,"EOF");
		fputs("EOF", salida);
		if (cantErrores == 0 ) {
			printf(".El analizador lexico acabó sin errores.\n");
		
		}
		
	}
}

/*ANALSIS SINTACTICO*/
int p_json[2] = {L_LLAVE, L_CORCHETE};
int p_element[2]= {L_LLAVE, L_CORCHETE};
int p_object[1] ={L_LLAVE};
int p_objectO[2] = {LITERAL_CADENA, R_LLAVE};
int p_attList[1] = {LITERAL_CADENA};
int p_att[2] = {COMA};
int p_array[1] = {L_CORCHETE};
int p_arrayA[3] = {L_LLAVE, L_CORCHETE, R_CORCHETE};
int p_attribute[1] = {LITERAL_CADENA};
int p_att_name[1] = {LITERAL_CADENA};
int p_att_value[7] = {L_LLAVE, L_CORCHETE, LITERAL_CADENA,LITERAL_NUM,PR_TRUE,PR_FALSE,PR_NULL};
int p_ele[2] = {COMA};
int p_eleList[2] = {L_LLAVE, L_CORCHETE};


int s_json[1] = {EOF};
int s_element[4]= {COMA,R_LLAVE,R_CORCHETE, EOF};
int s_object[4] ={COMA,R_LLAVE,R_CORCHETE, EOF};
int s_objectO[4] = {COMA,R_LLAVE,R_CORCHETE, EOF};
int s_attList[1] = {R_LLAVE};
int s_att[1] = {R_LLAVE};
int s_array[4] = {COMA,R_LLAVE,R_CORCHETE, EOF};
int s_arrayA[4] = {COMA,R_LLAVE, R_CORCHETE, EOF};
int s_attribute[2] = {COMA, R_LLAVE};
int s_att_name[1] = {DOS_PUNTOS};
int s_att_value[2] = {COMA,R_LLAVE};
int s_ele[1] = {R_CORCHETE};
int s_eleList[1] = {R_CORCHETE};

char *tokens[]={"L_CORCHETE", "R_CORCHETE", "L_LLAVE", "R_LLAVE", "COMA", "EOF", "DOS_PUNTOS", "LITERAL_CADENA", "LITERAL_NUM", "PR_TRUE", "PR_FALSE", "PR_NULL"};


void element(int synchset[]);

int busqueda(int v[], int len) {
   int i=0;
   
   do { 
      

      if (v[i] == t.compLex) {
         
         return 1;
      }
      i++;
   } while (i < len) ;
   return 0;
}

int *unnion(int prim[], int sig[], int len1, int len2) {
 
   int len= len1 + len2 + 1;
   int i=1;
   static int v[13];
   v[0]=EOF;
   while (len1 > i) { 
      v[i] == sig[i];
      i++;
   }
   i = 0;
   while (len2 > i) { 
      v[i] == prim[i];
      i++;
   }
   return v;
}

void errorS(){
    printf("Error sintactico en la linea: %d. No se esperaba %s, componente lexico: %s\n",numLinea,t.lexema,tokens[t.compLex]);
    e++;
}

void match(int lex) { 
    
    if (t.compLex == lex) 
	sigLex();
    else 
	errorS();
}

void scanto(int synchset[], int len) {
   do {
      sigLex();
      if (t.compLex == EOF) {
        match(EOF);
	exit(1);
      }
   } while (!busqueda(synchset, len));
}


void check_input(int primeros[], int siguientes[], int len1, int len2) {
    
    if (!busqueda(primeros, len1)){ 
       
       errorS(); 
       printf("%d",numLinea);
       scanto(unnion(primeros, siguientes, len1, len2), len1+len2+1);
    }
}


void attribute_value(int synchset[]) {
    check_input(p_att_value, s_att_value, 7, 2);
    switch(t.compLex) {
      case LITERAL_CADENA:    
         match(LITERAL_CADENA);
         break;
      case LITERAL_NUM:
         match(LITERAL_NUM);
         break;
      case PR_TRUE:
         match(PR_TRUE);
         break;
      case PR_FALSE:
         match(PR_FALSE);
         break;
      case PR_NULL:
         match(PR_NULL);
         break;
      default:
         element(s_element);
         break;
   }
   check_input(synchset, p_att_value, 2, 7);
}

void attribute_name(int synchset[]) {
   check_input(p_att_name, s_att_name, 1, 1);
   match(LITERAL_CADENA);
   check_input(synchset, p_att_name, 1, 1);
}

void attribute(int synchset[]) {
   check_input(p_attribute, s_attribute, 1, 2);
   attribute_name(s_att_name);
   match(DOS_PUNTOS);  
   attribute_value(s_att_value);
   
   check_input(synchset, p_attribute, 2, 1);
}


void att(int synchset[]) {
  
   if (t.compLex == COMA) {
      check_input(p_att, s_att, 1,1);
      match(COMA);
    
      attribute(s_attribute);
      
      att(s_att);
      
      check_input(synchset, p_att, 1, 1);
   } 
   
} 

void ele(int synchset[]) {
   
   if (t.compLex == COMA) {
   check_input(p_ele, s_ele, 1, 1);
      match(COMA);
      element(s_element);
      ele(s_ele);
   check_input(synchset, p_ele, 1, 1);
   }   
}

void attribute_list(int synchset[]) {
   check_input(p_attList, s_attList, 1, 1);
   attribute(s_attribute);
   
   att(s_att);
   
   check_input(synchset, p_attList, 1, 1);
}

void element_list(int synchset[]) {
   check_input(p_eleList, s_eleList, 2, 1);
   element(s_element);
   ele(s_ele);
   check_input(synchset, p_eleList, 1, 2);
}


void objectO(int synchset[]) {
   check_input(p_objectO, s_objectO, 2, 4);
   if (t.compLex == R_LLAVE) 
      match(R_LLAVE);
   else {
     
      attribute_list(s_attList);
     
      match(R_LLAVE);
   }
    check_input(synchset, p_objectO, 4, 2);
}

void arrayA(int synchset[]) {
   check_input(p_arrayA, s_arrayA, 3, 4);
   if (t.compLex == R_CORCHETE) 
      match(R_CORCHETE);
   else {
      element_list(s_eleList);
      match(R_CORCHETE);
   }
    check_input(synchset, p_arrayA, 4, 3);
}


void array(int synchset[]) {
   check_input(p_array, s_array, 1, 4);
   if (t.compLex == L_CORCHETE) {
      match(L_CORCHETE);
      arrayA(s_arrayA);
   } 
   check_input(synchset, p_array, 4, 1);
}


void object(int synchset[]) {
   check_input(p_object, s_object, 1, 4);
   if (t.compLex == L_LLAVE) {
      match(L_LLAVE);
      objectO(s_objectO);
   } 
   check_input(synchset, p_object, 4, 1);
}

void element(int synchset[]) {
   check_input(p_element, s_element, 2, 4);
   if (t.compLex == L_LLAVE) {
      object(s_object);
   } else if (t.compLex == L_CORCHETE) { 
      array(s_array);
   } 
   check_input(synchset, p_element, 4, 2);
}

void json() {
   check_input(p_json, s_json, 2, 1);
   element(s_element);
   if (e== 0) 
	printf("No se encontraron errores sintacticos\n");
   exit(1);
}


int main() {
	if (!((fuente = fopen("fuente.txt","rt")) && (salida = fopen("output.txt", "w"))))
	{
			printf("Archivo no encontrado.\n");
			exit(1);
	} else {
		do{
			sigLex();
			json();
		} while(t.compLex != EOF);
		fclose(fuente);
		fclose(salida);
	}
	return 0;
}


