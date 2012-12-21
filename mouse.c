#include "mouse.h"

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
			mouse->under[bitmap_offset] = doubleBuffer[(word)(screen_offset+mx+x)];
			/* check for screen boundries */
			if (mx+x < SCREEN_WIDTH  && mx+x >= 0 && my+y < SCREEN_HEIGHT && my+y >= 0) {
				data = mouse->bmp->data[bitmap_offset];
				if (data)
					doubleBuffer[(word)(screen_offset+mx+x)] = data;
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
				doubleBuffer[(word)(screen_offset+mx+x)] = mouse->under[bitmap_offset];
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
				doubleBuffer[screen_offset+button->x+x] = data;
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
	fillRect(P1,P2,8);
	//Preview Color Button
	P1 = Point(2,7);
	P2 = Point(26,31);
	drawRectp(P1,P2,15);
	
	
	//Draw Color Picker Button
	P1 = Point(12,2);
	P2 = Point(28,18);
	P3 = Point(13,3);
	P4 = Point(27,17);
	for (counter=0;counter<8;counter++) {
		P1.x+=20;	P3.x+=20;
		P2.x+=20;	P4.x+=20;
		drawRectp(P1,P2,15);
		fillRect(P3,P4,counter);
	}
	P1 = Point(12,20);
	P2 = Point(28,36);
	P3 = Point(13,21);
	P4 = Point(27,35);
	for (counter=8;counter<16;counter++) {
		P1.x+=20;	P3.x+=20;
		P2.x+=20;	P4.x+=20;
		drawRectp(P1,P2,15);
		fillRect(P3,P4,counter);
	}
	
	//drawLine Button
	P1 = Point(196,7);
	P2 = Point(220,31);
	P3 = Point(200,11);
	P4 = Point(216,27);
	fillRect(P1,P2,GRAY);
	drawRectp(P1,P2,WHITE);
	drawLinep(P3,P4,BROWN);
	
	//drawRectp Button
	P1.x += 28;
	P2.x += 28;
	P3 = Point(228,12);
	P4 = Point(244,26);
	fillRect(P1,P2,GRAY);
	drawRectp(P1,P2,WHITE);
	drawRectp(P3,P4,BROWN);
	
	//drawCircle Button
	P1.x += 28;
	P2.x += 28;
	P3 = Point(264,19);
	fillRect(P1,P2,GRAY);
	drawRectp(P1,P2,WHITE);
	drawCirclep(P3,8,BROWN);
	
	//fill Button
	P1.x += 28;
	P2.x += 28;
	fillRect(P1,P2,GRAY);
	drawRectp(P1,P2,WHITE);
	P1 = Point(286,15);
	P2 = Point(298,28);
	P3 = Point(292,15);
	drawCirclep(P3,7,BROWN);
	fillRect(P1,P2,DARK_GRAY);
	drawRectp(P1,P2,WHITE);
		
	//exit Button
	P1 = Point(310,0);
	P2 = Point(319,9);
	fillRect(P1,P2,RED);
	P1 = Point(312,2);
	P2 = Point(317,7);
	P3 = Point(317,2);
	P4 = Point(312,7);
	drawLinep(P1,P2,WHITE);
	drawLinep(P3,P4,WHITE);
		
	//help Button
	P1 = Point(314,17);
	fillCircle(P1,6,DARK_BLUE);
	P1 = Point(314,13);
	P2 = Point(315,14);
	P3 = Point(314,16);
	P4 = Point(315,21);
	fillRect(P1,P2,WHITE);
	fillRect(P3,P4,WHITE);
	
	//Scrollbar
	P1 = Point(310,41);
	P2 = Point(319,199);
	P3 = Point(319,51);
	P4 = Point(310,189);
	fillRect(P1,P2,GRAY);
	fillRect(P1,P3,DARK_GRAY);
	fillRect(P4,P2,DARK_GRAY);
	P1 = Point(314,42);
	P2 = Point(317,50);
	P3 = Point(311,50);
	P4 = Point(315,45);
	drawLinep(P1,P2,BLACK);
	drawLinep(P2,P3,BLACK);
	drawLinep(P3,P1,BLACK);
	P1 = Point(314,198);
	P2 = Point(317,190);
	P3 = Point(311,190);
	P4 = Point(315,195);
	drawLinep(P1,P2,BLACK);
	drawLinep(P2,P3,BLACK);
	drawLinep(P3,P1,BLACK);		
}

