/* File : Object.c */

#include "Object.h"

void MakeEmpty(ListObject* LO){
	*LO = NULL;
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
void drawObject(Object O){
	switch(O.type){
		case LINE:
			drawLine(DoubleBuffer, O.ArrayP[0], O.ArrayP[1], O.color);
			break;
		case RECT:
			drawRect(DoubleBuffer, O.ArrayP[0], O.ArrayP[1], O.color);
			break;
		case CIRCLE:
			drawCircle(DoubleBuffer, O.ArrayP[0], O.radius, O.color);
			break;
	}
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