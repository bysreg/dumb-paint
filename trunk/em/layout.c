#include <stdio.h>
#include <stdlib.h>
#include <dos.h>
#include "layout.h"
#include "fill.h"

void fskip(FILE *fp, int num_bytes) {
	int i;
	for (i=0; i<num_bytes; i++) {
		fgetc(fp);
	}
}

void load_bmp(char *file, BITMAP *b) {
	FILE *fp;
	long index;
	word num_colors;
	int x;

	/* open the file */
	if ((fp = fopen(file,"rb")) == NULL) {
		printf("Error opening file %s.\n",file);
		exit(1);
	}

	/* check to see if it is a valid bitmap file */
	if (fgetc(fp)!='B' || fgetc(fp)!='M') {
		fclose(fp);
		printf("%s is not a bitmap file.\n",file);
		exit(1);
	}

	/* read in the width and height of the image, and the
	 number of colors used; ignore the rest */
	fskip(fp,16);
	fread(&b->width, sizeof(word), 1, fp);
	fskip(fp,2);
	fread(&b->height,sizeof(word), 1, fp);
	fskip(fp,22);
	fread(&num_colors,sizeof(word), 1, fp);
	fskip(fp,6);

	/* assume we are working with an 8-bit file */
	if (num_colors==0) num_colors=256;

	/* try to allocate memory */
	if ((b->data = (byte *) malloc((word)(b->width*b->height))) == NULL) {
		fclose(fp);
		printf("Error allocating memory for file %s.\n",file);
		exit(1);
	}

	/* read the palette information */
	for(index=0;index<num_colors;index++) {
		b->palette[(int)(index*3+2)] = fgetc(fp) >> 2;
		b->palette[(int)(index*3+1)] = fgetc(fp) >> 2;
		b->palette[(int)(index*3+0)] = fgetc(fp) >> 2;
		x=fgetc(fp);
	}

	/* read the bitmap */
	for(index=(b->height-1)*b->width;index>=0;index-=b->width) {
		for(x=0;x<b->width;x++) {
			b->data[(word)(index+x)]=(byte)fgetc(fp);
		}
	}
	fclose(fp);
}

void set_palette(byte *palette) {
	int i;

	outp(PALETTE_INDEX,0);
	for(i=0;i<256*3;i++) {
		outp(PALETTE_DATA,palette[i]);    /* write the data */
	}
}

void wait_for_retrace(void) {
    while  ((inp(INPUT_STATUS) & VRETRACE));
    while (!(inp(INPUT_STATUS) & VRETRACE));
}

void get_mouse_motion(sword *dx, sword *dy) {
	union REGS regs;

	regs.x.ax = MOUSE_GETMOTION;
	int86(MOUSE_INT, &regs, &regs);
	*dx=regs.x.cx;
	*dy=regs.x.dx;
}

sword init_mouse(MOUSE *mouse) {
	sword dx,dy;
	union REGS regs;

	regs.x.ax = MOUSE_RESET;
	int86(MOUSE_INT, &regs, &regs);
	mouse->on=regs.x.ax;
	mouse->num_buttons=regs.x.bx;
	mouse->button1=0;
	mouse->button2=0;
	mouse->button3=0;
	mouse->x=SCREEN_WIDTH/2;
	mouse->y=SCREEN_HEIGHT/2;
	get_mouse_motion(&dx,&dy);
	return mouse->on;
}

sword get_mouse_press(sword button) {
	union REGS regs;

	regs.x.ax = MOUSE_GETPRESS;
	regs.x.bx = button;
	int86(MOUSE_INT, &regs, &regs);
	return regs.x.bx;
}

sword get_mouse_release(sword button) {
	union REGS regs;

	regs.x.ax = MOUSE_GETRELEASE;
	regs.x.bx = button;
	int86(MOUSE_INT, &regs, &regs);
	return regs.x.bx;
}

void show_mouse(MOUSE *mouse) {
	int x, y;
	int mx = mouse->x - mouse->bmp->hot_x;
	int my = mouse->y - mouse->bmp->hot_y;
	long screen_offset = (my<<8)+(my<<6);
	word bitmap_offset = 0;
	byte data;

	for(y=0;y<MOUSE_HEIGHT;y++) {
		for(x=0;x<MOUSE_WIDTH;x++,bitmap_offset++) {
			mouse->under[bitmap_offset] = DoubleBuffer[(word)(screen_offset+mx+x)];
			/* check for screen boundries */
			if (mx+x < SCREEN_WIDTH  && mx+x >= 0 && my+y < SCREEN_HEIGHT && my+y >= 0) {
				data = mouse->bmp->data[bitmap_offset];
				if (data)
					DoubleBuffer[(word)(screen_offset+mx+x)] = data;
			}
		}
		screen_offset+=SCREEN_WIDTH;
	}
}

