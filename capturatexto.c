#include <allegro.h>
#include<stdio.h>
#include<stdlib.h>
#include"funcoes.h"

void CapturaTexto(){
     char vetorRetorno[100];
     int posVetor = 0;
     char caracter ;
     int x = 255;
     int y = 220;
      fflush(stdin);
    while((caracter = getch())!= key[KEY_ENTER]){
          
                                                                                                                  
          vetorRetorno[posVetor] = caracter;
          textprintf(screen, font, x,y, makecol(100,255,125),"%c", vetorRetorno[posVetor]);
          posVetor++;
         // void limpa_menuCaptura(void);
          x += 10;
          rest(200);
           fflush(stdin);
          }
                    return;
                    }