void drawLineButton(int state) {
	
	point P1 = {196,7};
	point P2 = {220,31};
	point P3 = {200,11};
	point P4 = {216,27};
	if (state==STATE_NORM) {
		fillRect(P1,P2,GRAY);
		drawRect(P1.x, P1.y, P2.x, P2.y, WHITE);
		drawLine(P3.x, P3.y ,P4.x ,P4.y, BROWN);
	} else if (state==STATE_PRESSED) {
		fillRect(P1,P2,CYAN);		
		drawRect(P1.x, P1.y, P2.x, P2.y, WHITE);
		drawLine(P3.x, P3.y ,P4.x ,P4.y, BROWN);
	}
	
}

void drawRectButton(int state) {	
	point P1 = {224,7};
	point P2 = {248,31};
	point P3 = {228,12};
	point P4 = {244,26};
	if (state==STATE_NORM) {
		fillRect(P1,P2,GRAY);		
		drawRect(P1.x, P1.y, P2.x, P2.y, WHITE);
		drawRect(P3.x, P3.y ,P4.x ,P4.y, BROWN);
	} else if (state==STATE_PRESSED) {
		fillRect(P1,P2,CYAN);
		drawRect(P1.x, P1.y, P2.x, P2.y, WHITE);
		drawRect(P3.x, P3.y ,P4.x ,P4.y, BROWN);
	}	
}

void drawCircleButton(int state) {	
	point P1 = {252,7};
	point P2 = {276,31};
	point P3 = {264,19};
	if (state==STATE_NORM) {
		fillRect(P1,P2,GRAY);
		drawRect(P1.x, P1.y, P2.x, P2.y,WHITE);
		drawCircle(P3.x, P3.y, 8, BROWN);		
	} else if (state==STATE_PRESSED) {
		fillRect(P1,P2,CYAN);
		drawRect(P1.x, P1.y, P2.x, P2.y,WHITE);
		drawCircle(P3.x, P3.y, 8, BROWN);
	}	
}

void drawFillButton(int state) {
	
	point P1 = {280,7},
		  P2 = {304,31},
		  P3;
	if (state==STATE_NORM) {
		fillRect(P1,P2,GRAY);
		drawRect(P1.x, P1.y, P2.x, P2.y, WHITE);
	} else if (state==STATE_PRESSED) {
		fillRect(P1,P2,CYAN);
		drawRect(P1.x, P1.y, P2.x, P2.y, WHITE);
	}
	P1 = Point(286,15);
	P2 = Point(298,28);
	P3 = Point(292,15);
	drawCircle(P3.x, P3.y, 7, BROWN);
	fillRect(P1,P2,DARK_GRAY);
	drawRect(P1.x, P1.y, P2.x, P2.y, WHITE);
	
}

void MakeEmpty(ListObject* LO){
	*LO = NULL;
}


Object Line(point P1, point P2, int color){
	Object O;
	O.type = LINE;
	O.size = 2;
	O.ArrayP = (point*) malloc(O.size*sizeof(point));
	O.ArrayP[0] = P1;
	O.ArrayP[1] = P2;
	O.color = color;
	
	return O;
}
Object Circle(point P, int radius, int color){
	Object O;
	O.type = CIRCLE;
	O.size = 1;
	O.ArrayP = (point*) malloc(O.size*sizeof(point));
	O.ArrayP[0] = P;
	O.color = color;
	
	return O;
}
Object Rect(point P1, point P2, int color){
	Object O;
	O.type = RECT;
	O.size = 2;
	O.ArrayP = (point*) malloc(O.size*sizeof(point));
	O.ArrayP[0] = P1;
	O.ArrayP[1] = P2;
	O.color = color;
	
	return O;
}

void drawObject(Object O){
	switch(O.type){
		case LINE:
			drawLinep(O.ArrayP[0], O.ArrayP[1], O.color);
			break;
		case RECT:
			drawRectp(O.ArrayP[0], O.ArrayP[1], O.color);
			break;
		case CIRCLE:
			drawCirclep(O.ArrayP[0], O.radius, O.color);
			break;
	}
}

void Add(ListObject *LO, Object O){
	ElmtList *P, *Temp;
	P = (ElmtList*) malloc(sizeof(ElmtList)); //Alokasi element list
	if(P != NULL){
		P->Value = O;
		P->Next = NULL;
		if(*LO == NULL){ //Kasus kosong
			*LO = P;
		}else{ //Kasus tidak kosong
			Temp = *LO; //First Elmt
			while(Temp->Next != NULL){
				Temp = Temp->Next; //NextElmt
			}
			Temp->Next = P;
		}
	}
}