void hide_mouse(MOUSE *mouse) {
	int x, y;
	int mx = mouse->x - mouse->bmp->hot_x;
	int my = mouse->y - mouse->bmp->hot_y;
	long screen_offset = (my<<8)+(my<<6);
	word bitmap_offset = 0;

	for(y=0;y<MOUSE_HEIGHT;y++) {
		for(x=0;x<MOUSE_WIDTH;x++,bitmap_offset++) {
			/* check for screen boundries */
			if (mx+x < SCREEN_WIDTH  && mx+x >= 0 && my+y < SCREEN_HEIGHT && my+y >= 0) {
				DoubleBuffer[(word)(screen_offset+mx+x)] = mouse->under[bitmap_offset];
			}
		}
		screen_offset+=SCREEN_WIDTH;
	}
}

void draw_button(BUTTON *button) {
	int x, y;
	word screen_offset = (button->y<<8)+(button->y<<6);
	word bitmap_offset = 0;
	byte data;

	for(y=0;y<BUTTON_HEIGHT;y++) {
		for(x=0;x<BUTTON_WIDTH;x++,bitmap_offset++) {
			data = button->bitmap[button->state%BUTTON_BITMAPS][bitmap_offset];
			if (data)
				DoubleBuffer[screen_offset+button->x+x] = data;
		}
		screen_offset+=SCREEN_WIDTH;
	}
}

void makeLayout() {
	int		counter;
	point	P1,
			P2,
			P3,
			P4;
	P1 = Point(0,0);
	P2 = Point(319,40);
	fillRect(DoubleBuffer,P1,P2,8);
	//Preview Color Button
	P1 = Point(2,7);
	P2 = Point(26,31);
	drawRect(DoubleBuffer,P1,P2,15);
	
	//Draw Color Picker Button
	P1 = Point(12,2);
	P2 = Point(28,18);
	P3 = Point(13,3);
	P4 = Point(27,17);
	for (counter=0;counter<8;counter++) {
		P1.x+=20;	P3.x+=20;
		P2.x+=20;	P4.x+=20;
		drawRect(DoubleBuffer,P1,P2,15);
		fillRect(DoubleBuffer,P3,P4,counter);
	}
	P1 = Point(12,20);
	P2 = Point(28,36);
	P3 = Point(13,21);
	P4 = Point(27,35);
	for (counter=8;counter<16;counter++) {
		P1.x+=20;	P3.x+=20;
		P2.x+=20;	P4.x+=20;
		drawRect(DoubleBuffer,P1,P2,15);
		fillRect(DoubleBuffer,P3,P4,counter);
	}
	
	//drawLine Button
	P1 = Point(196,7);
	P2 = Point(220,31);
	P3 = Point(200,11);
	P4 = Point(216,27);
	fillRect(DoubleBuffer,P1,P2,GRAY);
	drawRect(DoubleBuffer,P1,P2,WHITE);
	drawLine(DoubleBuffer,P3,P4,BROWN);
	
	//drawRect Button
	P1.x += 28;
	P2.x += 28;
	P3 = Point(228,12);
	P4 = Point(244,26);
	fillRect(DoubleBuffer,P1,P2,GRAY);
	drawRect(DoubleBuffer,P1,P2,WHITE);
	drawRect(DoubleBuffer,P3,P4,BROWN);
	
	//drawCircle Button
	P1.x += 28;
	P2.x += 28;
	P3 = Point(264,19);
	fillRect(DoubleBuffer,P1,P2,GRAY);
	drawRect(DoubleBuffer,P1,P2,WHITE);
	drawCircle(DoubleBuffer,P3,8,BROWN);
	
	//fill Button
	P1.x += 28;
	P2.x += 28;
	fillRect(DoubleBuffer,P1,P2,GRAY);
	drawRect(DoubleBuffer,P1,P2,WHITE);
	P1 = Point(286,15);
	P2 = Point(298,28);
	P3 = Point(292,15);
	drawCircle(DoubleBuffer,P3,7,BROWN);
	fillRect(DoubleBuffer,P1,P2,DARK_GRAY);
	drawRect(DoubleBuffer,P1,P2,WHITE);
		
	//exit Button
	P1 = Point(310,0);
	P2 = Point(319,9);
	fillRect(DoubleBuffer,P1,P2,RED);
	P1 = Point(312,2);
	P2 = Point(317,7);
	P3 = Point(317,2);
	P4 = Point(312,7);
	drawLine(DoubleBuffer,P1,P2,WHITE);
	drawLine(DoubleBuffer,P3,P4,WHITE);
		
	//help Button
	P1 = Point(314,17);
	fillCircle(DoubleBuffer,P1,6,DARK_BLUE);
	P1 = Point(314,13);
	P2 = Point(315,14);
	P3 = Point(314,16);
	P4 = Point(315,21);
	fillRect(DoubleBuffer,P1,P2,WHITE);
	fillRect(DoubleBuffer,P3,P4,WHITE);
	
	//Scrollbar
	P1 = Point(310,41);
	P2 = Point(319,199);
	P3 = Point(319,51);
	P4 = Point(310,189);
	fillRect(DoubleBuffer,P1,P2,GRAY);
	fillRect(DoubleBuffer,P1,P3,DARK_GRAY);
	fillRect(DoubleBuffer,P4,P2,DARK_GRAY);
	P1 = Point(314,42);
	P2 = Point(317,50);
	P3 = Point(311,50);
	P4 = Point(315,45);
	drawLine(DoubleBuffer,P1,P2,BLACK);
	drawLine(DoubleBuffer,P2,P3,BLACK);
	drawLine(DoubleBuffer,P3,P1,BLACK);
	P1 = Point(314,198);
	P2 = Point(317,190);
	P3 = Point(311,190);
	P4 = Point(315,195);
	drawLine(DoubleBuffer,P1,P2,BLACK);
	drawLine(DoubleBuffer,P2,P3,BLACK);
	drawLine(DoubleBuffer,P3,P1,BLACK);
	//Final State
	drawBuffer();
}

