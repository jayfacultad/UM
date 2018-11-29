#include <stdlib.h>
#include <stdio.h>

#include "uarray.h"
#include "seq.h"
#include "instructions.h"
#include "assert.h"

#include "seg_zero.h"

int main (int argc, char *argv[])
{
        
        FILE* fp = NULL;
        /* Sequence of Seqments */
        Seq_T $m = Seq_new(1000);  
        assert($m != NULL);   
        /* Stack of unmapped Seqments */
        Seq_T unmapped = Seq_new(100);
        assert(unmapped != NULL);

        /* Read from file and populate the 0th segments in $m with 
        instructions */
        get_instructions(&fp, $m, argc, argv);
        /* Execute instructions in sequence $m */
        execute_instructions(&fp, $m, unmapped);

        /* Free all memory used for instructions */
        while(Seq_length($m) != 0) {
                UArray_T to_delete = 
                        (UArray_T)Seq_remhi($m);
                if (to_delete != NULL) {
                        UArray_free(&to_delete);
                }
        }
        while (Seq_length(unmapped) != 0) {
                uint32_t *to_delete = 
                        (uint32_t*)Seq_remhi(unmapped);
                if (to_delete != NULL) {
                        free(to_delete);
                }
        }
        
        Seq_free(&$m);
        Seq_free(&unmapped);
        
        /* Kills computation. */
        
        fclose(fp);
        
        return EXIT_SUCCESS;
}


