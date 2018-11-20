#include <stdio.h>

#ifndef SEG_INCLUDED
#define SEG_INCLUDED

#define T Seg_T
typedef struct T *T;

T Seg_new(int); 
void *Seg_addlo(T, void *);
void *Seg_addhi(T, void *);
void *Seg_remlo(T);
void *Seg_remhi(T);
void *Seg_put(T, int, void *);
void *Seg_get(T, int);
void Seg_free(T*);
int Seg_length(T);
void Seg_unmapp(T, int);
void Seg_map(T, T, void *);

void * Seg_value(T, int, int);

#undef T
#endif
