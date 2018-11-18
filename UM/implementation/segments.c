#include <stdio.h>
#include "seq.h"
#include <inttypes.h>

#define T Seg_T
typedef struct T *T;

T Seg_new(int hint) 
{
        return (T) Seq_new(hint);
}

void *Seg_addlo(T seg, void *x)
{
        return Seq_addlo((Seq_T) seg, x);
}

void *Seg_addhi(T seg, void *x)
{
        return Seq_addhi((Seq_T) seg, x);
}

void *Seg_remhi(T seg)
{
        return Seq_remhi((Seq_T) seg);
}

void *Seg_remlo(T seg)
{
        return Seq_remlo((Seq_T) seg);
}

void *Seg_get(T seg, int i)
{
        return Seq_get((Seq_T) seg, i);
}

void *Seg_put(T seg, int i, void *x)
{
        return Seq_put((Seq_T) seg, i, x);
}

void Seg_free(T seg)
{
        Seq_free((Seq_T *)(seg));
}

int Seg_length(T seg)
{
        return Seq_length((Seq_T) seg);
}

int Seg_size(T seg, int i) 
{
        (void) seg;
        (void) i;
        return 10;
}

void Seg_unmapp(T seg, int i)
{
        Seg_put(seg, i, NULL);
}

void Seg_map(T seg, T unmapped, void *x)
{
        if(Seg_length(unmapped) > 0) {
                Seg_put(seg, *(int *)Seg_remlo(unmapped), x);
        } else {
                Seg_addhi(seg, x);
        }        
}



void * Seg_value(T segments, int index, int offset) 
{
        int arrsize = Seg_size(segments, index);
        int (*seg)[arrsize] = Seg_get(segments, index);
        return (seg)[offset];
}

#undef T
