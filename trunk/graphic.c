#include "graphic.h"

byte *VGA=(byte *)0xA0000000L;        /* this points to video memory. */
word *my_clock=(word *)0x0000046C;    /* this points to the 18.2hz system clock. */

byte *doubleBuffer;

void initDoubleBuffer() {
	doubleBuffer = (byte *) malloc(320 * 200);
	if(doubleBuffer == NULL) {
		printf("error");
		exit(1);
	}
}	

void set_mode(byte mode) {
	union REGS regs;
	regs.h.ah = SET_MODE;
	regs.h.al = mode;
	int86(VIDEO_INT, &regs, &regs);
}

void drawPixel(int x, int y, byte color) {
	//doubleBuffer[(y<<8) + (y<<6) + x] = color;
	VGA[y*SCREEN_WIDTH+x]=color;
}

byte getColor(int x, int y) {
	//return (doubleBuffer[(y<<8)+(y<<6)+x]);
	return VGA[y*SCREEN_WIDTH+x];
}

/**************************************************************************
 *  fskip                                                                 *
 *     Skips bytes in a file.                                             *
 **************************************************************************/

void fskip(FILE *fp, int num_bytes)
{
   int i;
   for (i=0; i<num_bytes; i++)
      fgetc(fp);
}

void set_palette(byte *palette)
{
  int i;

  outp(PALETTE_INDEX,0);              /* tell the VGA that palette data
                                         is coming. */
  for(i=0;i<256*3;i++)
    outp(PALETTE_DATA,palette[i]);    /* write the data */
}

