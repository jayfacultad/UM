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


int main (int argc, char *argv[])
{
        FILE * fp;
        uint32_t $r[8];                      // Registers
        Seg_T $m = Seg_new(1000);       // Segments
        Seg_T stack = Seg_new(1);       // Stack of unmapped segments

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
        int num_words = file_size / sizeof(uint32_t);
        // fprintf(stderr, "%d\n", num_words);
        // int file_size = fstat(fd, st);

        // Create and load Segment 0 with all of the instructions
        uint32_t arr[num_words];
        Seg_map($m, stack, arr);        
        int offset = 0;
        while (feof(fp) == 0) {
                uint32_t word = 0;
                for (int i = 0; i < 4; i++) {
                        uint8_t extract_byte = (uint8_t)getc(fp);
                        word = Bitpack_news(word, byte_len, (24 - (byte_len  * i)), extract_byte);
                        fprintf(stderr, "%c\n", extract_byte);
                        fprintf(stderr, "%c\n", word);
                }
                arr[offset++] = word;
        }
        Seg_put($m, 0, arr);

        /* Testing extraction of value in segments structure */
        uint32_t (*temp)[num_words] = Seg_get($m, 0);
        printf("%" PRIu32"\n", *temp[0]);
        printf("\n");
        (void)temp;
        (void)$r;
        (void)fd;
}