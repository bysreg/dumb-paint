#include "graphic.h"

byte *VGA=(byte *)0xA0000000L;        /* this points to video memory. */
word *my_clock=(word *)0x0000046C;    /* this points to the 18.2hz system clock. */

byte *doubleBuffer;
byte *canvas;
letter* dict; // kamus huruf
Matrix3x3 _transmat;


void initDoubleBuffer() {
	doubleBuffer = (byte *) malloc(320 * 200);	
	if(doubleBuffer == NULL) {
		printf("error");
		exit(1);
	}
	canvas = (byte *) malloc(SCREEN_WIDTH * SCREEN_HEIGHT);
	if(canvas == NULL) {
		printf("error");
		exit(1);
	}
	matrix3x3SetIdentity(_transmat);
	memset(doubleBuffer,0,SCREEN_SIZE);	
	memset(canvas,0,SCREEN_SIZE);	
}	

void loadFontDb() {	
	char buffer[1000];
	int n, i, lineCount, j;	
	int x0, y0, x1, y1;
	FILE *fp;
	
	fp = fopen("db.txt", "r");
	if (fp == NULL) {
		perror("Error opening file");
	} else {				
		fgets(buffer, 100, fp);
		sscanf(buffer, "%d", &n);		
		dict = (letter*) malloc(sizeof(letter) * n);			
		
		for(i=0;i < n;i++) {			
			letter let;
			
			fgets(buffer , 100 , fp); //baris pertama deskripsi huruf dihiraukan			
			fgets(buffer , 100 , fp); //baris kedua menyatakan jumlah garis yang dibutuhkan			
			sscanf(buffer, "%d", &lineCount);			
			let.lineCount = lineCount;
			let.lines = (line*) malloc(sizeof(line) * lineCount);									
			for(j=0;j<lineCount;j++) {
				point p1, p2;
				line l;
				fgets(buffer , 100 , fp); //tiap baris berikutnya menyatakan dua titik yang membentuk garis (x0 y0 x1 y1)				
				sscanf(buffer, "%d %d %d %d", &x0, &y0, &x1, &y1);								
				p1.x = x0;p1.y = y0;
				p2.x = x1, p2.y = y1;								
				l.p1 = p1;l.p2 = p2;
				let.lines[j] = l;
			}
			dict[i] = let;
		}		
		
		fclose (fp);
	}
}

void set_mode(byte mode) {
	union REGS regs;
	regs.h.ah = SET_MODE;
	regs.h.al = mode;
	int86(VIDEO_INT, &regs, &regs);
}

void drawPixel(int x, int y, byte color) {
	canvas[(y<<8) + (y<<6) + x] = color;	
	//doubleBuffer[(y<<8) + (y<<6) + x] = color;	
}

byte getColor(int x, int y) {
	return (canvas[(y<<8)+(y<<6)+x]);	
	//return (doubleBuffer[(y<<8)+(y<<6)+x]);	
}

//skip byte di file
void fskip(FILE *fp, int num_bytes) {
   int i;
   for (i=0; i<num_bytes; i++)
      fgetc(fp);
}

void set_palette(byte *palette) {
  int i;

  outp(PALETTE_INDEX,0);              /* tell the VGA that palette data
                                         is coming. */
  for(i=0;i<256*3;i++)
    outp(PALETTE_DATA,palette[i]);    /* write the data */
}

