#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <inttypes.h>
#include <bitpack.h>
#include <math.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <assert.h>

#include "uarray.h"
#include "seq.h"

const int byte_len = 8;

void conditional_move(uint32_t (*$r)[], short a, short b, short c);
void segmented_load(uint32_t (*$r)[], Seq_T $m, short a, short b, short c);
void segmented_store(uint32_t (*$r)[], Seq_T $m, short a, short b, short c);
void addition(uint32_t (*$r)[], short a, short b, short c);
void multiplication(uint32_t (*$r)[], short a, short b, short c);
void division(uint32_t (*$r)[], short a, short b, short c);
void bitwise_nand(uint32_t (*$r)[], short a, short b, short c);
void halt();
void map_segment(uint32_t (*$r)[], Seq_T $m, short b, short c, Seq_T unmapped);
void unmap_segment(uint32_t (*$r)[], Seq_T $m, short c, Seq_T unmapped);
void output(uint32_t (*$r)[], short c);
void input(uint32_t (*$r)[], short c);
void load_program(uint32_t (*$r)[], Seq_T $m, short b);
void load_value(uint32_t (*$r)[], short a, uint32_t value);

int main (int argc, char *argv[])
{
        FILE * fp;
        uint32_t $r[8] = {0};           // Registers
        Seq_T $m = Seq_new(1000);       // Sequence of Seqments
        Seq_T unmapped = Seq_new(100);  // Stack of unmapped Seqments
        uint32_t *program_counter = 0;  // Program Counter

        if (argc == 2) {
                fp = fopen(argv[1], "r");
        }
        else {
                fprintf(stderr, "Need one file for input");
        }

        uint32_t fd = fileno(fp);
        struct stat st;
        stat(argv[1], &st);
        uint32_t file_size = st.st_size;
        uint32_t num_words = file_size / (sizeof(uint32_t)); 
        (void)fd;

        // Create and load Seqment 0 with every instruction
        UArray_T arr = UArray_new(num_words, sizeof(uint32_t));
        uint32_t x = 0;
        while (x < num_words) {
                uint32_t *value = (uint32_t*)UArray_at(arr, x++);
                *value = 0;
        }
        
        uint32_t offset = 0;
        for (uint32_t i = 0; i < num_words; i++) {
                uint32_t word = 0;
                for (uint32_t j = 0; j < 4; j++) { 
                        uint32_t extract_byte = (uint32_t)getc(fp);
                        word = Bitpack_newu(word, byte_len, 24 - (byte_len  * j), extract_byte);             
                }
                uint32_t *value = UArray_at(arr, offset++);
                *value = (uint32_t)word;
        }

        Seq_addhi($m, arr);    

        program_counter = (uint32_t *)UArray_at((UArray_T)Seq_get($m, 0), 0);

        short op_code = 0;
        uint32_t max = num_words;

        uint32_t i = 0;

        while (i < max) {

                uint32_t word = *program_counter;

                if ( (i + 1) < max ) {
                        program_counter = (uint32_t*) UArray_at( (UArray_T) Seq_get($m,0), ++i);
                } else {
                        ++i;
                }
               
                op_code = Bitpack_getu(word, 4, 28);

                short a = 0;
                short b = 0;
                short c = 0;
                uint32_t value = 0;

                if (op_code != 13 ) {
                        a = Bitpack_getu(word, 3, 6);
                        b = Bitpack_getu(word, 3, 3);
                        c = Bitpack_getu(word, 3, 0);
                } else if (op_code == 13) {
                        a = Bitpack_getu(word, 3, 25);
                        value = Bitpack_getu(word, 25, 0);
                        value = value << 7;
                        value = value >> 7;
                }
                assert(op_code >= 0 && op_code < 14);

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
                        case 7: 
                                while (Seq_length($m) != 0) {
                                        UArray_T to_delete = (UArray_T)Seq_remhi($m);
                                        if (to_delete != NULL) {
                                                UArray_free(&to_delete);
                                        }
                                }
                                while (Seq_length(unmapped) != 0) {
                                        uint32_t *to_delete = (uint32_t*)Seq_remhi(unmapped);
                                        if (to_delete != NULL) {
                                                free(to_delete);
                                        }
                                }
                                Seq_free(&$m);
                                Seq_free(&unmapped);
                                fclose(fp);
                                halt();
                                break;     
                        case 8: map_segment(&$r, $m, b, c, unmapped);
                                break;     
                        case 9: unmap_segment(&$r, $m, c, unmapped);
                                break;     
                        case 10: output(&$r, c);
                                 break;     
                        case 11: input(&$r, c);
                                 break;     
                        case 12: load_program(&$r, $m, b);
                                 i = (uint32_t)$r[c];
                                 max = (uint32_t)UArray_length((UArray_T)Seq_get($m, 0));
                                 program_counter = (uint32_t*) UArray_at( (UArray_T) Seq_get($m,0), i);
                                 break;     
                        case 13: load_value(&$r, a, value);
                                 break;              
                }

        }

        while(Seq_length($m) != 0) {
                UArray_T to_delete = (UArray_T)Seq_remhi($m);
                if (to_delete != NULL) {
                        UArray_free(&to_delete);
                }
        }
        while (Seq_length(unmapped) != 0) {
                uint32_t *to_delete = (uint32_t*)Seq_remhi(unmapped);
                        if (to_delete != NULL) {
                                free(to_delete);
                        }
                }
        Seq_free(&$m);
        Seq_free(&unmapped);
        fclose(fp);
        
        return EXIT_SUCCESS;
}


