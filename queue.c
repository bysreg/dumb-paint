/* NIM/Nama  : 13509047/Muhammad Hilman Beyri */
/* Nama file : T-12-13509047-queuelistlinear.c */
/* Topik     : ADT queue list linear */
/* Tanggal   : 22 November 2010 */
/* Deskripsi : ADT queue list linear */
#include "queue.h"

/*
{ *** Prototype manajemen memori *** }
procedure Alokasi (output P : address, input X : infotype)
{ I.S. P sembarang, X terdefinisi }
{ F.S. Alamat P dialokasi, jika berhasil maka Info(P) = X dan Next(P) = Nil }
{P = Nil jika alokasi gagal }
procedure Dealokasi (input P : address)
{ I.S. P adalah hasil alokasi, P <> Nil }
{ F.S. Alamat P didealokasi, dikembalikan ke sistem }
*/

	void Alokasi(address* P, infotype X) {
		/* kamus lokal */

		/* algoritma */
		(*P) = (address) malloc (sizeof(ElmtQueue));
		if (*P != Nil) {
			Info(*P) = X;
			Next(*P) = Nil;
		}
	}
	
	void Dealokasi(address P) {
		free(P);
	}

/*
{ *** Predikat Pemeriksaan Kondisi Queue *** }
function IsEmpty (Q : Queue) -> boolean
{ Mengirim true jika Q kosong: HEAD(Q) = Nil and TAIL(Q) = Nil }
function NBElmt (Q : Queue) -> integer
{ Mengirimkan banyaknya elemen queue. Mengirimkan 0 jika Q kosong. }
*/

	boolean IsEmpty(Queue Q){
		return (Head(Q) == Nil && Tail(Q) == Nil);
	}
	
	int NBElmt(Queue Q) {
		address P;
		int jumlah = 0;
		P = Head(Q);
		while(P != Nil) {
			jumlah++;
			P = Next(P);
		}
		return jumlah;
	}
	
/*
{ *** Konstruktor *** }
procedure CreateEmpty (input/output Q : Queue)
{ I.S. sembarang }
{ F.S. Sebuah Q kosong terbentuk }
*/

	void CreateEmpty(Queue* Q){
		Head(*Q)=Nil;
		Tail(*Q)=Nil;
	}

/*
{ *** Primitif Add/Delete *** }
procedure Add (input/output Q : Queue, input X : infotype)
{ Proses : Mengalokasi X dan menambahkan X pada bagian TAIL dari Q jika alokasi
berhasil; jika alokasi gagal Q tetap }
{ I.S. Q mungkin kosong }
{ F.S. X menjadi TAIL, TAIL "maju" }
procedure Del (input/output Q : Queue, output X : infotype)
{ Proses : Menghapus X pada bagian HEAD dari Q dan mendealokasi elemen HEAD }
{ I.S. Q tidak mungkin kosong }
{ F.S. X = nilai elemen HEAD pd I.S., HEAD "mundur" }
*/

	void Add(Queue* Q, infotype X){
		address P;
		Alokasi(&P,X);
		if(P!=Nil) {
			if(IsEmpty(*Q)) {
				Head(*Q) = P; 
				Tail(*Q) = P;	
			}else{
				Next(Tail(*Q)) = P;
				Tail(*Q) = P;
			}
		}
	}
	void Del(Queue* Q, infotype* X){
		address P;
		*X = InfoHead(*Q);
		P = Head(*Q);
		Head(*Q) = Next(Head(*Q));
		if(Head(*Q)==Nil) {
			Tail(*Q) = Nil;
		}
		Dealokasi(P);
	}
	void printqueue(Queue Q) {
		address P;
		P = Head(Q);
		while(P!=Nil) {
			printf("%d ",Info(P));
			P = Next(P);
		}
		printf("\n");
	}
