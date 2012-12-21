/*grafik.c*/

#include "graphic.h"

byte 		*VGA =(byte *)0xA0000000L;
word 		*my_clock=(word *)0x0000046C;
byte 		*DoubleBuffer;
fixed16_16 	SIN_ACOS[1024];

point Point(int x, int y) {
	point P;
	P.x=x;
	P.y=y;
	return P;
}

void set_mode(byte mode){
	union REGS regs;
	regs.h.ah = SET_MODE;
	regs.h.al = mode;
	int86(VIDEO_INT, &regs, &regs);
}

void draw_pixel(byte* Buffer, int x,int y, byte color) {
	Buffer[(y<<8)+(y<<6)+x]=color;
}

void drawPixel(byte* Buffer, int memno, byte color) {
	Buffer[memno]=color;
}

byte getColor(byte* Buffer, int x, int y) {
	return (Buffer[(y<<8)+(y<<6)+x]);
}

void makeBuffer() {	
	int x, y;
	DoubleBuffer = (byte *) malloc((SCREEN_WIDTH * SCREEN_HEIGHT));
	if(DoubleBuffer != NULL){
		for (y=0; y<SCREEN_HEIGHT; y++) 
			for (x=0; x<SCREEN_WIDTH; x++) {
				draw_pixel(DoubleBuffer,x,y,0);
		}
	}else{
		printf("Double Buffer allocation failed\n");
		exit(1);
	}
}

void drawBuffer() {
	while ( (inp(INPUT_STATUS_1) & VRETRACE));
	while (!(inp(INPUT_STATUS_1) & VRETRACE));
	memcpy(VGA,DoubleBuffer,320*200);
}

void drawLine(byte* Buffer, point P0, point P1, byte color) {
	int steep = abs(P1.y - P0.y) > abs(P1.x - P0.x);
	int temp, deltax, deltay, error, ystep, x, y;
	if (steep) {
		temp = P0.x;
		P0.x = P0.y;
		P0.y = temp;
		temp = P1.x;
		P1.x = P1.y;
		P1.y = temp;
	}
	if (P0.x > P1.x) {
		temp = P0.x;
		P0.x = P1.x;
		P1.x = temp;
		temp = P0.y;
		P0.y = P1.y;
		P1.y = temp;
	}
	deltax = P1.x - P0.x;
	deltay = abs(P1.y - P0.y);
	error = deltax / 2;
	y = P0.y;
	if (P0.y < P1.y) {ystep = 1;} else {ystep = -1;}
	for (x=P0.x; x<=P1.x; x++) {
		if (steep) draw_pixel(Buffer,y,x,color); 
		else draw_pixel(Buffer,x,y,color);
		error = error - deltay;
		if (error < 0) {
			y = y + ystep;
			error = error + deltax;
		}
	}
}

void drawRect(byte* Buffer, point P0, point P1, byte color){
	point P2; 
	point P3; 
	P2.x = P1.x; P2.y = P0.y;
	P3.x = P0.x; P3.y = P1.y;
	
	drawLine(Buffer,P0,P2,color);
	drawLine(Buffer,P2,P1,color);
	drawLine(Buffer,P1,P3,color);
	drawLine(Buffer,P3,P0,color);
}

void fillRect(byte* Buffer, point P0, point P1, byte color) {
	int x, y;
	for(y=P0.y; y<=P1.y; y++) {
		for(x=P0.x; x<=P1.x; x++) {
			draw_pixel(DoubleBuffer,x,y,color);
		}
	}
}

void circleReference() {
	int i;
	for(i=0;i<1024;i++) SIN_ACOS[i]=sin(acos((float)i/1024))*0x10000L;
}

void drawCircle(byte* Buffer, point P, int radius, byte color) {
	fixed16_16	n=0,
				invradius=(1/(float)radius)*0x10000L;
	int 		dx=0,
				dy=radius-1;
	while (dx<=dy) {  
		draw_pixel(DoubleBuffer, P.x-dx, P.y+dy, color);	/* octant 0 */
		draw_pixel(DoubleBuffer, P.x-dy, P.y+dx, color);	/* octant 1 */
		draw_pixel(DoubleBuffer, P.x-dy, P.y-dx, color);	/* octant 2 */
		draw_pixel(DoubleBuffer, P.x-dx, P.y-dy, color);	/* octant 3 */
		draw_pixel(DoubleBuffer, P.x+dx, P.y-dy, color);	/* octant 4 */
		draw_pixel(DoubleBuffer, P.x+dy, P.y-dx, color);	/* octant 5 */
		draw_pixel(DoubleBuffer, P.x+dy, P.y+dx, color);	/* octant 6 */
		draw_pixel(DoubleBuffer, P.x+dx, P.y+dy, color);	/* octant 7 */
		dx++;
		n+=invradius;
		dy = (int)((radius * SIN_ACOS[(int)(n>>6)]) >> 16);
	}
}

// void fillCircle(byte* Buffer, point P, int radius, byte color) {
	// fixed16_16 	n=0,
				// invradius=(1/(float)radius)*0x10000L;
	// int 		dx=0,
				// dy=radius-1,i;
	// word 		dxoffset,dyoffset,offset = (P.y<<8)+(P.y<<6)+P.x;

	// while (dx<=dy) {
		// dxoffset = (dx<<8) + (dx<<6);
		// dyoffset = (dy<<8) + (dy<<6);
		// for(i=dy;i>=dx;i--,dy--) {
			// draw_pixel(DoubleBuffer, P.x-dx, P.y+i,  color);	/* octant 0 */
			// draw_pixel(DoubleBuffer, P.x-dy, P.y+dx, color);	/* octant 1 */
			// draw_pixel(DoubleBuffer, P.x-dy, P.y-dx, color);	/* octant 2 */
			// draw_pixel(DoubleBuffer, P.x-dx, P.y-i,  color);	/* octant 3 */
			// draw_pixel(DoubleBuffer, P.x+dx, P.y-i,  color);	/* octant 4 */
			// draw_pixel(DoubleBuffer, P.x+dy, P.y-dx, color);	/* octant 5 */
			// draw_pixel(DoubleBuffer, P.x+dy, P.y+dx, color);	/* octant 6 */
			// draw_pixel(DoubleBuffer, P.x+dx, P.y+i,  color);	/* octant 7 */
		// }
		// dx++;
		// n+=invradius;
		// dy = (int)((radius * SIN_ACOS[(int)(n>>6)]) >> 16);
	// }
// }