void conditional_move(uint32_t (*$r)[], short a, short b, short c)
{
        if ( (*$r)[c] != 0) {
                (*$r)[a] = (uint32_t)(*$r)[b];
        }
}

void segmented_load(uint32_t (*$r)[], Seq_T $m, short a, short b, short c)
{
        UArray_T segment = (UArray_T) Seq_get($m, (*$r)[b]);
        uint32_t index = (*$r)[c];

        uint32_t *word = (uint32_t *) (uintptr_t)UArray_at(segment,index);
        (*$r)[a] = *word;          
}

void segmented_store(uint32_t (*$r)[], Seq_T $m, short a, short b, short c)
{
        UArray_T segment = (UArray_T) Seq_get($m, (*$r)[a]);
        uint32_t index = (uint32_t)(*$r)[b];
       
        uint32_t *word = (uint32_t *) (uintptr_t)UArray_at(segment, index);
        *word = (uint32_t)(*$r)[c]; 
}

void addition(uint32_t (*$r)[], short a, short b, short c)
{ 
        (*$r)[a] = (uint32_t)( (uint32_t)(*$r)[b] + (uint32_t)(*$r)[c] );
}

void multiplication(uint32_t (*$r)[], short a, short b, short c)
{
        (*$r)[a] = (uint32_t)( (uint32_t)(*$r)[b] * (uint32_t)(*$r)[c] ); 
}

void division(uint32_t (*$r)[], short a, short b, short c)
{
        (*$r)[a] = (uint32_t) ((uint32_t)(*$r)[b]) / (uint32_t)(*$r)[c]; 
}

void bitwise_nand(uint32_t (*$r)[], short a, short b, short c)
{
        (*$r)[a] = (uint32_t) ~( (uint32_t)(*$r)[b] & (uint32_t)(*$r)[c] );
}

void halt()
{
        exit(0);
}

void map_segment(uint32_t (*$r)[], Seq_T $m, short b, short c, Seq_T unmapped) 
{       
        uint32_t length = (uint32_t)(*$r)[c];
        UArray_T arr = UArray_new(length, sizeof(uint32_t));

        uint32_t i = 0;
        while (i < length) {
                uint32_t *value = (uint32_t*)(uintptr_t)UArray_at(arr, i++);
                *value = 0;
        }

        if (Seq_length(unmapped) > 0) {
                uint32_t *index = (uint32_t*)(uintptr_t)Seq_remhi(unmapped);
                UArray_T to_delete = Seq_put($m, *index, arr);
                (*$r)[b] = *index;
                free(index);
                if (to_delete != NULL) {
                        UArray_free(&to_delete);
                }
                
        } else {
                Seq_addhi($m, arr);
                (*$r)[b] = (uint32_t)(Seq_length($m) - 1);
        }          
}

void unmap_segment(uint32_t (*$r)[], Seq_T $m, short c, Seq_T unmapped)
{
        uint32_t *index = malloc(sizeof(uint32_t));
        *index = (uint32_t)(*$r)[c];
        Seq_addhi(unmapped, (uint32_t *)(uintptr_t)index);

        UArray_T to_unmap = (UArray_T) Seq_put($m, (*$r)[c], NULL);
        UArray_free(&to_unmap);
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
                (*$r)[c] = (uint32_t)extracted_byte;
        }
}

void load_program(uint32_t (*$r)[], Seq_T $m, short b)
{ 
        if((*$r)[b] != 0) {
                UArray_T to_copy = (UArray_T)Seq_get($m, (*$r)[b]);
                UArray_T duplicate = UArray_copy(to_copy, UArray_length(to_copy));
                UArray_T to_delete = Seq_put($m, 0, duplicate);
                UArray_free(&to_delete);
        }           
}

void load_value(uint32_t (*$r)[], short a, uint32_t value)
{
        value = value << 7;
        value = value >> 7;
        (*$r)[a] = (uint32_t)value;
}
