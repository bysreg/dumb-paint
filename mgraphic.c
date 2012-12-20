#include "graphic.h"

int main() {
	char c = 'a';
	
	initDoubleBuffer();
	
	getch();
	//***MULAI GAMBAR*****
	while(1) {
		set_mode(VGA_256_COLOR_MODE);    				
				
			
		//rasterFill(bgColor, planeLineColor, planeInfo[planeInfoI].pos.x, planeInfo[planeInfoI].pos.x + 20, planeInfo[planeInfoI].pos.y, planeInfo[planeInfoI].pos.y + 10);
		//rasterFill(planeFillColor, planeLineColor, 0, 0, 100, 100);
		
		drawRect(0,0, 100,100, 0, 1);
		
		while ((inp(INPUT_STATUS_1) & VRETRACE));
		while (!(inp(INPUT_STATUS_1) & VRETRACE));
		memcpy(VGA,doubleBuffer,320*200);
		
		c = getch();		
		if(c=='c') 
			break;		
	}	
	
	//***SELESAI GAMBAR****
	set_mode(TEXT_MODE);    		
		
	getch();
	
	return 0;
}