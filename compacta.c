/********************************************************************
**
**  Algoritmo para compressão de arquivos ao formato LZW
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


#define BITS 12                   /* Definindo o numero de Bits*/  //This means codes 0-255 refer to individual bytes, while codes 256-4095 refer to substrings.
#define HASHING_SHIFT (BITS-8)    /* Definindo o Hashing  */
#define MAX_VALUE (1 << BITS) - 1 /* Definindo o que o ms-dos precisa  */
#define MAX_CODE MAX_VALUE - 1    /* Definição para Funcionar caso o numero de bits mude para 14*/         
#define TABLE_SIZE 5021


void *malloc();

extern int *code_value;                 	/* Valor do código matriz        */
extern unsigned int *prefix_code;   		 /* Armazena os prefixos do dicionário   */
extern unsigned char *append_character;   	/* Armazena os caracteres anexados */
extern unsigned char decode_stack[4000]; 	/* Aqui Guardamos a string decodificada */

/*
 *     Assinatura das funções
 */
void compress(FILE *input,FILE *output);
void porcentagemCompactacao(FILE *original, FILE *compactado);
/********************************************************************
**
** Este programa recebe um nome de arquivo a partir da linha de comando. 
** Ele comprime o arquivo, colocando a sua saída em um arquivo chamado arquivoCompactado.lzw. 
** Em seguida, se expande em test.lzw test.out. Test.out deve então ser uma duplicata exata do arquivo de entrada.
**
*************************************************************************/


void compactaArquivo()
{
FILE *input_file;
FILE *output_file;
FILE *lzw_file;
char input_file_name[81];

/*
**  The three buffers are needed for the compression phase.
*/
  code_value=(int*)malloc(TABLE_SIZE*sizeof(int));
  prefix_code=(unsigned int *)malloc(TABLE_SIZE*sizeof(unsigned int));
  append_character=(unsigned char *)malloc(TABLE_SIZE*sizeof(unsigned char));
  if (code_value==NULL || prefix_code==NULL || append_character==NULL)
  {
  textprintf_ex(screen, font, 255,160, makecol(100,255,125),-1,"ERRO FATAL AO ALOCAR MEMORIA!");
    exit(-1);
  }
/*
** Pega o nome do arquivo, abre este, e em seguida abre o arquivo de saida lzw
*/
 
  input_file=fopen("teste1.txt","rb+");
  lzw_file=fopen("arquivoCompactado.lzw","wb");
  if (input_file==NULL || lzw_file==NULL)
  {
  textprintf_ex(screen, font, 255,160, makecol(100,255,125),-1,"ERRO FATAL AO ABRIR ARQUIVOS!");
  rest(400);
    exit(-1);
  };
/*
** Comprimindo o arquivo
*/
  compress(input_file,lzw_file);
  porcentagemCompactacao(input_file,lzw_file);
  fclose(input_file);
  fclose(lzw_file);
  free(code_value);
  free(prefix_code);
  free(append_character);
}

// FUNÇÃO QUE INFORMA A PORCENTAGEM DA COMPACTAÇÃO DO ARQUIVO
void porcentagemCompactacao(FILE *original, FILE *compactado){
      FONT *vladmir;
     PALETTE palette;
     vladmir = load_font("Vladmir.pcx",palette, NULL);
         // textprintf_ex(PorcentagemFundo,vladmir, 265, 245, makecol(185,122,87),-1, "%3.0f", porcentagemCompactacao); 
       BITMAP *PorcentagemFundo = load_bitmap("Porcentagem.bmp",NULL);
     unsigned long tamOriginal;
     unsigned long tamCompactado;
     float porcentagemCompactacao;
     fseek(original,0,SEEK_END);
     fseek(compactado,0,SEEK_END);
     tamOriginal = ftell(original);
     tamCompactado = ftell(compactado);
     porcentagemCompactacao = tamCompactado * 100 / tamOriginal ;
     textprintf_ex(PorcentagemFundo,vladmir, 265, 245, makecol(185,122,87),-1, "%3.0f", porcentagemCompactacao);
      draw_sprite(screen,PorcentagemFundo,0,0);
    	destroy_bitmap(PorcentagemFundo);
    	 destroy_font(vladmir);
    	rest(1200);
       getch();
       fflush(stdin);
       }
