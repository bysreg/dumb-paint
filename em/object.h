/* File : Object.h */

#ifndef OBJECT_H
#define OBJECT_H
#include "graphic.h"

typedef enum {LINE, RECT, CIRCLE}TPolygon;
typedef struct{
	TPolygon type;
	int size; //jumlah poin yang disimpan
	point* ArrayP;
	int radius; //untuk lingkaran
	byte color;
}Object;

typedef struct TElmtList{
	Object Value; //Value object
	struct TElmtList* Next;
}ElmtList;

typedef ElmtList* ListObject;

void MakeEmpty(ListObject* LO);
void Add(ListObject* LO, Object O); //menambah objek di akhir list
void drawObject(Object O);
Object Line(point P1, point P2, int color); //constructor garis
Object Circle(point P, int radius, int color); //constructor lingkaran
Object Rect(point P1, point P2, int color); //constructor kotak

#endif