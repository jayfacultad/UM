#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <inttypes.h>
#include "segments.h"
#include <bitpack.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#define Seq_T Seg_T

const int byte_len = 8;

// void Bitpack_printu(uint32_t word)
// {
//         uint32_t mask = Bitpack_mask(1, TYPE_SIZE - 1);
//         for(int i - TYPE_SIZE; i >o; i--) {


//         }
// }

void conditional_move(uint32_t (*$r)[], short a, short b, short c);
void segmented_load(uint32_t (*$r)[], Seg_T *$m, int num_words, short a, short b, short c);
void segmented_store(uint32_t (*$r)[], Seg_T *$m, int num_words, short a, short b, short c);
void addition(uint32_t (*$r)[], short a, short b, short c);
void multiplication(uint32_t (*$r)[], short a, short b, short c);
void division(uint32_t (*$r)[], short a, short b, short c);
void bitwise_nand(uint32_t (*$r)[], short a, short b, short c);
void halt(uint32_t (*$r)[], short a, short b, short c);
void map_segment(uint32_t (*$r)[], Seg_T *$m, Seg_T *stack, int num_words, short a, short b, short c);
void umap_segment(uint32_t (*$r)[], Seg_T *$m, Seg_T *stack, int num_words, short a, short b, short c);
void output(uint32_t (*$r)[], short a, short b, short c);
void input(uint32_t (*$r)[], short a, short b, short c);
void load_program(uint32_t (*$r)[], Seg_T *$m, int num_words, short a, short b, short c);
void load_value(uint32_t (*$r)[], short a);


int main (int argc, char *argv[])
{
        FILE * fp;
        uint32_t $r[8];                 // Registers
        Seg_T $m = Seg_new(1000);       // Sequence of segments
        Seg_T stack = Seg_new(1);       // Stack of unmapped segments
        uint32_t * program_counter;     // Program Counter

        if (argc == 2) {
                fp = fopen(argv[1], "r");
        }
        else {
                fprintf(stderr, "Need one file for input");
        }

        // Determine size of file
        int fd = fileno(fp);
        struct stat st;
        stat(argv[1], &st);
        int file_size = st.st_size;
        int num_words = file_size / (sizeof(uint32_t)); 
        fprintf(stderr, "File size: %d\n", file_size);
        fprintf(stderr, "Number of words: %d\n", num_words);

        // Create and load Segment 0 with every instruction
        uint32_t arr[num_words];
        Seg_map($m, stack, arr);        
        int offset = 0;
        for (int i = 0; i < num_words; i++) {
                uint32_t word = 0;
                for (int j = 0; j < 4; j++) { 
                        char extract_byte = (uint8_t)getc(fp);
                        word = Bitpack_news(word, byte_len, (24 - (byte_len  * i)), extract_byte);             
                }
                printf("%x\n", word);
                arr[offset++] = word;
        }
        Seg_put($m, 0, arr);

        /* Testing extraction of value in segments structure */
        // uint32_t (*temp)[num_words] = Seg_get($m, 0);
        // printf("%x\n", (*temp)[0]);
        // printf("%x\n", (*temp)[1]);
        // printf("\n");

        program_counter = Seg_get($m, 0);

        do {
                uint32_t instruction = *program_counter;

                // Extract op code
                short op_code = Bitpack_getu(instruction, 4, 28);
                short a = Bitpack_getu(instruction, 3, 6);
                short b = Bitpack_getu(instruction, 3, 3);
                short c = Bitpack_getu(instruction, 3, 0);

                switch (op_code) {
                        case 0: conditional_move(&$r, a, b, c);
                                break;
                        case 1: segmented_load(&$r, &$m, num_words, a, b, c);
                                break;
                        case 2: segmented_store(&$r, &$m, num_words, a, b, c);
                                break;
                        case 3: addition(&$r, a, b, c);
                                break;
                        case 4: multiplication(&$r, a, b, c);
                                break;   
                        case 5: division(&$r, a, b, c);
                                break;
                        case 6: bitwise_nand(&$r, a, b, c);
                                break;     
                        case 7: halt(&$r, a, b, c);
                                break;     
                        case 8: map_segment(&$r, &$m, &stack, num_words, a, b, c);
                                break;     
                        case 9: umap_segment(&$r, &$m, &stack, num_words, a, b, c);
                                break;     
                        case 10: output(&$r, a, b, c);
                                break;     
                        case 11: input(&$r, a, b, c);
                                break;     
                        case 12: load_program(&$r, &$m, num_words, a, b, c);
                                break;     
                        case 13: load_value(&$r, a);
                                break;              
                        default: fprintf(stderr, "Error: Unknown Instruction.\n");
                                return EXIT_FAILURE;
                }

                program_counter += byte_len;

        } while (1);

        (void)$r;
        (void)fd;
}


void conditional_move(uint32_t (*$r)[], short a, short b, short c)
{
        if ((*$r)[c] != 0) {
                (*$r)[a] = (*$r)[b];
        }
}

void segmented_load(uint32_t (*$r)[], Seg_T *$m, int num_words, short a, short b, short c)
{
        int32_t (*temp)[num_words] = Seg_get(*$m, (*$r)[b]);
        (*$r)[a] = (*temp)[c];
}

void segmented_store(uint32_t (*$r)[], Seg_T *$m, int num_words, short a, short b, short c)
{
        int32_t (*temp)[num_words] = Seg_get(*$m, (*$r)[a]);
        int index = (*$r)[b];
        (*temp)[index] = (*$r)[c]; // TO DO: check if this works
}

void addition(uint32_t (*$r)[], short a, short b, short c)
{
        (void)$r;
        (void)a;
        (void)b;
        (void)c;
}

void multiplication(uint32_t (*$r)[], short a, short b, short c)
{
        (void)$r;
        (void)a;
        (void)b;
        (void)c;
}

void division(uint32_t (*$r)[], short a, short b, short c)
{
        (void)$r;
        (void)a;
        (void)b;
        (void)c;
}

void bitwise_nand(uint32_t (*$r)[], short a, short b, short c)
{
        (void)$r;
        (void)a;
        (void)b;
        (void)c;
}

void halt(uint32_t (*$r)[], short a, short b, short c)
{
        (void)$r;
        (void)a;
        (void)b;
        (void)c;
}

void map_segment(uint32_t (*$r)[], Seg_T *$m, Seg_T *stack, int num_words, short a, short b, short c) 
{
        (void)$r;
        (void)$m;
        (void)stack;
        (void)num_words;
        (void)a;
        (void)b;
        (void)c;
}

void umap_segment(uint32_t (*$r)[], Seg_T *$m, Seg_T *stack, int num_words, short a, short b, short c)
{
        (void)$r;
        (void)$m;
        (void)stack;
        (void)num_words;
        (void)a;
        (void)b;
        (void)c;
}

void output(uint32_t (*$r)[], short a, short b, short c)
{
        (void)$r;
        (void)a;
        (void)b;
        (void)c;
}

void input(uint32_t (*$r)[], short a, short b, short c)
{
        (void)$r;
        (void)a;
        (void)b;
        (void)c;
}

void load_program(uint32_t (*$r)[], Seg_T *$m, int num_words, short a, short b, short c)
{
        (void)$r;
        (void)$m;
        (void)num_words;
        (void)a;
        (void)b;
        (void)c;
}

void load_value(uint32_t (*$r)[], short a)
{
        (void)$r;
        (void)a;
}


