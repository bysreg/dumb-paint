#include "dpf.h"

int main() {		
	DPF dpf;	
	initDoubleBuffer();
	loadFontDb();	
	
	set_mode(VGA_256_COLOR_MODE);      //  siap - siap gambar		
	
	/* 1. TEST SAVE FILE
	memset(canvas,0,SCREEN_SIZE);	
	drawRect(0,0, 50, 50, 2);
	floodFill(2, 2, 0, 14);
	show_buffer(); // show the buffer			
	getch();
	save_dpf("hilman.dpf", canvas);
	*/	
		
	///* 2. TEST LOAD FILE
	set_mode(TEXT_MODE);      	
	open_dpf("hilman.dpf", &dpf);		
	memset(canvas,0,SCREEN_SIZE);
	draw_dpf(dpf);	
	set_mode(VGA_256_COLOR_MODE);
	show_buffer(); // show the buffer			
	getch();	
	//*/
		
	free(doubleBuffer); // free up memory used
	free(canvas);
	free(dict); // jangan lupa nge-free heap
	set_mode(TEXT_MODE);	
	return 0;
}
