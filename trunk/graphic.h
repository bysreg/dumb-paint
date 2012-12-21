#ifndef TRANSMAT_H
#define TRANSMAT_H

#include <stdio.h>
#include <stdlib.h>
#include <dos.h>
#include <mem.h>
#include "queue.h"


#define VIDEO_INT           0x10      /* the BIOS video interrupt. */
#define SET_MODE            0x00      /* BIOS func to set the video mode. */
#define VGA_256_COLOR_MODE  0x13      /* use to set 256-color mode. */
#define TEXT_MODE           0x03      /* use to set 80x25 text mode. */


#define SC_INDEX            0x03c4    /* VGA sequence controller */
#define SC_DATA             0x03c5
#define PALETTE_INDEX       0x03c8    /* VGA digital-to-analog converter */
#define PALETTE_DATA        0x03c9
#define GC_INDEX            0x03ce    /* VGA graphics controller */
#define GC_DATA             0x03cf
#define CRTC_INDEX          0x03d4    /* VGA CRT controller */
#define CRTC_DATA           0x03d5
#define INPUT_STATUS_1      0x03da

#define MAP_MASK            0x02      /* Sequence controller registers */
#define ALL_PLANES          0xff02
#define MEMORY_MODE         0x04

#define LATCHES_ON          0x0008    /* Graphics controller registers */
#define LATCHES_OFF         0xff08

#define HIGH_ADDRESS        0x0C      /* CRT controller registers */
#define LOW_ADDRESS         0x0D
#define UNDERLINE_LOCATION  0x14
#define MODE_CONTROL        0x17

#define DISPLAY_ENABLE      0x01      /* VGA input status bits */
#define VRETRACE            0x08

#define SCREEN_WIDTH        320       /* width in pixels of mode 0x13 */
#define SCREEN_HEIGHT       200       /* height in pixels of mode 0x13 */
#define SCREEN_SIZE         (word)(SCREEN_WIDTH*SCREEN_HEIGHT)
#define NUM_COLORS          256       /* number of colors in mode 0x13 */

#define VERTICAL_RETRACE              /* comment out this line for more
                                         accurate timing */
#define INPUT_STATUS_1      0x03da
#define INPUT_STATUS        0x03da
										 
#define PI 3.14

typedef unsigned char  byte;
typedef unsigned short word;
typedef long fixed16_16;

typedef struct {
	float x;
	float y;
} vec2;

typedef struct {
	int x;
	int y;
} point;

typedef struct {
	point p1;
	point p2;
} line;

typedef struct {
	line* lines;
	int lineCount;	
} letter;

typedef float Matrix3x3[3][3];

typedef struct tagBITMAP              /* the structure for a bitmap. */
{
  word width;
  word height;
  byte palette[256*3];
  byte *data;
} BITMAP;

typedef struct tagOBJECT              /* the structure for a moving object in 2d space; used for animation */
{
  int x,y;
  int dx,dy;
  byte width,height;
} OBJECT;

extern byte *VGA;        /* this points to video memory. */
extern word *my_clock;    /* this points to the 18.2hz system clock. */
extern byte *doubleBuffer;
extern byte *canvas; //data canvas sebenarnya
extern letter* dict; // kamus huruf
extern Matrix3x3 _transmat; // matrix transformasi
extern fixed16_16 SIN_ACOS[1024];		

point Point(int x, int y);
void initDoubleBuffer();
void loadFontDb();
void set_mode(byte mode);
void drawPixel(int x, int y, byte color);
byte getColor(int x, int y);
void set_palette(byte *palette);
void fskip(FILE *fp, int num_bytes);
void load_bmp(char *file,BITMAP *b);
void drawLine(int x0, int y0, int x1, int y1, byte color);
void drawPoints(point *points, int len, byte color);
void drawCircle(int xCenter, int yCenter, int radius, byte color); 
float convertAngleToRadian(float angle);
void matrix3x3SetIdentity(Matrix3x3 m); 
void matrix3x3PreMultiply(Matrix3x3 a, Matrix3x3 b);
void translate2(int tx, int ty, Matrix3x3 mat);
void scale2(float sx, float sy, vec2 refPt, Matrix3x3 mat);
void rotate2(float a, vec2 refPt, Matrix3x3 mat);
void transformPoints2(int npts, vec2 *pts, Matrix3x3 mat); 
void transformPoint2(int *x, int *y, Matrix3x3 mat);
vec2 calcTransformPoint2(vec2 p, Matrix3x3 mat);
void printMatrix3x3(Matrix3x3 m);
void floodFill(int x, int y, byte target_color, byte replacement_color);
void rasterFill(byte color, byte mark, int xAwal, int xAkhir, int yAwal, int yAkhir);
void copyPolygon(vec2 *p1, vec2 *p2, int n) ;
void drawPolygon(vec2 *points, int n, byte color);
void drawPolygonp(point *points, int n, byte color);
void drawRect(int x1, int y1, int x2, int y2, byte lineColor);
void drawBitmap(BITMAP bmp, int xscreen, int yscreen);
void drawLetter(int letterIndex, int x, int y, byte color);
void drawString(const char* s, int x, int y, int space, byte color);
void transformCanvas();
void zoom(float scale);
void createBezier4(point *points, point p1, point p2, point p3, point p4, float mu);
point bezier4(point p1, point p2, point p3, point p4, float mu);
void fillRect(point P0, point P1, byte color); 
void drawLinep(point P0, point P1, byte color);
void drawRectp(point P0, point P1, byte color);
void drawCirclep(point P, int radius, byte color);
void circleReference();
void wait_for_retrace(void) ;

#endif