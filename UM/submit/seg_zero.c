#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <inttypes.h>
#include <bitpack.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <assert.h>

#include "uarray.h"
#include "seq.h"
#include "seg_zero.h"

const int byte_len = 8;

/*****************************************************************************
 Function: get_instructions
 Purpose: Opens file and reads instructions into 0th Segment.
 Parameters: FILE ** fp, Seq_T $m, int argc, char *argv[]
 Return: void
*****************************************************************************/
void get_instructions(FILE ** fp, Seq_T $m, int argc, char *argv[])
{
        /* Check for arguments and open if filename provided is valid  */
        assert(argc == 2);
        *fp = fopen(argv[1], "r");
        assert(*fp != NULL);
        
        /* Calculate file size and number of words */
        fileno(*fp);
        struct stat st;
        stat(argv[1], &st);
        double file_size = st.st_size;
        uint32_t num_words = file_size / (sizeof(uint32_t)); 
        
        /* Determine correct number of complete words */
        uint32_t num_bytes = num_words * 4;
        if (num_bytes != (uint32_t)file_size) {
                fprintf(stderr, "Incomplete word\n");
                exit(EXIT_FAILURE);
        }
        
        /* Create and load Seqment 0 with every instruction */
        UArray_T arr = UArray_new(num_words, sizeof(uint32_t));

        load_instructions(fp, arr, $m, num_words);
}

/*****************************************************************************
 Function: load_instructions
 Purpose: Load instructions from file into 0th Segment.
 Parameters: FILE** fp, UArray_T arr, Seq_T $m, uint32_t num_words
 Return: void
*****************************************************************************/
void load_instructions(FILE** fp, UArray_T arr, Seq_T $m, uint32_t num_words) 
{

        uint32_t offset = 0;
        /* Iterate over all instructions */
        
        for (uint32_t i = 0; i < num_words; i++) {
                uint32_t word = 0;
                /* Read full instruction from file */
                for (uint32_t j = 0; j < 4; j++) { 
                        /* Pack bits into word to store full instruction */
                        char extract_byte = getc(*fp);
                        word = Bitpack_news(word, byte_len, 24 - (byte_len  * 
                                            j), extract_byte);             
                }
                /* Store word into array */
                uint32_t *value = UArray_at(arr, offset++);
                *value = (uint32_t)word;
        }
        /* Create the 0th segment */
        Seq_addhi($m, arr);  
}

