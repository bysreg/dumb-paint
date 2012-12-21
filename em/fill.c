#include "fill.h"

void rasterFillAll(byte color, byte mark) {
	byte pivot;	
	int bool, next, x, y;
	point start, stop;
	start.x = 0;
	start.y = 0;
	stop.x = 0;
	stop.y = 0;	
	
	bool = 0;// bool is false
	next = 0;// next is false
		
	for(y=0;y<SCREEN_WIDTH;y++) {
		for(x=0;x<SCREEN_HEIGHT;x++) {
			pivot = getColor(DoubleBuffer,x,y);
			if(pivot == mark) {
				if(bool == 0) {
					bool = 1;
					next = 1;
					start.x = x+1;					
					start.y = y;					
				} else if(next == 1 && bool == 1) {				
					bool = 1;
					start.x = x+1;					
					start.y = y;					
				} else {
					stop.x = x-1;
					stop.y = y;
				}
			} else {
				next = 0;
			} if(stop.x != 0) {
				drawLine(DoubleBuffer, start, stop, color);
				start.x = 0;
				start.y = 0;
				stop.x = 0;
				stop.y = 0;					
				bool = 0;
				next = 0;
			}
		}
		bool = 0;
		next = 0;
	}
}

void rasterFill(byte color, byte mark, int xAwal, int xAkhir, int yAwal, int yAkhir) {

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
			pivot = getColor(DoubleBuffer,x,y);
			if(pivot == mark) {
				if(bool == 0) {
					bool = 1;
					next = 1;
					start.x = x+1;					
					start.y = y;					
				} else if(next == 1 && bool == 1) {				
					bool = 1;
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
				drawLine(DoubleBuffer, start, stop, color);
				start.x = 0;
				start.y = 0;
				stop.x = 0;
				stop.y = 0;					
				bool = 1;
				next = 1;
			}
		}
		bool = 0;
		next = 0;
	}
}