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
#include "instructions.h"


/*****************************************************************************
 Function: conditional_move
 Purpose: Store register b value in register a if register c is nonzero.
 Parameters: uint32_t (*$r)[], short a, short b, short c
 Return: void
*****************************************************************************/
void conditional_move(uint32_t (*$r)[], short a, short b, short c)
{
        if ( (*$r)[c] != 0) {
                (*$r)[a] = (uint32_t)(*$r)[b];
        }
}

/*****************************************************************************
 Function: segmented_load
 Purpose: Load instruction from a segment into a register
 Parameters: uint32_t (*$r)[], Seq_T $m, short a, short b, short c
 Return: void
*****************************************************************************/
void segmented_load(uint32_t (*$r)[], Seq_T $m, short a, short b, short c)
{
        /* Get segment from mapped sequence */
        UArray_T segment = (UArray_T) Seq_get($m, (*$r)[b]);
        uint32_t index = (*$r)[c];

        /* Assign word from mapped segment to register a */
        uint32_t *word = (uint32_t *) UArray_at(segment,index);
        (*$r)[a] = *word;          
}
/*****************************************************************************
 Function: segmented_store
 Purpose: Store value into the word of a register
 Parameters: uint32_t (*$r)[], Seq_T $m, short a, short b, short c
 Return: void
*****************************************************************************/
void segmented_store(uint32_t (*$r)[], Seq_T $m, short a, short b, short c)
{
        /* Get segment from sequence */
        UArray_T segment = (UArray_T) Seq_get($m, (*$r)[a]);
        uint32_t index = (uint32_t)(*$r)[b];
        
        /* Store value from register c into the word of a segment */
        uint32_t *word = (uint32_t *) UArray_at(segment, index);
        *word = (uint32_t)(*$r)[c]; 
}

/*****************************************************************************
 Function: addition
 Purpose: Add values stored in registers b and c and store the result to 
 register a.
 Parameters: uint32_t (*$r)[], short a, short b, short c
 Return: void
*****************************************************************************/
void addition(uint32_t (*$r)[], short a, short b, short c)
{ 
        (*$r)[a] = (uint32_t)( (uint32_t)(*$r)[b] + (uint32_t)(*$r)[c] );
}

/*****************************************************************************
 Function: multiplication
 Purpose: Multiply values in registers b and c and store the result mod 2^32 
 in register a.
 Parameters: uint32_t (*$r)[], short a, short b, short c
 Return: void
*****************************************************************************/
void multiplication(uint32_t (*$r)[], short a, short b, short c)
{
        (*$r)[a] = (uint32_t)( (uint32_t)(*$r)[b] * (uint32_t)(*$r)[c] ); 
}

/*****************************************************************************
 Function: division
 Purpose: Divide integers in registers b and c and store floored result in
 register a.
 Parameters: uint32_t (*$r)[], Seq_T $m, short a, short b, short c
 Return: void
*****************************************************************************/
void division(uint32_t (*$r)[], short a, short b, short c)
{
        (*$r)[a] = (uint32_t) ((uint32_t)(*$r)[b]) / (uint32_t)(*$r)[c]; 
}

/*****************************************************************************
 Function: bitwise_nand
 Purpose: Perform the bitwise nand on integers stored in registers b and c. 
 Stores result into register a
 Parameters: uint32_t (*$r)[], Seq_T $m, short a, short b, short c
 Return: void
*****************************************************************************/
void bitwise_nand(uint32_t (*$r)[], short a, short b, short c)
{
        (*$r)[a] = (uint32_t) ~( (uint32_t)(*$r)[b] & (uint32_t)(*$r)[c] );
}

/*****************************************************************************
 Function: halt
 Purpose: Kills computation.
 Parameters: 
 Return: void
*****************************************************************************/
void halt()
{
        exit(0);
}

/*****************************************************************************
 Function: map_segment
 Purpose: Store segment into sequence. Reuses any unmapped segments in the
 sequence before creating any new sequence segment.
 Parameters: uint32_t (*$r)[], Seq_T $m, short b, short c, Seq_T unmapped
 Return: void
*****************************************************************************/
void map_segment(uint32_t (*$r)[], Seq_T $m, short b, short c, Seq_T unmapped) 
{       
        /* Create new segment with numer of words equal to the value in
           register c */
        uint32_t length = (uint32_t)(*$r)[c];
        UArray_T arr = UArray_new(length, sizeof(uint32_t));
        assert(arr != NULL);

        /* Check if there are any unmapped segments */
        if (Seq_length(unmapped) > 0) {
                /* Store new segment into unmapped segment index */
                uint32_t *index = (uint32_t*)(uintptr_t)Seq_remhi(unmapped);
                UArray_T to_delete = Seq_put($m, *index, arr);
                (*$r)[b] = *index;
                
                /* Free segment identifier in unmapped stack */
                free(index);
                if (to_delete != NULL) {
                        /* Free array of words in the segments identifier */
                        UArray_free(&to_delete);
                }                
        } else {
                /* If no unmapped segment, create new segment position */
                Seq_addhi($m, arr);
                (*$r)[b] = (uint32_t)(Seq_length($m) - 1);
        }          
}

