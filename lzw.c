/********************************************************************
**
**  Algoritmo para compress�o e descompress�o de arquivos ao formato LZW
**
**
*****************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include<allegro.h>

#define BITS 12                   /* Definindo o numero de Bits*/
#define HASHING_SHIFT 4  		 /* Definindo o Hashing  */
#define MAX_VALUE (1 << BITS) - 1 /* Definindo o que o ms-dos precisa  */
#define MAX_CODE MAX_VALUE - 1    /* Defini��o para Funcionar caso o numero de bits mude para 14*/
#define TABLE_SIZE 5021



// Variaveis Globais para controle
int *code_value;                  /* INDICES DO DICIONARIO    */
unsigned int *prefix_code;        /* Armazena os prefixos do dicion�rio   */
unsigned char *append_character;  /* Armazena os caracteres anexados */
unsigned char decode_stack[4000]; /* Aqui Guardamos a string decodificada  para ser colocada inversamente*/

/*
 *     Assinatura das fun��es
 */
void imprimiDicionario();
void compress(FILE *input,FILE *output);
void expand(FILE *input,FILE *output);
int find_match(int hash_prefix,unsigned int hash_character);
void output_code(FILE *output,unsigned int code);
unsigned int input_code(FILE *input);
unsigned char *decode_string(unsigned char *buffer,unsigned int code);


/* *************************************************************************************
             Implementa��o das fun��es de Controle
             1 - void compress(FILE *input,FILE *output);
             2-  void expand(FILE *input,FILE *output);
             3-  int find_match(int hash_prefix,unsigned int hash_character);
             4-  void output_code(FILE *output,unsigned int code);
             5-  unsigned int input_code(FILE *input);
             6-  unsigned char *decode_string(unsigned char *buffer,unsigned int code);
 *************************************************************************************
*/


/************************************************************
           S�ntese da fun��o de compress�o:
           De acordo com sua id�ia b�sica, a codifica��o LZW est� sempre tentando produzir 
           c�digos para cadeias de caracteres (strings) que j� s�o conhecidas, e cada vez 
           que um c�digo novo � produzido, uma string nova � acrescentada no dicion�rio, 
           que nada mais � que uma tabela de strings.
 ************************************************************
 */
void compress(FILE *input,FILE *output)
{
     FONT *vladmir;
     PALETTE palette;
     vladmir = load_font("Vladmir.pcx",palette, NULL);
  FILE *Dicionario;
  Dicionario = fopen("dicionario.txt","w+");   
  char *addDic;
  addDic = (char*) malloc(50);
unsigned int next_code; // Pr�ximo c�digo a ser usado no dicion�rio
unsigned int character; // pr�ximo caractere a ser pego no arquivo de entrada
unsigned int string_code; // primeiro caractere a ser pego, e posteriormente o prefixo
unsigned int index;  // index para hashing bin�rio 
         int i;  // ind�ce contador
//--------------------------------------
//          Indices de Interface         
   int x = 50;
   int y = 360;
//---------------------------------------

  next_code=256;              /* Definindo o c�digo padr�o, o pr�ximo ser� incluso no dicion�rio */
  
  for (i=0;i<TABLE_SIZE;i++)  /* La�o respons�vel por limpar  o Dicion�rio */
    code_value[i]=-1;         /* representamos o dicion�rio por um array code_value */
    
  i=0;
    textprintf_ex(screen,vladmir, 250, 160, makecol(0,0,0),-1,"Compressao em andamento");
    textprintf_ex(screen,vladmir, 250, 180, makecol(0,0,0),-1,"Arquivo Teste.txt para arquivoCompactado.lzw");
    rest(1500);
  string_code=getc(input);    /* Pega o Primeiro Caractere                       */
  
  
/*
** Temos aqui o Principal Loop, Este Loop funciona percorre todo o arquivo de entrada
    e cria o dicion�rio
*/
fprintf(Dicionario,"Pos. Dic  || c�digo \n");
  while ((character=getc(input)) != (unsigned)EOF)  
  /* Pega caracteres enquanto n�o acontecer o final do arquivo
     j� come�amos no segundo caractere, pois o primeiro foi pego por string_code */

  {
    index=find_match(string_code,character);/* Verifica se a sequ�ncia est� no dicion�rio, caso esteja, obt�m o  c�digo livre  */
    if (code_value[index] != -1)     
    {       /* caso n�o esteja, adiciona no dicion�rio */
      string_code=code_value[index];
 
         } 
    else                                  
    {                                    
      if (next_code <= MAX_CODE) // Verifica se pode adicionar
      {
        code_value[index]=next_code++;
        prefix_code[index]=string_code;
        append_character[index]=character;
      }        
      // Code value[index] = valor da entrada no dicionario(257,258..) append_char[index] s�o os valores das novas entradas no dic
      // index � a parte bin�ria 
      output_code(output,string_code);  /* Quando a String � encontrada e n�o ta no dicion�rio, imprimimos a ultima string,  */
        fprintf(Dicionario,"\n %d",code_value[index]);
         if(prefix_code[index] >=256){
                               
      fprintf(Dicionario,"       %d + %c ",prefix_code[index],append_character[index]);
      }else{
            fprintf(Dicionario,"        %c%c ",prefix_code[index],append_character[index]);
            }
   //   rest(1000);
      free(addDic);
      string_code=character;            /* e depois adicionamos a nova*/
  
    }                                   
  }                                   
/*
** Fim do Looping Principal
*/
  output_code(output,string_code); /* Grava o c�digo de sa�da           */
  output_code(output,MAX_VALUE);   /* Grava a extremidade do codigo     */
  output_code(output,0);           /* Libera o buffer de sa�da;*/
    textprintf_ex(screen,vladmir, 270, 200, makecol(0,0,0),-1,"Compressao Completa!");
    rest(2000);
  fclose(Dicionario);
}



