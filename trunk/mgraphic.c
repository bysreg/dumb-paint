#include "graphic.h"

int main() {
	
	word x,y;	
	BITMAP bmp;	
	vec2 polypoints[3];	
	int i, j = 0;	
	point p1, p2, p3, p4;
	point points[200];
	
	initDoubleBuffer();
	loadFontDb();
	
	load_bmp("balls.bmp",&bmp);
		
	set_mode(VGA_256_COLOR_MODE);      //  siap - siap gambar		
	
	// TEST 1. gambar lingkaran
	memset(canvas,0,SCREEN_SIZE);
	drawCircle(SCREEN_WIDTH / 2,SCREEN_HEIGHT / 2, 25, 2);
	show_buffer(); // show the buffer			
	getch();
	
	// TEST 2. gambar garis
	memset(canvas,0,SCREEN_SIZE);
	for (i=0;i<SCREEN_HEIGHT;i++) {
		for (j=0;j<SCREEN_WIDTH;j++) {
			drawPixel(j, i, 15);
		}
	}
	drawLine(1,1,100, 100, 25);
	drawLine(2,2,101, 101, 25);
	show_buffer(); // show the buffer			
	getch();
	
	// TEST 3. gambar polygon
	memset(canvas,0,SCREEN_SIZE);	
	polypoints[0].x = 0;
	polypoints[0].y = 0;
	polypoints[1].x = 20;
	polypoints[1].y = 0;
	polypoints[2].x = 10;
	polypoints[2].y = 10;	
	drawPolygon(polypoints, 3, 25);			
	show_buffer(); // show the buffer			
	getch();
		
	// TEST 4. fill
	memset(canvas,0,SCREEN_SIZE);			
	drawCircle(30,30, 7, 14);	
	floodFill(30, 30, getColor(30, 30), 14);	
	show_buffer(); // show the buffer			
	getch();
	
	// TEST 5. gambar font
	memset(canvas,0,SCREEN_SIZE);			
	drawString("ababa", 20, 20, 30, 14);
	show_buffer(); // show the buffer			
	getch();	
	
	// TEST 6. zoom in zoom out	
	zoom(1.5);
	show_buffer(); // show the buffer			
	getch();
	
	// TEST 7. kurva bezier
	zoom(1);
	memset(canvas,0,SCREEN_SIZE);	
	p1.x = 0;
	p1.y = 0;
	p2.x = 100;
	p2.y = 100;
	p3.x = 0;
	p3.y = SCREEN_HEIGHT;
	p4.x = SCREEN_WIDTH / 2;
	p4.y = SCREEN_HEIGHT / 2;	
	createBezier4(points, p1, p2, p3, p4, 0.01);		
	drawPolygonp(points, 100, 14);
	show_buffer(); // show the buffer			
	getch();
	
	// TEST 6. gambar bitmap	
	set_palette(bmp.palette);
	memset(canvas,0,SCREEN_SIZE);	
	drawBitmap(bmp, 0, 0);
	show_buffer(); // show the buffer			
	getch();
		
	set_mode(TEXT_MODE);			
		
	free(doubleBuffer); // free up memory used
	free(canvas);
	free(dict); // jangan lupa nge-free heap
	set_mode(TEXT_MODE);	
	
	return 0;
}