void drawLineButton(int state) {
	point P1 = {196,7};
	point P2 = {220,31};
	point P3 = {200,11};
	point P4 = {216,27};
	if (state==STATE_NORM) {
		fillRect(DoubleBuffer,P1,P2,GRAY);
		drawRect(DoubleBuffer,P1,P2,WHITE);
		drawLine(DoubleBuffer,P3,P4,BROWN);
	} else if (state==STATE_PRESSED) {
		fillRect(DoubleBuffer,P1,P2,CYAN);
		drawRect(DoubleBuffer,P1,P2,WHITE);
		drawLine(DoubleBuffer,P3,P4,BROWN);
	}
}

void drawRectButton(int state) {
	point P1 = {224,7};
	point P2 = {248,31};
	point P3 = {228,12};
	point P4 = {244,26};
	if (state==STATE_NORM) {
		fillRect(DoubleBuffer,P1,P2,GRAY);
		drawRect(DoubleBuffer,P1,P2,WHITE);
		drawRect(DoubleBuffer,P3,P4,BROWN);
	} else if (state==STATE_PRESSED) {
		fillRect(DoubleBuffer,P1,P2,CYAN);
		drawRect(DoubleBuffer,P1,P2,WHITE);
		drawRect(DoubleBuffer,P3,P4,BROWN);
	}
}

void drawCircleButton(int state) {
	point P1 = {252,7};
	point P2 = {276,31};
	point P3 = {264,19};
	if (state==STATE_NORM) {
		fillRect(DoubleBuffer,P1,P2,GRAY);
		drawRect(DoubleBuffer,P1,P2,WHITE);
		drawCircle(DoubleBuffer,P3,8,BROWN);
	} else if (state==STATE_PRESSED) {
		fillRect(DoubleBuffer,P1,P2,CYAN);
		drawRect(DoubleBuffer,P1,P2,WHITE);
		drawCircle(DoubleBuffer,P3,8,BROWN);
	}
}

void drawFillButton(int state) {
	point P1 = {280,7},
		  P2 = {304,31},
		  P3;
	if (state==STATE_NORM) {
		fillRect(DoubleBuffer,P1,P2,GRAY);
		drawRect(DoubleBuffer,P1,P2,WHITE);
	} else if (state==STATE_PRESSED) {
		fillRect(DoubleBuffer,P1,P2,CYAN);
		drawRect(DoubleBuffer,P1,P2,WHITE);
	}
	P1 = Point(286,15);
	P2 = Point(298,28);
	P3 = Point(292,15);
	drawCircle(DoubleBuffer,P3,7,BROWN);
	fillRect(DoubleBuffer,P1,P2,DARK_GRAY);
	drawRect(DoubleBuffer,P1,P2,WHITE);
}