/*
** Esta � a rotina de hashing. Ele tenta encontrar uma correspond�ncia para o prefixo + char string na tabela string. 
** Se encontr�-lo, o �ndice � retornado. Se a string n�o for encontrada, o primeiro �ndice dispon�vel na tabela de String � retornado.
*/
int find_match(int hash_prefix,unsigned int hash_character) // Encontra correspondente ou posi��o livre
{
int index;
int offset;

  index = (hash_character << HASHING_SHIFT) ^ hash_prefix;
  if (index == 0)
    offset = 1;
  else
    offset = TABLE_SIZE - index;
  while (1)
  {
        // Se o code_valu[index]( Posi��o do dicion�rio) for igual a -1, ou seja, posi��o livre, 
        // retorne o index. para adi��o.
    if (code_value[index] == -1)
      return(index);
      /* Se prefix_code[index] for igual ao hash_prefix( string_code) 
          e o append_character[index] for igual ao hash_character ( character, pr�ximo caracter)
          ent�o a palavra j� existe , e retornaremos o ind�ce desta palavra */
    if (prefix_code[index] == hash_prefix && 
        append_character[index] == hash_character)
      return(index);
      
      // Caso o ind�ce n�o tenha sido encontrado, 
      // avan�amos at� que chegue uma posi��o livre, ou a palavra seja escontrada no dicion�rio
    index -= offset;
    if (index < 0)
      index += TABLE_SIZE;
  }
}



/*
**  Esta � a rotina de expans�o. � preciso um arquivo de formato LZW, e amplia-lo para um arquivo de sa�da.
*/


