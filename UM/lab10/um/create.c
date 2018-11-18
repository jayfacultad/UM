#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <bitpack.h>
#include <stdint.h>

#include "assert.h"
#include "fmt.h"
#include "seq.h"


int main (int argc, char *argv[])
{
        uint32_t word = 0;

        word = Bitpack_newu(word, 4, 28, op);
        word = Bitpack_newu(word, 3, 6, ra);
        word = Bitpack_newu(word, 3, 3, rb);
        word = Bitpack_newu(word, 3, 0, rc);

        uint32_t byte_1 = Bitpack_getu(word, 8, 24);
        uint32_t byte_2 = Bitpack_getu(word, 8, 16);
        uint32_t byte_3 = Bitpack_getu(word, 8, 8);
        uint32_t byte_4 = Bitpack_getu(word, 8, 0);

        putchar(byte_1);
        putchar(byte_2);
        putchar(byte_3);
        putchar(byte_4);

}