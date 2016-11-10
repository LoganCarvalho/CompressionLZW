/********************************************************************
**
** 
**
**  Algoritmo para compressão de arquivos ao formato LZW
*****************************************************************************/

#include <allegro.h>
#include<stdio.h>
#include<stdlib.h>
#include"funcoes.h"

// ************************************************************
//             Implementação das funções de Interface
// ************************************************************



/*     ************************************************************
       Esta função limpa a tela e imprime o Menu.
       ************************************************************
 */
void limpa_menu(void)
{
     BITMAP *fundo = load_bitmap("menu.bmp",NULL);
    	draw_sprite(screen,fundo,0,0);
    	destroy_bitmap(fundo);
}


/* ************************************************************
** Esta função limpa a tela e imprimi a tela para captura do nome do arquivo
 ************************************************************
*/
void limpa_menuCaptura(void)
{
     BITMAP *fundo = load_bitmap("menuCaptura.bmp",NULL);
    	draw_sprite(screen,fundo,0,0);
    	destroy_bitmap(fundo);
}


// ************************************************************
// Principal função de interface, responsável por selecionar a opção do usuário
// ************************************************************
void Menu()
{
     fflush(stdin);
     char c;
     int pos = 0;
     limpa_menu();
     BITMAP *fundo = load_bitmap("cursor.bmp",NULL);
     draw_sprite(screen,fundo,255,240);
    
    do
        {
        c = getchar();
        if(c == key[KEY_ENTER]) {
                                 limpa_menuCaptura();
                                 rest(80);
                                 if(pos == 3) {
                                         BITMAP *credito = load_bitmap("creditos.bmp",NULL);
                                          draw_sprite(screen,credito,0,0);
                                          rest(3000);
                                          fflush(stdin);
                                          getchar();
                                          destroy_bitmap(credito);
                                               exit(0);
                                               }
                                 if(pos ==0 || pos ==1){
                                            int x = 255;
                                             int y = 220;                                                                                  
                                             compactaArquivo();
                                             verDicionario();
                                             rest(400);                    
                                             limpa_menu();
                                             BITMAP *fundo = load_bitmap("cursor.bmp",NULL);
                                             draw_sprite(screen,fundo,255,240);
                                                fflush(stdin);
                                                rest(80);
                                                c = getchar();
                                                                }
                                 if(pos == 2){
                                    int x = 255;
                                      int y = 220;                                                                                  
                                                       descompactaArquivo();
                                                            rest(800);                    
                                                                pos = 5;
                                                                limpa_menu();
                                                                       BITMAP *fundo = load_bitmap("cursor.bmp",NULL);
                                                                        draw_sprite(screen,fundo,255,240);
                                                                }                               
                                                                
                                 
                                 }
        if(c == key[KEY_RIGHT])
                               {
                                limpa_menu();
                                  BITMAP *fundo = load_bitmap("cursor.bmp",NULL);
                                   draw_sprite(screen,fundo,450,240);
                                   rest(80);
                                   pos = 2 ;
                                }
                                
        if(c == key[KEY_LEFT])
                             {
                                limpa_menu();
                                  BITMAP *fundo = load_bitmap("cursor.bmp",NULL);
                                  draw_sprite(screen,fundo,255,240);
                                   rest(80);
                                   pos = 1;
                             }  
         if(c == key[KEY_DOWN])
                             {
                                limpa_menu();
                                  BITMAP *fundo = load_bitmap("cursor.bmp",NULL);
                                  draw_sprite(screen,fundo,460,320);
                                   rest(80);
                                   pos = 3;
                             }       
         if(c == key[KEY_UP])
                               {
                                limpa_menu();
                                  BITMAP *fundo = load_bitmap("cursor.bmp",NULL);
                                   draw_sprite(screen,fundo,450,240);
                                   rest(80);
                                   pos = 2;
                                }                                                    
    
         }	while(pos != 4);
         
}
                
	