/*****************************************************************************
 Function: unmap_segment
 Purpose: Remove segment from mapped sequence and place the index of the 
 unmapped segments into the unmapped stack/sequence.
 Parameters: uint32_t (*$r)[], Seq_T $m, short c, Seq_T unmapped
 Return: void
*****************************************************************************/
void unmap_segment(uint32_t (*$r)[], Seq_T $m, short c, Seq_T unmapped)
{
        /* Add segment identifier into umapped stack */
        uint32_t *index = malloc(sizeof(uint32_t));
        assert(index != NULL);
        *index = (uint32_t)(*$r)[c];
        Seq_addhi(unmapped, (void *)(uintptr_t)index);

        /* Free recently unmapped segment */
        UArray_T to_unmap = (UArray_T) Seq_put($m, (*$r)[c], NULL);
        UArray_free(&to_unmap);
}

/*****************************************************************************
 Function: output
 Purpose: Output value in register c
 Parameters: uint32_t (*$r)[], short c
 Return: void
*****************************************************************************/
void output(uint32_t (*$r)[], short c)
{
        putchar((*$r)[c]);
}

/*****************************************************************************
 Function: input
 Purpose: Read standard input into register c. If EOF, store all 1's.
 Parameters: uint32_t (*$r)[], short c
 Return: void
*****************************************************************************/
void input(uint32_t (*$r)[], short c)
{
        /* Read byte from standard input */
        char extracted_byte = getchar();

        /* Check if EOF */
        if (extracted_byte == EOF) {
                /* Store all 1s in register c if EOF */
                (*$r)[c] = ~0;
        } else {
                /* Store standard input value into register c */
                (*$r)[c] = (uint32_t)extracted_byte;
        }
}

/*****************************************************************************
 Function: load_program
 Purpose: Loads next instruction to be executed.
 Parameters: uint32_t (*$r)[], Seq_T $m, short b
 Return: void
*****************************************************************************/
void load_program(uint32_t (*$r)[], Seq_T $m, short b)
{ 
        if((*$r)[b] != 0) {
                /* If value in register b is not 0 make a hard copy of segment
                   identified by the identifier stored in the register b */
                UArray_T to_copy = (UArray_T)Seq_get($m, (*$r)[b]);
                UArray_T duplicate = UArray_copy(to_copy, 
                                                 UArray_length(to_copy));
                assert(duplicate != NULL);
                /* Free segment previously stored in the 0th segment */
                UArray_T to_delete = Seq_put($m, 0, duplicate);
                UArray_free(&to_delete);
        }           
}


/*****************************************************************************
 Function: load_value
 Purpose: Load value into register a.
 Parameters: uint32_t (*$r)[], short a, uint32_t value
 Return: void
*****************************************************************************/
void load_value(uint32_t (*$r)[], short a, uint32_t value)
{
        (*$r)[a] = (uint32_t)value;
}

/*****************************************************************************
 Function: execute_instructions
 Purpose: iterates through the 0th segment and executes the instructions
 successively, and update program pointer as required by instructions.
 Parameters: FILE** fp, Seq_T $m, Seq_T unmapped
 Return: void
*****************************************************************************/
void execute_instructions(FILE** fp, Seq_T $m, Seq_T unmapped)
{
        /* Initialize registers, op_code, program counter, and max */
        uint32_t $r[8] = { 0 };
        uint32_t *program_counter = 0;  // Program Counter
        program_counter = (uint32_t *)UArray_at((UArray_T)Seq_get($m, 0), 0);
        short op_code = 0;
        uint32_t i = 0;
        /* Number of instructions in the 0th segment */
        uint32_t max = (uint32_t)UArray_length((UArray_T)Seq_get($m, 0));
        uint32_t opcode_mask = 0xF0000000;
        uint32_t a_mask = 0x1C0;
        uint32_t b_mask = 0x38;
        uint32_t c_mask = 0x7;
        uint32_t loadval_a_mask = 0xE000000;
        uint32_t loadval_val_mask = 0x1FFFFFF;
        uint32_t opcode_shift = 28;
        uint32_t a_shift = 6;
        uint32_t b_shift = 3;
        uint32_t loadval_a_shift = 25;

        /* Iterate until the last instruction is executed */
        while (i < max) {

                /* Get instruction to be executed */
                uint32_t word = *program_counter;

                if ( (i + 1) < max ) {
                        /* Set program counter to next instruction to be 
                        executed */
                        program_counter += 1;
                } else {
                        i++;
                }
                
                /* Get the instruction from the word */
                op_code = (word & opcode_mask) >> opcode_shift;

                short a = 0;
                short b = 0;
                short c = 0;
                uint32_t value = 0;

                /* Get the registers of the word depending on the instruction*/
                if (op_code != 13 ) {
                        a = (word & a_mask) >> a_shift;
                        b = (word & b_mask) >> b_shift;
                        c = word & c_mask;
                } else if (op_code == 13) {
                        a = (word & loadval_a_mask) >> loadval_a_shift;
                        value = word & loadval_val_mask;
                }
                assert(op_code >= 0 && op_code < 14);

                /* Switch case executes the instruction store in the word */
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
                                /* Free memory before halting program */
                                free_instructions($m, unmapped);
                                fclose(*fp);
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
                                 /* Get number of instructions stored in new 
                                 segment */
                                 max = (uint32_t)UArray_length((UArray_T)
                                        Seq_get($m, 0));
                                 /* Get next instruction to be executed */
                                 program_counter = (uint32_t*) UArray_at( 
                                        (UArray_T) Seq_get($m,0), i);
                                 break;     
                        case 13: load_value(&$r, a, value);
                                 break;              
                }
        }
}

/*****************************************************************************
 Function: free_instructions
 Purpose: Free all segments in mapped sequence, and free all segment 
 identifiers in the unmapped stack.
 Parameters: Seq_T $m, Seq_T unmapped
 Return: void
*****************************************************************************/
void free_instructions(Seq_T $m, Seq_T unmapped)
{
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

}

