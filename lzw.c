/********************************************************************
**
**  Algoritmo para compressão e descompressão de arquivos ao formato LZW
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
#define MAX_CODE MAX_VALUE - 1    /* Definição para Funcionar caso o numero de bits mude para 14*/
#define TABLE_SIZE 5021



// Variaveis Globais para controle
int *code_value;                  /* INDICES DO DICIONARIO    */
unsigned int *prefix_code;        /* Armazena os prefixos do dicionário   */
unsigned char *append_character;  /* Armazena os caracteres anexados */
unsigned char decode_stack[4000]; /* Aqui Guardamos a string decodificada  para ser colocada inversamente*/

/*
 *     Assinatura das funções
 */
void imprimiDicionario();
void compress(FILE *input,FILE *output);
void expand(FILE *input,FILE *output);
int find_match(int hash_prefix,unsigned int hash_character);
void output_code(FILE *output,unsigned int code);
unsigned int input_code(FILE *input);
unsigned char *decode_string(unsigned char *buffer,unsigned int code);


/* *************************************************************************************
             Implementação das funções de Controle
             1 - void compress(FILE *input,FILE *output);
             2-  void expand(FILE *input,FILE *output);
             3-  int find_match(int hash_prefix,unsigned int hash_character);
             4-  void output_code(FILE *output,unsigned int code);
             5-  unsigned int input_code(FILE *input);
             6-  unsigned char *decode_string(unsigned char *buffer,unsigned int code);
 *************************************************************************************
*/


/************************************************************
           Síntese da função de compressão:
           De acordo com sua idéia básica, a codificação LZW está sempre tentando produzir 
           códigos para cadeias de caracteres (strings) que já são conhecidas, e cada vez 
           que um código novo é produzido, uma string nova é acrescentada no dicionário, 
           que nada mais é que uma tabela de strings.
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
unsigned int next_code; // Próximo código a ser usado no dicionário
unsigned int character; // próximo caractere a ser pego no arquivo de entrada
unsigned int string_code; // primeiro caractere a ser pego, e posteriormente o prefixo
unsigned int index;  // index para hashing binário 
         int i;  // indíce contador
//--------------------------------------
//          Indices de Interface         
   int x = 50;
   int y = 360;
//---------------------------------------

  next_code=256;              /* Definindo o código padrão, o próximo será incluso no dicionário */
  
  for (i=0;i<TABLE_SIZE;i++)  /* Laço responsável por limpar  o Dicionário */
    code_value[i]=-1;         /* representamos o dicionário por um array code_value */
    
  i=0;
    textprintf_ex(screen,vladmir, 250, 160, makecol(0,0,0),-1,"Compressao em andamento");
    textprintf_ex(screen,vladmir, 250, 180, makecol(0,0,0),-1,"Arquivo Teste.txt para arquivoCompactado.lzw");
    rest(1500);
  string_code=getc(input);    /* Pega o Primeiro Caractere                       */
  
  
/*
** Temos aqui o Principal Loop, Este Loop funciona percorre todo o arquivo de entrada
    e cria o dicionário
*/
fprintf(Dicionario,"Pos. Dic  || código \n");
  while ((character=getc(input)) != (unsigned)EOF)  
  /* Pega caracteres enquanto não acontecer o final do arquivo
     já começamos no segundo caractere, pois o primeiro foi pego por string_code */

  {
    index=find_match(string_code,character);/* Verifica se a sequência está no dicionário, caso esteja, obtém o  código livre  */
    if (code_value[index] != -1)     
    {       /* caso não esteja, adiciona no dicionário */
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
      // Code value[index] = valor da entrada no dicionario(257,258..) append_char[index] são os valores das novas entradas no dic
      // index é a parte binária 
      output_code(output,string_code);  /* Quando a String é encontrada e não ta no dicionário, imprimimos a ultima string,  */
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
  output_code(output,string_code); /* Grava o código de saída           */
  output_code(output,MAX_VALUE);   /* Grava a extremidade do codigo     */
  output_code(output,0);           /* Libera o buffer de saída;*/
    textprintf_ex(screen,vladmir, 270, 200, makecol(0,0,0),-1,"Compressao Completa!");
    rest(2000);
  fclose(Dicionario);
}



/*
** Esta é a rotina de hashing. Ele tenta encontrar uma correspondência para o prefixo + char string na tabela string. 
** Se encontrá-lo, o índice é retornado. Se a string não for encontrada, o primeiro índice disponível na tabela de String é retornado.
*/
int find_match(int hash_prefix,unsigned int hash_character) // Encontra correspondente ou posição livre
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
        // Se o code_valu[index]( Posição do dicionário) for igual a -1, ou seja, posição livre, 
        // retorne o index. para adição.
    if (code_value[index] == -1)
      return(index);
      /* Se prefix_code[index] for igual ao hash_prefix( string_code) 
          e o append_character[index] for igual ao hash_character ( character, próximo caracter)
          então a palavra já existe , e retornaremos o indíce desta palavra */
    if (prefix_code[index] == hash_prefix && 
        append_character[index] == hash_character)
      return(index);
      
      // Caso o indíce não tenha sido encontrado, 
      // avançamos até que chegue uma posição livre, ou a palavra seja escontrada no dicionário
    index -= offset;
    if (index < 0)
      index += TABLE_SIZE;
  }
}



