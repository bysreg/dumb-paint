#ifndef LAYOUT_H
#define LAYOUT_H

#include "graphic.h"

#define PALETTE_INDEX       0x03c8
#define PALETTE_DATA        0x03c9
#define INPUT_STATUS        0x03da
#define VRETRACE            0x08

#define SCREEN_WIDTH        320       /* width in pixels of mode 0x13 */
#define SCREEN_HEIGHT       200       /* height in pixels of mode 0x13 */
#define NUM_COLORS          256       /* number of colors in mode 0x13 */

#define MOUSE_INT           0x33
#define MOUSE_RESET         0x00
#define MOUSE_GETPRESS      0x05
#define MOUSE_GETRELEASE    0x06
#define MOUSE_GETMOTION     0x0B
#define LEFT_BUTTON         0x00
#define RIGHT_BUTTON        0x01
#define MIDDLE_BUTTON       0x02


#define MOUSE_WIDTH         24
#define MOUSE_HEIGHT        24
#define MOUSE_SIZE          (MOUSE_HEIGHT*MOUSE_WIDTH)

#define BUTTON_WIDTH        48
#define BUTTON_HEIGHT       24
#define BUTTON_SIZE         (BUTTON_HEIGHT*BUTTON_WIDTH)
#define BUTTON_BITMAPS      3
#define STATE_NORM          0
#define STATE_ACTIVE        1
#define STATE_PRESSED       2
#define STATE_WAITING       3

#define BLACK				0
#define DARK_BLUE			1
#define DARK_GREEN			2
#define DARK_CYAN			3
#define RED					4
#define PURPLE				5
#define BROWN				6
#define GRAY				7
#define DARK_GRAY			8
#define BLUE				9
#define GREEN				10
#define CYAN				11
#define VERMILLION			12
#define VIOLET				13
#define YELLOW				14
#define WHITE				15

#define NUM_BUTTONS         2
#define NUM_MOUSEBITMAPS    9

typedef unsigned long  dword;
typedef short          sword;

typedef struct {
	word width;
	word height;
	byte palette[256*3];
	byte *data;
} BITMAP;

typedef struct tagMOUSEBITMAP MOUSEBITMAP; /* the structure for animated mouse pointers. */
struct tagMOUSEBITMAP {
	int hot_x;
	int hot_y;
	byte data[MOUSE_SIZE];
	MOUSEBITMAP *next;	/* points to the next mouse bitmap, if any */
};

typedef struct {        	/* the structure for a mouse. */
	byte on;
	byte button1;
	byte button2;
	byte button3;
	int num_buttons;
	sword x;
	sword y;
	byte under[MOUSE_SIZE];
	MOUSEBITMAP *bmp;
} MOUSE;

typedef struct {            /* the structure for a button. */
  int x;
  int y;
  int state;
  byte bitmap[BUTTON_BITMAPS][BUTTON_SIZE];
} BUTTON;

void fskip(FILE *fp, int num_bytes);
void load_bmp (char *file,BITMAP *b);
void set_palette(byte *palette);
void wait_for_retrace(void);
void get_mouse_motion(sword *dx, sword *dy);
sword init_mouse(MOUSE *mouse);
sword get_mouse_press(sword button);
sword get_mouse_release(sword button);
void show_mouse(MOUSE *mouse);
void hide_mouse(MOUSE *mouse);
void makeLayout();
void drawLineButton(int state);
void drawRectButton(int state);

#endif