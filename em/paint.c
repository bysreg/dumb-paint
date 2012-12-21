#include "layout.h"
#include "graphic.h"
#include "fill.h"
#include "Object.h"

void main() {
	typedef enum {line, rect, circle, fill} MODE;
	
	BITMAP 		bmp;
	MOUSE  		mouse;
	MOUSEBITMAP *mb[NUM_MOUSEBITMAPS],
				*mouse_norm,
				*mouse_wait,
				*mouse_new=NULL;
	BUTTON 		*button[NUM_BUTTONS];
	// Drawing mode handler
	int			color;
	point		P1={0,0},
				P2={0,0};
	int 		draw_line_mode=0,
				enter_draw_mode=0,
				draw_circle_mode=0,
				draw_rect_mode=0;
	word 		redraw;
	// Mouse coordinate interrupts handler
	sword 		dx=0,
				dy=0,
				new_x,
				new_y;
	// Mouse button interrupts handler			
	word 		press,
				release;
	// Command parsing
	char 		command[10],
				param[20];
	int 		i,
				j,
				radius,
				done=0,
				x,y;
	word 	last_time;
	int 	preview_x,
			preview_y;
			
	MODE 	CURRENT_MODE;
	ElmtList *Temp;
	ListObject LIST_OBJECT;
	Object OBJECT;
	
	/************************************************************************************/
	system("cls");
	// printf("##################################################\n");
	// printf("#                  16-BIT PAINT                  #\n");
	// printf("##################################################\n");
	// printf("to open existing file, type: open <filename>\n");
	// printf("to create a fresh file, type: new <filename>\n");
	// printf(">");
	// scanf("%s %s", &command, &param);
	// if (strcmp(command,"open")==0) {
		// printf("File open: %s\n", param);
		
	// } else if (strcmp(command,"new")==0) {
		// printf("Create new file: %s\n", param);
	// } else {
		// printf("Syntax error\n");
		// exit(1);
	// }
		
	//Inisialisasi Variable dasar
	CURRENT_MODE = line;
	color	= WHITE;
	MakeEmpty(&LIST_OBJECT);
	
	for (i=0; i<NUM_MOUSEBITMAPS; i++) {
		if ((mb[i] = (MOUSEBITMAP *) malloc(sizeof(MOUSEBITMAP))) == NULL) {
			printf("Error allocating memory for bitmap.\n");
			exit(1);
		}
	}

	for (i=0; i<NUM_BUTTONS; i++) {
		if ((button[i] = (BUTTON *) malloc(sizeof(BUTTON))) == NULL) {
			printf("Error allocating memory for bitmap.\n");
			exit(1);
		}
	}
	mouse_norm = mb[0];
	mouse_wait = mb[1];

	mouse.bmp = mouse_norm;

	button[0]->x     = 48;
	button[0]->y     = 152;
	button[0]->state = STATE_NORM;

	button[1]->x     = 224;
	button[1]->y     = 152;
	button[1]->state = STATE_NORM;
	
	if (!init_mouse(&mouse)) {
		printf("Mouse not found.\n");
		exit(1);
	}

	load_bmp("images.bmp",&bmp);
	circleReference();
	makeBuffer();
	set_mode(VGA_256_COLOR_MODE);       /* set the video mode. */
	// makeLayout();
	
	for(i=0;i<NUM_MOUSEBITMAPS;i++) {   /* copy mouse pointers */
		for(y=0;y<MOUSE_HEIGHT;y++) {
			for(x=0;x<MOUSE_WIDTH;x++) {
				mb[i]->data[x+y*MOUSE_WIDTH] = bmp.data[i*MOUSE_WIDTH+x+y*bmp.width];
				mb[i]->next = mb[i+1];
				mb[i]->hot_x = 12;
				mb[i]->hot_y = 12;
			}
		}
	}

	mb[0]->next  = mb[0];
	mb[8]->next  = mb[1];
	mb[0]->hot_x = 7;
	mb[0]->hot_y = 2;
	
	/* copy button bitmaps */
	for(i=0;i<NUM_BUTTONS;i++) {
		for(j=0;j<BUTTON_BITMAPS;j++) {
			for(y=0;y<BUTTON_HEIGHT;y++) {
				for(x=0;x<BUTTON_WIDTH;x++) {
					button[i]->bitmap[j][x+y*BUTTON_WIDTH] =
					bmp.data[i*(bmp.width>>1) + j*BUTTON_WIDTH + x +
					(BUTTON_HEIGHT+y)*bmp.width];
				}
			}
		}
	}

	free(bmp.data);                     /* free up memory used */
	
	new_x=mouse.x;
	new_y=mouse.y;
	redraw=0xFFFF;
	show_mouse(&mouse);
	last_time=*my_clock;
	
	//Inisialisasi Warna awal
	P1 = Point(3,8);
	P2 = Point(25,30);
	fillRect(DoubleBuffer,P1,P2,color);
	drawBuffer();

	while (!done) {                     // start main loop
		if (redraw) {                   // draw the mouse only as needed
			wait_for_retrace();
			hide_mouse(&mouse);
			if (redraw>1) {
				for(i=0;i<NUM_BUTTONS;i++) {
					if (redraw & (2<<i)) draw_button(button[i]);
				}
			}
			if (mouse_new!=NULL) mouse.bmp=mouse_new;
			mouse.x=new_x;
			mouse.y=new_y;
			show_mouse(&mouse);
			drawBuffer();
			last_time=*my_clock;
			redraw=0;
			mouse_new=NULL;
		}

		do {                              // check mouse status
			get_mouse_motion(&dx,&dy);
			press   = get_mouse_press(LEFT_BUTTON);
			release = get_mouse_release(LEFT_BUTTON);
		} while (dx==0 && dy==0 && press==0 && release==0 && *my_clock==last_time);

		if (press) mouse.button1=1;
		if (release) mouse.button1=0;

		if (dx || dy) {                   // calculate movement
			new_x = mouse.x+dx;
			new_y = mouse.y+dy;
			if (new_x<0)   new_x=0;
			if (new_y<0)   new_y=0;
			if (new_x>319) new_x=319;
			if (new_y>199) new_y=199;
			redraw=1;
		}
		
		/******************************* Colour Picking *******************************/
		if (press && new_x >= 32 && new_x <= 188 && new_y >= 2 && new_y <= 36) {
			if (new_x>=32 && new_x<=48 && new_y>=2 && new_y<=18)	color = BLACK;
			else if (new_x>=52 && new_x<=68 && new_y>=2 && new_y<=18)	color = DARK_BLUE;
			else if (new_x>=72 && new_x<=88 && new_y>=2 && new_y<=18)	color = DARK_GREEN;
			else if (new_x>=92 && new_x<=108 && new_y>=2 && new_y<=18)	color = DARK_CYAN;
			else if (new_x>=112 && new_x<=128 && new_y>=2 && new_y<=18)	color = RED;
			else if (new_x>=122 && new_x<=148 && new_y>=2 && new_y<=18)	color = PURPLE;
			else if (new_x>=152 && new_x<=168 && new_y>=2 && new_y<=18)	color = BROWN;
			else if (new_x>=172 && new_x<=188 && new_y>=2 && new_y<=18)	color = GRAY;
			else if (new_x>=32 && new_x<=48 && new_y>=20 && new_y<=36)	color = DARK_GRAY;
			else if (new_x>=52 && new_x<=68 && new_y>=20 && new_y<=36)	color = BLUE;
			else if (new_x>=72 && new_x<=88 && new_y>=20 && new_y<=36)	color = GREEN;
			else if (new_x>=92 && new_x<=108 && new_y>=20 && new_y<=36)	color = CYAN;
			else if (new_x>=112 && new_x<=128 && new_y>=20 && new_y<=36)	color = VERMILLION;
			else if (new_x>=122 && new_x<=148 && new_y>=20 && new_y<=36)	color = VIOLET;
			else if (new_x>=152 && new_x<=168 && new_y>=20 && new_y<=36)	color = YELLOW;
			else if (new_x>=172 && new_x<=188 && new_y>=20 && new_y<=36)	color = WHITE;
			P1 = Point(3,8);
			P2 = Point(25,30);
			fillRect(DoubleBuffer,P1,P2,color);
			drawBuffer();
		}
		/******************************* Mode Changing ********************************/
		if (press && new_x>=196 && new_x<=220 && new_y>=7 && new_y<31) {
			hide_mouse(&mouse);
			drawLineButton(STATE_PRESSED);
			drawRectButton(STATE_NORM);
			drawCircleButton(STATE_NORM);
			drawFillButton(STATE_NORM);
			show_mouse(&mouse);
			drawBuffer();
			CURRENT_MODE=line;
		} else if (press && new_x>=224 && new_x<=248 && new_y>=7 && new_y<31) {
			hide_mouse(&mouse);
			drawLineButton(STATE_NORM);
			drawRectButton(STATE_PRESSED);
			drawCircleButton(STATE_NORM);
			drawFillButton(STATE_NORM);
			show_mouse(&mouse);
			drawBuffer();
			CURRENT_MODE=rect;
		}
		/******************************* Drawing Method *******************************/
		if (CURRENT_MODE == line) { 
			if (press && !draw_line_mode && new_x >= 0 && new_x < 320 && new_y > 40 && new_y < 200) {
				P1.x=new_x; 	P1.y=new_y;
				draw_line_mode=1;
				drawBuffer();
			}		
			if (release && draw_line_mode && new_x >= 0 && new_x < 320 && new_y > 40 && new_y < 200) {
				P2.x=new_x; 	P2.y=new_y;
				hide_mouse(&mouse);
				OBJECT = Line(P1,P2,color);
				drawObject(OBJECT);
				Add(&LIST_OBJECT, OBJECT);
				show_mouse(&mouse);
				drawBuffer();
				draw_line_mode=0;
				enter_draw_mode=0;
			}
		}
		
		if (CURRENT_MODE == rect) { 
			if (press && !draw_line_mode && new_x >= 0 && new_x < 320 && new_y > 40 && new_y < 200) {
				P1.x=new_x; 	P1.y=new_y;
				draw_line_mode=1;
				drawBuffer();
			}		
			if (release && draw_line_mode && new_x >= 0 && new_x < 320 && new_y > 40 && new_y < 200) {
				P2.x=new_x; 	P2.y=new_y;
				hide_mouse(&mouse);
				OBJECT = Rect(P1,P2,color);
				drawObject(OBJECT);
				Add(&LIST_OBJECT, OBJECT);
				show_mouse(&mouse);
				drawBuffer();
				draw_line_mode=0;
				draw_rect_mode=0;
			}
		}
		
		if (CURRENT_MODE == circle) { 
			if (press && !draw_line_mode && new_x >= 0 && new_x < 320 && new_y > 40 && new_y < 200) {
				P1.x=new_x; 	P1.y=new_y;
				draw_line_mode=1;
				drawBuffer();
			}		
			if (release && draw_line_mode && new_x >= 0 && new_x < 320 && new_y > 40 && new_y < 200) {
				P2.x=new_x; 	P2.y=new_y;
				radius=sqrt(pow(P2.x-P1.x,2)+pow(P2.y-P1.y,2))/2;
				P1.x=(P1.x+P2.x)/2;
				P1.y=(P1.y+P2.y)/2;
				hide_mouse(&mouse);
				OBJECT = Circle(P1,radius,color);
				drawObject(OBJECT);
				Add(&LIST_OBJECT, OBJECT);
				show_mouse(&mouse);
				drawBuffer();
				draw_line_mode=0;
				draw_rect_mode=0;
			}
		}
		
		if(CURRENT_MODE == fill) {
		}
		/******************************************************************************/
		
		/******************************************************************************/
		for(i=0;i<NUM_BUTTONS;i++) {      // check button status
			if (new_x >= button[i]->x && new_x < button[i]->x+48 && new_y >= button[i]->y && new_y < button[i]->y+24) {
				if (release && button[i]->state==STATE_PRESSED) {
				//Handle a currently pressed button
					button[i]->state=STATE_ACTIVE;
					redraw|=(2<<i);
					if (i==0) {					// Button draw
						CURRENT_MODE = line;
						//draw_circle_mode=1;
					} else if (i==1) {			// Button exit
						done=1;
					}
				}
				else if (press) {
				//Handle not pressed button, if pressed, do this
					button[i]->state=STATE_PRESSED;
					redraw|=(2<<i);
				} else if (button[i]->state==STATE_NORM && mouse.button1==0) {
					button[i]->state=STATE_ACTIVE;
					redraw|=(2<<i);
				} else if (button[i]->state==STATE_WAITING) {
					if (mouse.button1) {
						button[i]->state=STATE_PRESSED;
					} else {
						button[i]->state=STATE_ACTIVE;
					}
					redraw|=(2<<i);
				}
			}
			else if (button[i]->state==STATE_ACTIVE) {
				button[i]->state=STATE_NORM;
				redraw|=(2<<i);
			}
			else if (button[i]->state==STATE_PRESSED && mouse.button1) {
				button[i]->state=STATE_WAITING;
				redraw|=(2<<i);
			}
			else if (button[i]->state==STATE_WAITING && release) {
				button[i]->state=STATE_NORM;
				redraw|=(2<<i);
			}
		}
	}                                   // end while loop

	for (i=0; i<NUM_MOUSEBITMAPS; i++) {/* free allocated memory */
		free(mb[i]);
	}

	for (i=0; i<NUM_BUTTONS; i++) {     /* free allocated memory */
		free(button[i]);
	}
	
	
	set_mode(TEXT_MODE);                /* set the video mode back to
                                         text mode. */
										 
	do{									/* free allocated memory*/	
		Temp = LIST_OBJECT;
		LIST_OBJECT = LIST_OBJECT->Next;
		free(Temp);
	}while(LIST_OBJECT != NULL);
	
	return 0;
}