/*
**  Esta é a rotina de expansão. É preciso um arquivo de formato LZW, e amplia-lo para um arquivo de saída.
*/


/* 
   Sintese da função expand(FILE *input,FILE *output) : 
   Na decodificação, o descompressor recebe o código compactado como entrada, vai 
   reconstituindo a tabela do dicionário e gerando como saída às cadeias de caracteres originais 
   que foram codificadas na compressão.
*/
void expand(FILE *input,FILE *output)
{
FONT *vladmir;
     PALETTE palette;
     vladmir = load_font("Vladmir.pcx",palette, NULL);
unsigned int next_code; //
unsigned int new_code; // 
unsigned int old_code; // codígo do antecessor 
int character;
unsigned char *string;

  next_code=256;           /* Aqui temos o próximo código, e um contador.*/      
  textprintf_ex(screen,vladmir, 270, 170, makecol(0,0,0),-1,"Descompressao em Andamento!");
   textprintf_ex(screen,vladmir, 270, 180, makecol(0,0,0),-1,"Arquivo arquivoCompactado.lzw ");
     textprintf_ex(screen,vladmir, 290, 200, makecol(0,0,0),-1,"-------------------------");
       textprintf_ex(screen,vladmir, 270, 220, makecol(0,0,0),-1,"arquivoDescompactado.txt");
   rest(1500);
  old_code=input_code(input);  /* Le a primeira palavra e inicializa a variavel caracter, */
  character=old_code;          /* envia a primeira palavra para o código de saída. */
  putc(old_code,output);      
   
/*
**  Este é o ciclo de expansão principal. 
**  Ele lê em caracteres do arquivo LZW até que ele vê o código especial usado para inidicate o fim dos dados.
*/

  while ((new_code=input_code(input)) != (MAX_VALUE))
  {
/*
**  Verificamos a ocorrência especial deste algoritmo,
** que é  a repetição de strings, como ABABA  ( ou STRING  + Char + STRING + Char +  STRING) que geraria um código indefinido. 
** Ele lida com ela pela decodificação do último código, e adicionando um único Char ao final da cadeia de decodificação.
*/
    if (new_code>=next_code)
    {
      *decode_stack=character;
      string=decode_string(decode_stack+1,old_code);
    }
/*
** Caso contrário, vamos fazer uma decodificação  do novo código.
*/
    else{
      string=decode_string(decode_stack,new_code);
         }
      
/*
** Agora temos saída a string decodificada em ordem inversa.
*/
    character=*string;
              //        fprintf(Dicionario," decode = %d ",next_code);
    while (string >= decode_stack)
    {
      putc(*string--,output);
      }
/*
** Finalmente, se possível, adicionar um novo código para a tabela de string.
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
** Essa rotina simplesmente decodifica uma seqüência da tabela de string, armazenando-o em um buffer. 
** O buffer pode então ser produzido em ordem inversa pelo programa de expansão.
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
** As duas rotinas que se seguem são usadas para transmitir os códigos de comprimento variável.
**  São escritos estritamente para maior clareza, e não são particurlamente eficientes.
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
