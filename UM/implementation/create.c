#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <bitpack.h>
#include <stdint.h>

#include "assert.h"
#include "fmt.h"
#include "seq.h"

void make_word(uint32_t *word, short op_code, short a, short b, short c);
void load_val(short a, short val);

int main (int argc, char *argv[])
{       
        // 0 Conditional Move
        make_word(0, 0, 1, 2);

        // 1 Segmented Load
        make_word(1, 0, 1, 2);

        // 2 Segmented Store
        make_word(2, 0, 1, 2);

        // 3 Addition
        make_word(3, 0, 1, 2);   

        // 4 Multiplication
        make_word(4, 0, 1, 2);  

        // 5 Division
        make_word(5, 0, 1, 2);  

        // 6 Bitwise NAND
        make_word(6, 0, 1, 2);  

        // 7 Halt
        make_word(7, 0, 1, 2);  

        // 8 Map Segment
        make_word(8, 0, 1, 2);  

        // 9 Unmap Segment
        make_word(9, 0, 1, 2);  

        // 10 Output
        make_word(10, 0, 1, 2);  

        // 11 Input
        make_word(11, 0, 1, 2);  

        // 12 Load Program
        make_word(12, 0, 1, 2);  

        // 13 Load Value
        load_val(0, 500)
        Um_instruction instruction = 0;

}


void make_word(short op_code, short a, short b, short c)
{
        uint32_t word = 0;

        word = Bitpack_newu(word, 4, 28, op_code);
        word = Bitpack_newu(word, 3, 6, a);
        word = Bitpack_newu(word, 3, 3, b);
        word = Bitpack_newu(word, 3, 0, c);   

        uint32_t byte_1 = Bitpack_getu(word, 8, 24);
        uint32_t byte_2 = Bitpack_getu(word, 8, 16);
        uint32_t byte_3 = Bitpack_getu(word, 8, 8);
        uint32_t byte_4 = Bitpack_getu(word, 8, 0);

        putchar(byte_1);
        putchar(byte_2);
        putchar(byte_3);
        putchar(byte_4);     
}

void load_val(short a, short val)
{
        Um_instruction word = 0;
        word = Bitpack_newu(word, 4, 28, 13);   // Op code
        word = Bitpack_newu(word, 3, 25, a);   
        word = Bitpack_newu(word, 25, 0, val); 

        uint32_t byte_1 = Bitpack_getu(word, 8, 24);
        uint32_t byte_2 = Bitpack_getu(word, 8, 16);
        uint32_t byte_3 = Bitpack_getu(word, 8, 8);
        uint32_t byte_4 = Bitpack_getu(word, 8, 0);

        putchar(byte_1);
        putchar(byte_2);
        putchar(byte_3);
        putchar(byte_4);  
}