// FUNÇÃO QUE IMPRIMI O DICIONÁRIO NA TELA
void imprimiDicionario(){
     FILE *Dicionario;
     Dicionario = fopen("dicionario.txt","r");
     char *linha;
     linha = (char*) malloc(100);
     int x, y ;
      x= 240;
      y = 190;
      FONT *vladmir;
     PALETTE palette;
     vladmir = load_font("Vladmir.pcx",palette, NULL);
       BITMAP *FundoDic = load_bitmap("FundoDic.bmp",NULL);
     while(!feof(Dicionario))
       {
         fgets(linha,100,Dicionario);
          textprintf_ex(FundoDic,vladmir, x, y, makecol(185,122,87),-1, "%s", linha);
          rest(80);
          draw_sprite(screen,FundoDic,0,0);
          y+= 15;
          if(y >= 310) {
                  y = 190;
                  rest(1200);
                  fflush(stdin);
                  char c = getchar();
                  while(c != key[KEY_RIGHT]){
                  if(c == key[KEY_ESC]){ 
                                         destroy_bitmap(FundoDic);
                                         destroy_font(vladmir);
                                         fclose(Dicionario);
                                         return; }
                         c = getchar();
                                         }
                  FundoDic = load_bitmap("FundoDic.bmp",NULL);
                  textprintf_ex(FundoDic,vladmir, x, y-15, makecol(185,122,87),-1, "Pos. Dic  || codigo");
                  }
                  }
                  rest(600);
                  }
// FUNÇÃO QUE PERMITE AO USUÁRIO SELECIONAR SE DESEJA VER OU NÃO O DICIONÁRIO          
void verDicionario(){
       BITMAP *fundoVisualizaDic = load_bitmap("visualizaDicionario.bmp",NULL);
       BITMAP *bob = load_bitmap("cursor.bmp",NULL);
        BITMAP *lula = load_bitmap("cursor.bmp",NULL);
        char c;
        int pos = 0 ;
         draw_sprite(screen,fundoVisualizaDic,0,0);
             draw_sprite(screen,bob,265,235);
             fflush(stdin);
             rest(80);
              
        do
        {
              fflush(stdin);
        c = getchar();
        if(c == key[KEY_ENTER]) {
                if((pos == 0 ) || (pos == 1)) {
                                            imprimiDicionario();
                                            draw_sprite(screen,fundoVisualizaDic,0,0);
                                              draw_sprite(screen,bob,265,235);
                                               fflush(stdin);
                                                rest(80);
                                                c = getchar();
                                            }
                if(pos == 2) {  destroy_bitmap(fundoVisualizaDic); 
                             destroy_bitmap(bob); 
                             destroy_bitmap(lula);
                             return;
                }
                }
        if(c == key[KEY_RIGHT]){
                                BITMAP *fundoVisualizaDic = load_bitmap("visualizaDicionario.bmp",NULL);
    	                        draw_sprite(screen,fundoVisualizaDic,0,0);
    	                        destroy_bitmap(fundoVisualizaDic);
                                   draw_sprite(screen,lula,450,235);
                                   rest(80);
                                   pos = 2 ;
                                   }
       if(c == key[KEY_LEFT]){
                                BITMAP *fundoVisualizaDic = load_bitmap("visualizaDicionario.bmp",NULL);
    	                        draw_sprite(screen,fundoVisualizaDic,0,0);
    	                        destroy_bitmap(fundoVisualizaDic);
                                   draw_sprite(screen,bob,260,235);
                                   rest(80);
                                   pos = 2 ;
                                   }                             
                                   }while(1);
                                   }                
     
