/********************************************************************
**
**  Algoritmo para compressão de arquivos ao formato LZW
**
** 25/07/1993
**
** 
**
*****************************************************************************/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <conio.h>
#include <malloc.h>
#include <allegro.h>
#include<stdio.h>
#include"funcoes.h"



#define BITS 12                   /* Definindo o numero de Bits*/
#define HASHING_SHIFT (BITS-8)    /* Definindo o Hashing  */
#define MAX_VALUE (1 << BITS) - 1 /* Definindo o que o ms-dos precisa  */
#define MAX_CODE MAX_VALUE - 1    /* Definição para Funcionar caso o numero de bits mude para 14*/
#define TABLE_SIZE 5021

void *malloc();

extern int *code_value;                 /* Valor do código matriz        */
extern unsigned int *prefix_code;    /* Armazena os prefixos do dicionário   */
extern unsigned char *append_character;   /* Armazena os caracteres anexados */
extern unsigned char decode_stack[4000]; /* Aqui Guardamos a string decodificada */

/*
 *     Assinatura das funções
 */
void compress(FILE *input,FILE *output);
void expand(FILE *input,FILE *output);
int find_match(int hash_prefix,unsigned int hash_character);
void output_code(FILE *output,unsigned int code);
unsigned int input_code(FILE *input);
unsigned char *decode_string(unsigned char *buffer,unsigned int code);

/********************************************************************
**
** Este programa expande em test.lzw para  arquivoDescompactado.txt deve então ser uma duplicata exata do arquivo de entrada.
**
*************************************************************************/


void descompactaArquivo()
{
FILE *input_file;
FILE *output_file;
FILE *lzw_file;
char input_file_name[81];

/*
**  Os três buffers são necessários para a fase de compressão.
*/
  code_value=(int*)malloc(TABLE_SIZE*sizeof(int));
  prefix_code=(unsigned int *)malloc(TABLE_SIZE*sizeof(unsigned int));
  append_character=(unsigned char *)malloc(TABLE_SIZE*sizeof(unsigned char));
  if (code_value==NULL || prefix_code==NULL || append_character==NULL)
  {
  textprintf(screen, font, 255,160, makecol(100,255,125),"ERRO FATAL AO ALOCAR MEMORIA!");
  rest(800);
    exit(-1);
  }

/*
** Agora abra arquivos para a expansao
*/
  lzw_file=fopen("arquivoCompactado.lzw","rb");
  output_file=fopen("arquivoDescompactado.txt","wb");
  if (lzw_file==NULL || output_file==NULL)
  {
  textprintf(screen, font, 255,160, makecol(100,255,125),"ERRO FATAL AO ABRIR ARQUIVOS!");
  rest(800);
    exit(-2);
  };
/*
** Descompactando o Arquivo
*/
  expand(lzw_file,output_file);
  fclose(lzw_file);
  fclose(output_file);
  free(prefix_code);
  free(append_character);
}
