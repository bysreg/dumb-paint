#include "graphic.h"

int main() {								
	word x,y;	
	BITMAP bmp;	

	/* allocate memory for double buffer and background image */
	initDoubleBuffer();

	/* load the images */
	load_bmp("balls.bmp",&bmp);
		
	set_mode(VGA_256_COLOR_MODE);      //  set the video mode.
	set_palette(bmp.palette);
	
	memset(doubleBuffer,0,SCREEN_SIZE);
	drawBitmap(bmp, 0, 0);
	show_buffer(doubleBuffer); // show the buffer		
	free(doubleBuffer);                /* free up memory used */		
		
	getch();
	set_mode(TEXT_MODE);
	return 0;
}