/* NIM/Nama  : 13509047/Muhammad Hilman Beyri */
/* Nama file : T-12-13509047-mqueuelistlinear.c */
/* Topik     : ADT queue list linear */
/* Tanggal   : 22 November 2010 */
/* Deskripsi : ADT queue list linear */
#include "queue.h"
#include "graphic.h"

int main() {
	Queue Q1;
	int X;
	int i;
	CreateEmpty(&Q1);	
	printf("queue kosong ? %d\n",IsEmpty(Q1));
	printqueue(Q1);	
	for(i=1;i<=10;i++) {
		Add(&Q1,i);
		
	}
	printqueue(Q1);
	printf("queue kosong ? %d\n",IsEmpty(Q1));
	printf("elemen di awal : %d \n",InfoHead(Q1));
	printf("elemen di ujung : %d \n",InfoTail(Q1));
	for (i=1;i<=10;i++) {
		Del(&Q1,&X);
		printqueue(Q1);
		printf("yang dibuang : %d \n",X);
		printf("%p\n",Head(Q1));
		printf("%p\n",Tail(Q1));
	}
	printf("queue kosong ? %d\n",IsEmpty(Q1));
	printf("%p\n",Head(Q1));
	printf("%p\n",Tail(Q1));
	return 0;
}
