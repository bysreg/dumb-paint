
#ifndef FILL_H
#define FILL_H

#include "graphic.h"

// color = warna yang ingin di taruh
// mark = outline yang digunakan
void rasterFillAll(byte color, byte mark);
void rasterFill(byte color, byte mark, int xAwal, int xAkhir, int yAwal, int yAkhir);

#endif /* FILL_H */