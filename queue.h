/* NIM/Nama  : 13509047/Muhammad Hilman Beyri */
/* Nama file : T-12-13509047-queuelistlinear.h */
/* Topik     : ADT queue list linear */
/* Tanggal   : 22 November 2010 */
/* Deskripsi : ADT queue list linear */

#ifndef QUEUELINEAR_H
	#define QUEUELINEAR_H
	#include "bool.h"
	#include <stdio.h>
	#include <stdlib.h>
	
/*	
{ Modul Queue }
{ Direpresentasikan dengan list linier, secara fisik dengan pointer }
{ Queue direpresentasikan sebagai list dengan first dan last }
constant Nil : ... { terdefinisi }
*/

	#define Nil NULL
	
/*
{ *** Definisi Type Queue *** }
type infotype: integer
type address : pointer to ElmtQueue
type ElmtQueue : < info : infotype,next : address>
{ Type Queue dengan ciri HEAD dan TAIL : }
type Queue : < HEAD : address, { alamat penghapusan }
	       TAIL : address { alamat penambahan } >
*/

	typedef int infotype;
	typedef struct tElmtQueue* address;
	typedef struct tElmtQueue {
		infotype Info;
		address Next;
	} ElmtQueue;
	typedef struct {
		address HEAD;
		address TAIL;
	} Queue;
	#define Head(Q) (Q).HEAD
	#define Tail(Q) (Q).TAIL
	#define InfoHead(Q) (Q).HEAD->Info
	#define InfoTail(Q) (Q).TAIL->Info
	#define Next(P) (P)->Next
	#define Info(P) (P)->Info

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

	void Alokasi(address* P, infotype X);
	void Dealokasi(address P);

/*
{ *** Predikat Pemeriksaan Kondisi Queue *** }
function IsEmpty (Q : Queue) -> boolean
{ Mengirim true jika Q kosong: HEAD(Q) = Nil and TAIL(Q) = Nil }
function NBElmt (Q : Queue) -> integer
{ Mengirimkan banyaknya elemen queue. Mengirimkan 0 jika Q kosong. }
*/

	boolean IsEmpty(Queue Q);
	int NBElmt(Queue Q);
	
/*
{ *** Konstruktor *** }
procedure CreateEmpty (input/output Q : Queue)
{ I.S. sembarang }
{ F.S. Sebuah Q kosong terbentuk }
*/

	void CreateEmpty(Queue* Q);

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

	void Add(Queue* Q, infotype X);
	void Del(Queue* Q, infotype* X);
	void printqueue(Queue Q);

#endif
