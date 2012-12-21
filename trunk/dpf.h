#ifndef DPF_H
#define DPF_H

#include "graphic.h"

typedef struct {
	byte *data;
} DPF;

void open_dpf(char* file, DPF *dpf);
void save_dpf(char* file, byte* bitmap);
void draw_dpf(DPF dpf);

#endif