/* 
   Sintese da fun��o expand(FILE *input,FILE *output) : 
   Na decodifica��o, o descompressor recebe o c�digo compactado como entrada, vai 
   reconstituindo a tabela do dicion�rio e gerando como sa�da �s cadeias de caracteres originais 
   que foram codificadas na compress�o.
*/
void expand(FILE *input,FILE *output)
{
FONT *vladmir;
     PALETTE palette;
     vladmir = load_font("Vladmir.pcx",palette, NULL);
unsigned int next_code; //
unsigned int new_code; // 
unsigned int old_code; // cod�go do antecessor 
int character;
unsigned char *string;

  next_code=256;           /* Aqui temos o pr�ximo c�digo, e um contador.*/      
  textprintf_ex(screen,vladmir, 270, 170, makecol(0,0,0),-1,"Descompressao em Andamento!");
   textprintf_ex(screen,vladmir, 270, 180, makecol(0,0,0),-1,"Arquivo arquivoCompactado.lzw ");
     textprintf_ex(screen,vladmir, 290, 200, makecol(0,0,0),-1,"-------------------------");
       textprintf_ex(screen,vladmir, 270, 220, makecol(0,0,0),-1,"arquivoDescompactado.txt");
   rest(1500);
  old_code=input_code(input);  /* Le a primeira palavra e inicializa a variavel caracter, */
  character=old_code;          /* envia a primeira palavra para o c�digo de sa�da. */
  putc(old_code,output);      
   
/*
**  Este � o ciclo de expans�o principal. 
**  Ele l� em caracteres do arquivo LZW at� que ele v� o c�digo especial usado para inidicate o fim dos dados.
*/

  while ((new_code=input_code(input)) != (MAX_VALUE))
  {
/*
**  Verificamos a ocorr�ncia especial deste algoritmo,
** que �  a repeti��o de strings, como ABABA  ( ou STRING  + Char + STRING + Char +  STRING) que geraria um c�digo indefinido. 
** Ele lida com ela pela decodifica��o do �ltimo c�digo, e adicionando um �nico Char ao final da cadeia de decodifica��o.
*/
    if (new_code>=next_code)
    {
      *decode_stack=character;
      string=decode_string(decode_stack+1,old_code);
    }
/*
** Caso contr�rio, vamos fazer uma decodifica��o  do novo c�digo.
*/
    else{
      string=decode_string(decode_stack,new_code);
         }
      
/*
** Agora temos sa�da a string decodificada em ordem inversa.
*/
    character=*string;
              //        fprintf(Dicionario," decode = %d ",next_code);
    while (string >= decode_stack)
    {
      putc(*string--,output);
      }
/*
** Finalmente, se poss�vel, adicionar um novo c�digo para a tabela de string.
*/
    if (next_code <= MAX_CODE)
    {
      prefix_code[next_code]=old_code;
      append_character[next_code]=character;
      next_code++;
    }
    
    old_code=new_code;
  }
      textprintf_ex(screen,vladmir, 270, 240, makecol(0,0,0),-1,"Descompressao Completa!");
      rest(2000);
}

/*
** Essa rotina simplesmente decodifica uma seq��ncia da tabela de string, armazenando-o em um buffer. 
** O buffer pode ent�o ser produzido em ordem inversa pelo programa de expans�o.
*/

unsigned char *decode_string(unsigned char *buffer,unsigned int code)
{
  FILE *Dicionario;
         Dicionario = fopen("dicnovo.txt","a+");
int codigo = code;  
int i;

  i=0;
  while (code > 255) // Enquanto for maior que os caracteres ascii.
  {
        
    *buffer++ = append_character[code];
    code=prefix_code[code];
    if (i++>=MAX_CODE)
    {
      textprintf_ex(screen, font, 255,160, makecol(100,255,125),-1,"Erro Fatal Durante Expansao");
      exit(-3);
    }
  }
  fprintf(Dicionario,"Cod: %d Cont: %s \n",codigo,buffer);
  fclose(Dicionario);
  *buffer=code;
  return(buffer);
}

/*
** As duas rotinas que se seguem s�o usadas para transmitir os c�digos de comprimento vari�vel.
**  S�o escritos estritamente para maior clareza, e n�o s�o particurlamente eficientes.
*/

unsigned int input_code(FILE *input)
{
unsigned int return_value;
static int input_bit_count=0;
static unsigned long input_bit_buffer=0L;

  while (input_bit_count <= 24)
  {
    input_bit_buffer |= 
        (unsigned long) getc(input) << (24-input_bit_count);
    input_bit_count += 8;
  }
  return_value=input_bit_buffer >> (32-BITS);
  input_bit_buffer <<= BITS;
  input_bit_count -= BITS;
  return(return_value);
}

void output_code(FILE *output,unsigned int code)
{
static int output_bit_count=0;
static unsigned long output_bit_buffer=0L;

  output_bit_buffer |= (unsigned long) code << (32-BITS-output_bit_count);
  output_bit_count += BITS;
  while (output_bit_count >= 8)
  {
    putc(output_bit_buffer >> 24,output);
    output_bit_buffer <<= 8;
    output_bit_count -= 8;
  }
}
