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
        free_instructions($m, unmapped);
        
        fclose(fp);
        
        return EXIT_SUCCESS;
}


