#include "graphic.h"

int main() {					
	word bitmap_offset,screen_offset;
	word start;	
	int i,repeat,plane,num_objects=0;
	word x,y;	
	BITMAP bmp;
	OBJECT *object;

	num_objects=8;

	/* allocate memory for double buffer and background image */
	initDoubleBuffer();
	
	/* allocate memory for objects */
	if ((object = (OBJECT *) malloc(sizeof(OBJECT)*num_objects)) == NULL) {
		printf("Not enough memory for objects.\n");
		free(doubleBuffer);
		exit(1);
	}

	/* load the images */
	load_bmp("balls.bmp",&bmp);

	printf("%d %d\n", bmp.width, bmp.height);
	
	/* set the object positions */
	srand(*my_clock);
	
	for(i=0;i<num_objects;i++) {
		object[i].width   = 32;
		object[i].height  = 25;
		object[i].x       = rand() % (SCREEN_WIDTH - 32 );
		object[i].y       = rand() % (SCREEN_HEIGHT- 25);
		object[i].dx      = (rand()%5) - 2;
		object[i].dy      = (rand()%5) - 2;		
	}
	
	
	set_mode(VGA_256_COLOR_MODE);      //  set the video mode.
	set_palette(bmp.palette);
	
	for(repeat=0;repeat<TOTAL_FRAMES;repeat++) {
		if ((repeat%ANIMATION_FRAMES)==0) bitmap_offset=0;
		// clear background
		memset(doubleBuffer,0,SCREEN_SIZE);

		for(i=0;i<num_objects;i++) {
			screen_offset = (object[i].y<<8) + (object[i].y<<6) + object[i].x;
			// draw the object.
			for(y=0;y<25*bmp.width;y+=bmp.width)
			for(x=0;x<32;x++)
			  if (bmp.data[bitmap_offset+y+x]!=0)
				doubleBuffer[screen_offset+y+x]=bmp.data[bitmap_offset+y+x];
			// check to see if the object is within boundries
			if (object[i].x + object[i].dx < 0 ||
			  object[i].x + object[i].dx > SCREEN_WIDTH-object[i].width-1)
				object[i].dx=-object[i].dx;
			if (object[i].y + object[i].dy < 0 ||
			  object[i].y + object[i].dy > SCREEN_HEIGHT-object[i].height-1)
				object[i].dy=-object[i].dy;
				
			// move the object
			object[i].x+=object[i].dx;
			object[i].y+=object[i].dy;
		}
		
		bitmap_offset+=32; // point to the next image in the animation 
		if ((bitmap_offset%bmp.width)==0)
		  bitmap_offset+=bmp.width*(25-1);
		
		show_buffer(doubleBuffer); // show the buffer		
	}	
	
	free(doubleBuffer);                /* free up memory used */		
		
	getch();
	set_mode(TEXT_MODE);
	return 0;
}