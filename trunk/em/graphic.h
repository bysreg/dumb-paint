/* grafik.h */

#ifndef	GRAPHIC_H
#define GRAPHIC_H

#include <stdio.h>
#include <stdlib.h>
#include <dos.h>
#include <math.h>

#define VIDEO_INT           0x10      /* the BIOS video interrupt. */
#define WRITE_DOT           0x0C      /* BIOS func to plot a pixel. */
#define SET_MODE            0x00      /* BIOS func to set the video mode. */
#define VGA_256_COLOR_MODE  0x13      /* use to set 256-color mode. */
#define TEXT_MODE           0x03      /* use to set 80x25 text mode. */
#define INPUT_STATUS_1      0x03da
#define VRETRACE            0x08

#define SCREEN_WIDTH        320       /* width in pixels of mode 0x13 */
#define SCREEN_HEIGHT       200       /* height in pixels of mode 0x13 */
#define NUM_COLORS          256       /* number of colors in mode 0x13 */

typedef unsigned char           byte;
typedef unsigned short          word;
typedef struct {int x; int y;} 	point;
typedef long                    fixed16_16;

extern byte *VGA;        /* this points to video memory. */
extern word *my_clock;    /* this points to the 18.2hz system clock. */
extern byte *DoubleBuffer; /*memori penyimpanan sementara untuk menggambar ke VGA*/
extern fixed16_16 SIN_ACOS[1024];		  
		 
point Point(int x, int y);
void set_mode(byte mode);
void draw_pixel(byte* Buffer, int x,int y, byte color);
void drawPixel(byte* Buffer, int memno, byte color);
byte getColor(byte* Buffer, int x, int y);
void makeBuffer();
void drawBuffer();
void drawLine(byte* Buffer, point P0, point P1, byte color);
void drawRect(byte* Buffer, point P0, point P1, byte color);
void fillRect(byte* Buffer, point P0, point P1, byte color);
void circleReference();
void drawCircle(byte* Buffer, point P, int radius, byte color);
void fillCircle(byte* Buffer, point P, int radius, byte color);

#endif