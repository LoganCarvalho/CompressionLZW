/********************************************************************
**
**
**  Algoritmo para compress�o de arquivos ao formato LZW
**
**  Essa classe est� mais relacionada com a biblioteca Allegro
*****************************************************************************/

#include <allegro.h>
#include<stdio.h>
#include<stdlib.h>
#include"funcoes.h"

void init();
void deinit();


int main() {
	init();

	while (!key[KEY_ESC]) {
 // Chamamos a fun��o inicial aqui
	Menu();
	}
	

	deinit();
	return 0;
}
END_OF_MAIN()

void init() {
	int depth, res;
	allegro_init();
	depth = desktop_color_depth();
	if (depth == 0) depth = 32;
	set_color_depth(depth);
	res = set_gfx_mode(GFX_AUTODETECT_WINDOWED, 680, 460, 0, 0);
	if (res != 0) {
		allegro_message(allegro_error);
		exit(-1);
	}

	install_timer();
	install_keyboard();
	install_mouse();
	/* add other initializations here */
}

void deinit() {
	clear_keybuf();
	/* add other deinitializations here */
}
