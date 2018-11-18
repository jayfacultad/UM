#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <inttypes.h>
#include "segments.h"
#include <bitpack.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <assert.h>

#include "uarray.h"

#define Seq_T Seg_T

#define UArray_put(uarray, index, value) (uint32_t *word = UArray_at(uarray, index); *word = value;)

const int byte_len = 8;

void conditional_move(uint32_t (*$r)[], short a, short b, short c);
void segmented_load(uint32_t (*$r)[], Seg_T $m, short a, short b, short c);
void segmented_store(uint32_t (*$r)[], Seg_T $m, short a, short b, short c);
void addition(uint32_t (*$r)[], short a, short b, short c);
void multiplication(uint32_t (*$r)[], short a, short b, short c);
void division(uint32_t (*$r)[], short a, short b, short c);
void bitwise_nand(uint32_t (*$r)[], short a, short b, short c);
void halt();
void map_segment(uint32_t (*$r)[], Seg_T $m, short b, short c);
void umap_segment(uint32_t (*$r)[], Seg_T $m, short c);
void output(uint32_t (*$r)[], short c);
void input(uint32_t (*$r)[], short c);
void load_program(uint32_t (*$r)[], Seg_T $m, int num_words, short a, short b, short c, uint32_t * program_counter);
void load_value(uint32_t (*$r)[], short a, uint32_t value);


int main (int argc, char *argv[])
{
        FILE * fp;
        uint32_t $r[8];                // Registers
        Seg_T $m = Seg_new(1000);      // Sequence of segments
        uint32_t *program_counter;     // Program Counter

        if (argc == 2) {
                fp = fopen(argv[1], "r");
        }
        else {
                fprintf(stderr, "Need one file for input");
        }

        int fd = fileno(fp);
        struct stat st;
        stat(argv[1], &st);
        int file_size = st.st_size;
        int num_words = file_size / (sizeof(uint32_t)); 

        // Create and load Segment 0 with every instruction
        UArray_T arr = UArray_new(num_words, sizeof(uint32_t));
        Seg_addhi($m, &arr);
        
        int offset = 0;
        for (int i = 0; i < num_words; i++) {
                uint32_t word = 0;
                for (int j = 0; j < 4; j++) { 
                        uint32_t extract_byte = (uint32_t)getc(fp);
                        word = Bitpack_newu(word, byte_len, 24 - (byte_len  * j), extract_byte);             
                }
                // printf("%x\n", word);
                uint32_t *value = UArray_at(arr, offset++);
                *value = word;
        }
        Seg_put($m, 0, &arr);
        //        fprintf(stdout, "%x\n", *(uint32_t*)UArray_at(*(UArray_T *)Seg_get($m, 0),0));
        fclose(fp);


        int x = 0;
        program_counter = (uint32_t *)UArray_at(*(UArray_T*)Seg_get($m, 0), 0);
                //(uint32_t *)UArray_at(*(UArray_T*)Seg_get($m, 0), 0);
        short op_code;
        do {
                uint32_t instruction = *program_counter;

                op_code = Bitpack_getu(instruction, 4, 28);
                short a = 0;
                short b = 0;
                short c = 0;
                uint32_t value = 0;
                if (op_code != 13 ) {
                        a = Bitpack_getu(instruction, 3, 6);
                        b = Bitpack_getu(instruction, 3, 3);
                        c = Bitpack_getu(instruction, 3, 0);
                } else {
                        a = Bitpack_getu(instruction, 3, 25);
                        value = Bitpack_getu(instruction, 24, 0);
                }
                assert(op_code >= 0 && op_code < 14 && a >= 0 && a < 8 && b >= 0 && b < 8 && c >= 0 && c < 8);
                switch (op_code) {
                        case 0: conditional_move(&$r, a, b, c);
                                break;
                        case 1: segmented_load(&$r, $m, a, b, c);
                                break;
                        case 2: segmented_store(&$r, $m, a, b, c);
                                break;
                        case 3: addition(&$r, a, b, c);
                                break;
                        case 4: multiplication(&$r, a, b, c);
                                break;   
                        case 5: division(&$r, a, b, c);
                                break;
                        case 6: bitwise_nand(&$r, a, b, c);
                                break;     
                        case 7: halt();
                                break;     
                        case 8: map_segment(&$r, $m, b, c);
                                break;     
                        case 9: umap_segment(&$r, $m, c);
                                break;     
                        case 10: output(&$r, c);
                                break;     
                        case 11: input(&$r, c);
                                break;     
                        case 12: load_program(&$r, $m, num_words, a, b, c,  program_counter);
                                break;     
                        case 13: load_value(&$r, a, value);
                                break;              
                }
                program_counter = (uint32_t*) UArray_at(*(UArray_T *) Seg_get($m,0), ++x);

        } while (op_code != 7);

        (void)fd;

        UArray_free(&arr);
        Seg_free($m);
        

        return EXIT_SUCCESS;
}