void load_bmp(char *file,BITMAP *b) {
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

void drawCircle(int xCenter, int yCenter, int radius, byte color) 
{
	int x = 0;
	int y = radius;
	int p = 1 - radius;
	int ddF_x = 1;
	int ddF_y = -2 * radius;
	
	drawPixel(xCenter + radius, yCenter, color);
	drawPixel(xCenter - radius, yCenter, color);
	drawPixel(xCenter, yCenter + radius, color);
	drawPixel(xCenter, yCenter - radius, color);
	
	while(x < y) {
		if(p>=0) {
			y--;
			ddF_y += 2;			
			p += ddF_y;
		}
		x++;
		ddF_x += 2;
		p += ddF_x;
		drawPixel(xCenter + x, yCenter + y, color);
		drawPixel(xCenter + x, yCenter - y, color);
		drawPixel(xCenter - x, yCenter + y, color);
		drawPixel(xCenter - x, yCenter - y, color);
		drawPixel(xCenter + y, yCenter + x, color);
		drawPixel(xCenter + y, yCenter - x, color);
		drawPixel(xCenter - y, yCenter + x, color);
		drawPixel(xCenter - y, yCenter - x, color);
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

void floodFill(int x, int y, byte target_color, byte replacement_color) {
	byte color = getColor(x, y);		
	byte color_1 = getColor(x-1, y);
	byte color_2 = getColor(x, y-1);
	byte color_3 = getColor(x+1, y);
	byte color_4 = getColor(x, y+1);
	//printf("%d %d %d", x, y, color);
		//getch();
	if(color == target_color) {						
		//printf(" lanjut %d \n", (y<<8) + (y<<6) + x);
		drawPixel(x, y, replacement_color);			
		
		if(color_1 == target_color) 
		floodFill(x-1, y, target_color, replacement_color);
		
		if(color_2 == target_color) 
		floodFill(x, y - 1, target_color, replacement_color);
		
		if(color_3 == target_color) 
		floodFill(x + 1, y , target_color, replacement_color);
		
		if(color_4 == target_color) 
		floodFill(x, y + 1, target_color, replacement_color);		
	}else{
		//printf("\n");
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

void drawPolygonp(point *points, int n, byte color) {
	int i=0;
	for(i=0;i<n;i++) {
		drawLine(points[i].x, points[i].y, points[(i+1) % n].x, points[(i+1) % n].y, color);
	}
}

void drawRect(int x1, int y1, int x2, int y2, byte lineColor) {
	int x3 = x2;
	int y3 = y1;
	int x4 = x1;
	int y4 = y2;
	
	drawLine(x1, y1, x3, y3, lineColor);
	drawLine(x3, y3, x2, y2, lineColor);
	drawLine(x2, y2, x4, y4, lineColor);
	drawLine(x4, y4, x1, y1, lineColor);	
	
	//rasterFill(fillColor, lineColor, x1, y1, x2, y2);
}

void drawBitmap(BITMAP bmp, int xscreen, int yscreen) {		
	int x, y = 0;
	
	for(y=0;y<bmp.height;y++) {							
		for(x=0;x<bmp.width;x++) {				  
			if (bmp.data[y*bmp.width+x]!=0) {						
				drawPixel(xscreen + x, yscreen + y, bmp.data[y*bmp.width+x]);						
			}
		}
	}	
}

void show_buffer() {
  #ifdef VERTICAL_RETRACE
    while ((inp(INPUT_STATUS_1) & VRETRACE));
    while (!(inp(INPUT_STATUS_1) & VRETRACE));
  #endif
  
  memset(doubleBuffer,0,SCREEN_SIZE);
  transformCanvas();
  memcpy(VGA,doubleBuffer,SCREEN_SIZE);  
}

void transformPoint2(int *x, int *y, Matrix3x3 mat) {
	int tx, ty = 0;
	
	tx = *x;
	ty = *y;
	*x = mat[0][0] * tx + mat[0][1] * ty + mat[0][2];
	*y = mat[1][0] * tx + mat[1][1] * ty + mat[1][2];
}

void transformCanvas() {
	int x, y, xbuf, ybuf = 0;
	
	for(y=0;y<SCREEN_HEIGHT;y++) {
		for(x=0;x<SCREEN_WIDTH;x++) {
			xbuf = x;
			ybuf = y;
			transformPoint2(&xbuf, &ybuf, _transmat);
			//if(_transmat[0][0]==2) {
				//printf("%d %d %d %d\n", x, y, xbuf, ybuf);
			//}	
			if(xbuf < SCREEN_WIDTH && ybuf < SCREEN_HEIGHT) {
				doubleBuffer[(ybuf<<8) + (ybuf<<6) + xbuf] = getColor(x, y);	
			}
		}
	}
}

void drawLetter(int letterIndex, int x, int y, byte color) {
	letter l = dict[letterIndex];
	int i;
	for(i=0;i<l.lineCount;i++) {
		drawLine(l.lines[i].p1.x + x, l.lines[i].p1.y + y, l.lines[i].p2.x + x, l.lines[i].p2.y + y, color);		
	}
}

//hanya menerima huruf kecil, dan huruf tersebut harus terdaftar di kamus
void drawString(const char* s, int x, int y, int space, byte color) {
	int i, letterIndex, curX;
	int len = strlen(s);
	
	curX = x;
	for(i=0;i < len;i++) {		
		letterIndex = s[i] - 'a';
		drawLetter(letterIndex, curX, y, color);
		curX += space;
	}	
}

void zoom(float scale) {	
	vec2 origin = {0,0};	
	
	matrix3x3SetIdentity(_transmat);		
	scale2(scale, scale, origin, _transmat);
}

void createBezier4(point *points, point p1, point p2, point p3, point p4, float mu) {	
	point p;
	int count = 0;
	float i=0;
	
	while(i<=1) {
		p = bezier4(p1,p2,p3,p4,i);
		points[count] = p;
		//printf("%f %d %d %d\n", i, count, p.x, p.y);
		i+=mu;
		count++;
	}
}

point bezier4(point p1, point p2, point p3, point p4, float mu){
	float mum1,mum13,mu3;
	point p;

	mum1 = 1 - mu;
	mum13 = mum1 * mum1 * mum1;
	mu3 = mu * mu * mu;

	p.x = mum13*p1.x + 3*mu*mum1*mum1*p2.x + 3*mu*mu*mum1*p3.x + mu3*p4.x;
	p.y = mum13*p1.y + 3*mu*mum1*mum1*p2.y + 3*mu*mu*mum1*p3.y + mu3*p4.y;  

	return p;
}

void drawPoints(point *points, int len, byte color) {
	int i=0;
	
	for(i=0;i<len;i++) {
		drawPixel(points[i].x, points[i].y, color);
	}
}