void load_bmp(char *file,BITMAP *b)
{
  FILE *fp;
  long index;
  word num_colors;
  int x;

  /* open the file */
  if ((fp = fopen(file,"rb")) == NULL)
  {
    printf("Error opening file %s.\n",file);
    exit(1);
  }

  /* check to see if it is a valid bitmap file */
  if (fgetc(fp)!='B' || fgetc(fp)!='M')
  {
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
  if ((b->data = (byte *) malloc((word)(b->width*b->height))) == NULL)
  {
    fclose(fp);
    printf("Error allocating memory for file %s.\n",file);
    exit(1);
  }

  /* read the palette information */
  for(index=0;index<num_colors;index++)
  {
    b->palette[(int)(index*3+2)] = fgetc(fp) >> 2;
    b->palette[(int)(index*3+1)] = fgetc(fp) >> 2;
    b->palette[(int)(index*3+0)] = fgetc(fp) >> 2;
    x=fgetc(fp);
  }

  /* read the bitmap */
  for(index = (b->height-1)*b->width; index >= 0;index-=b->width)
    for(x = 0; x < b->width; x++)
      b->data[(int)(index+x)]=(byte)fgetc(fp);

  fclose(fp);
}

void drawLine(int x0, int y0, int x1, int y1, byte color) {
	int steep = abs(y1 - y0) > abs(x1 - x0);
	int temp, deltax, deltay, error, ystep, x, y;
	if (steep) {
		temp = x0;
		x0 = y0;
		y0 = temp;
		temp = x1;
		x1 = y1;
		y1 = temp;
	}
	if (x0 > x1) {
		temp = x0;
		x0 = x1;
		x1 = temp;
		temp = y0;
		y0 = y1;
		y1 = temp;
	}
	deltax = x1 - x0;
	deltay = abs(y1 - y0);
	error = deltax / 2;
	y = y0;
	if (y0 < y1) {ystep = 1;} else {ystep = -1;}
	for (x=x0; x<=x1; x++) {
		if (steep) drawPixel(y,x,color); 
		else drawPixel(x,y,color);
		error = error - deltay;
		if (error < 0) {
			y = y + ystep;
			error = error + deltax;
		}
	}
}

float convertAngleToRadian(float angle) {
	return angle * PI / 180.0;
}

void matrix3x3SetIdentity(Matrix3x3 m) {
	int i, j;
	
	for(i=0;i<3;i++) {
		for(j=0;j<3;j++) {
			m[i][j] = (i == j);
		}
	}
}

void matrix3x3PreMultiply(Matrix3x3 a, Matrix3x3 b) {
	int r, c;
	Matrix3x3 tmp;
	
	for(r = 0;r<3;r++) {
		for(c=0;c<3;c++) {
			tmp[r][c] = a[r][0] * b[0][c] + a[r][1] * b[1][c] + a[r][2] * b[2][c];			
		}
	}
	
	for(r = 0;r<3;r++) {
		for(c=0;c<3;c++) {
			b[r][c] = tmp[r][c];
		}
	}
}

void translate2(int tx, int ty, Matrix3x3 mat) {
	Matrix3x3 m;
	
	matrix3x3SetIdentity(m);
	m[0][2] = tx;
	m[1][2] = ty;
	matrix3x3PreMultiply(m, mat);
}

void scale2(float sx, float sy, vec2 refPt, Matrix3x3 mat) {
	Matrix3x3 m;
	
	matrix3x3SetIdentity(m);
	m[0][0] = sx;
	m[0][2] = (1-sx) * refPt.x;
	m[1][1] = sy;
	m[1][2] = (1-sy) * refPt.y;
	matrix3x3PreMultiply(m, mat);
}

void rotate2(float a, vec2 refPt, Matrix3x3 mat) {
	Matrix3x3 m;
	
	matrix3x3SetIdentity(m);
	a = convertAngleToRadian(a);
	m[0][0] = cos(a);
	m[0][1] = -sin(a);
	m[0][2] = refPt.x * (1 - cos(a)) * refPt.y * sin(a);
	m[1][0] = sin(a);
	m[1][1] = cos(a);
	m[1][2] = refPt.y * (1 - cos(a)) - refPt.y * sin(a);
	matrix3x3PreMultiply(m, mat);
}

void transformPoints2(int npts, vec2 *pts, Matrix3x3 mat) {
	int k;
	float tmp;
	
	for(k=0;k<npts;k++) {
		tmp = mat[0][0] * pts[k].x + mat[0][1] * pts[k].y + mat[0][2];
		pts[k].y = mat[1][0] * pts[k].x + mat[1][1] * pts[k].y + mat[1][2];
		pts[k].x = tmp;
 	}
}

vec2 calcTransformPoint2(vec2 p, Matrix3x3 mat) {
	vec2 ret;
	ret.x = mat[0][0] * p.x + mat[0][1] * p.y + mat[0][2];
	ret.y = mat[1][0] * p.x + mat[1][1] * p.y + mat[1][2];
	return ret;
}

void printMatrix3x3(Matrix3x3 m) {
	int i, j=0;
	for(i=0;i<3;i++) {
		for(j=0;j<3;j++) {
			printf("%f ", m[i][j]);
		}
		printf("\n");
	}	
}	

void rasterFill(byte color, byte mark, int xAwal, int yAwal, int xAkhir, int yAkhir) {

	byte pivot;	
	int bool, next, x, y;
	point start, stop;
	start.x = 0;
	start.y = 0;
	stop.x = 0;
	stop.y = 0;	
	
	bool = 0;// bool is false
	next = 0;// next is false
		
	for(y=yAwal;y<=yAkhir;y++) {
		for(x=xAwal;x<=xAkhir;x++) {
			pivot = getColor(x,y);
			if(pivot == mark) {
				if(bool == 0) {
					bool = 1;
					next = 1;
					start.x = x+1;					
					start.y = y;					
				} else if(next == 1 && bool == 1) {									
					start.x = x+1;					
					start.y = y;					
				} else  {
					stop.x = x-1;
					stop.y = y;
				}
			} else {
				next = 0;
			}
			if(stop.x != 0)
			{
				drawLine(start.x, start.y, stop.x, stop.y, color);				
				break;
			}
		}
		start.x = 0;
		start.y = 0;
		stop.x = 0;
		stop.y = 0;					
		bool = 0;
		next = 0;
	}
}

void copyPolygon(vec2 *p1, vec2 *p2, int n) {
	int i=0;
	for(i=0;i<n;i++) {
		p1[i].x = p2[i].x;
		p1[i].y = p2[i].y;		
	}
}

void drawPolygon(vec2 *points, int n, byte color) {
	int i=0;
	for(i=0;i<n;i++) {
		drawLine(points[i].x, points[i].y, points[(i+1) % n].x, points[(i+1) % n].y, color);
	}
}

void drawRect(int x1, int y1, int x2, int y2, byte lineColor, byte fillColor) {
	int x3 = x2;
	int y3 = y1;
	int x4 = x1;
	int y4 = y2;
	
	drawLine(x1, y1, x3, y3, lineColor);
	drawLine(x3, y3, x2, y2, lineColor);
	drawLine(x2, y2, x4, y4, lineColor);
	drawLine(x4, y4, x1, y1, lineColor);	
	
	rasterFill(fillColor, lineColor, x1, y1, x2, y2);
}

void show_buffer(byte *buffer) {
  #ifdef VERTICAL_RETRACE
    while ((inp(INPUT_STATUS_1) & VRETRACE));
    while (!(inp(INPUT_STATUS_1) & VRETRACE));
  #endif
  memcpy(VGA,buffer,SCREEN_SIZE);
}