void conditional_move(uint32_t (*$r)[], short a, short b, short c)
{
        if ((*$r)[c] != 0) {
                (*$r)[a] = (*$r)[b];
        }
}

void segmented_load(uint32_t (*$r)[], Seg_T $m, short a, short b, short c)
{
        // Need to check for nonexisting segments
        UArray_T segment = (UArray_T) Seg_get($m, (*$r)[b]);
        (*$r)[a] = *(uint32_t *) UArray_at(segment,(*$r)[c]);
}

void segmented_store(uint32_t (*$r)[], Seg_T $m, short a, short b, short c)
{
        UArray_T segment = (UArray_T) Seg_get($m, (*$r)[a]);
        int index = (*$r)[b];

        uint32_t* word = (uint32_t *) UArray_at(segment, index);
        *word = (*$r)[c]; 
}

void addition(uint32_t (*$r)[], short a, short b, short c)
{
        uint32_t temp = 1;
        (*$r)[a] = ((*$r)[b] + (*$r)[c]) & ((temp << 31) - 1); 
}

void multiplication(uint32_t (*$r)[], short a, short b, short c)
{
        uint32_t temp = 1;
        (*$r)[a] = ((*$r)[b] * (*$r)[c]) & ((temp << 31) - 1); 
}

void division(uint32_t (*$r)[], short a, short b, short c)
{
        uint32_t temp = 1;
        (*$r)[a] = ((*$r)[b] / (*$r)[c]) & ((temp << 31) - 1); 
}

void bitwise_nand(uint32_t (*$r)[], short a, short b, short c)
{
        (*$r)[a] = ~((*$r)[b] & (*$r)[c]);
}

void halt()
{
        exit(0);
}

void map_segment(uint32_t (*$r)[], Seg_T $m, short b, short c) 
{
        uint32_t seg_index = (*$r)[b];
        if (seg_index == 0) {
                return;
        }
        uint32_t size = (*$r)[c];
        UArray_T arr = UArray_new(size, sizeof(uint32_t));

        while (seg_index + 1> (uint32_t)Seg_length($m)) {
                Seg_addhi($m, NULL);
        }
        if ((UArray_T *)Seg_get($m, seg_index) == NULL) {
                Seg_put($m, seg_index, arr);
        }
}

void umap_segment(uint32_t (*$r)[], Seg_T $m, short c)
{
        // Need to free UArray
        UArray_T to_unmap = (UArray_T) Seg_get($m, (*$r)[c]);
        UArray_free(&to_unmap);
        Seg_unmapp($m, (*$r)[c]);
}

void output(uint32_t (*$r)[], short c)
{
        putchar((*$r)[c]);
}

void input(uint32_t (*$r)[], short c)
{
        char extracted_byte = getchar();
        if (extracted_byte == EOF) {
                (*$r)[c] = ~0;
        } else {
                (*$r)[c] = extracted_byte;
        }
        (void)c;
}

void load_program(uint32_t (*$r)[], Seg_T $m, int num_words, short a, short b, short c, uint32_t * program_counter)
{
        printf("%u\n", (*$r)[b]);
        if((*$r)[b] != 0) {
                int length = sizeof((uint32_t (*)[])Seg_get($m, (*$r)[b]))/sizeof(uint32_t);
                UArray_T duplicate = UArray_new(length, sizeof(uint32_t));
                duplicate = *(UArray_T *)Seg_get($m, (*$r)[b]);
                
                UArray_T *seg_zero = (UArray_T *)Seg_get($m, 0);
                *seg_zero = duplicate;
        }      
        UArray_T program = (*(UArray_T *)Seg_get($m, 0));
        program_counter = (uint32_t *) UArray_at(program, (*$r)[c]);

        (void)a;
        (void)c;
        (void)num_words;
        (void)program_counter;
}

void load_value(uint32_t (*$r)[], short a, uint32_t value)
{
        (*$r)[a] = value;
}
