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
                case 0:
                        /*Store register b value in register a if register c 
                          is nonzero.*/
                        if ( ($r)[c] != 0) {
                                ($r)[a] = (uint32_t)($r)[b];
                        }
                        
                        break;
                case 1: 
                        /*Load instruction from a segment into a 
                          register*/ 
                        {
                        /* Assign word from mapped segment to register a */
                        ($r)[a] = *(uint32_t *) UArray_at((UArray_T) 
                                                          Seq_get($m, ($r)[b]),
                                                          ($r)[c]);
                        
                        break;
                        }
                        case 2: 
                                /*Store value into the word of a register*/
                                {
                                /* Store value from register c into the word of 
                                   a segment */
                                uint32_t *word = (uint32_t *) 
                                        UArray_at((UArray_T) 
                                                  Seq_get($m, ($r)[a]), 
                                                  (uint32_t)($r)[b]);
                                *word = (uint32_t)($r)[c]; 
                                
                                break;
                                }
                        case 3: 
                                /*Add values stored in registers b and c and
                                  store the result to register a*/
                                ($r)[a] = (uint32_t)((uint32_t)($r)[b] 
                                                     + (uint32_t)($r)[c] );
                                break;
                        case 4: 
                                /*Multiply values in registers b and c and
                                  store the result mod 2^32 in register a*/
                                ($r)[a] = (uint32_t)( (uint32_t)($r)[b] 
                                                      * (uint32_t)($r)[c] ); 
                                break;   
                        case 5: 
                                /*Multiply values in registers b and c and
                                  store the result mod 2^32 in register a.*/
                                ($r)[a] = (uint32_t) ((uint32_t)($r)[b]) 
                                        / (uint32_t)($r)[c]; 
                                break;
                        case 6: 
                                /*Perform the bitwise nand on integers stored 
                                  in registers b and c. Stores result into 
                                  register a*/
                                ($r)[a] = (uint32_t) ~( (uint32_t)($r)[b] 
                                                        & (uint32_t)($r)[c] );
                                break;     
                        case 7: 
                                {
                                /* Free memory before halting program */
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
                                
                                fclose(*fp);
                                /* Kills computation. */
                                exit(0);
                                break;     
                                }
                        case 8: 
                                /*Store segment into sequence. Reuses any 
                                  unmapped segments in the sequence before
                                  creating any new sequence segment.*/
                                {                                        
                                        /* Check if there are any unmapped
                                           segments */
                                        if (Seq_length(unmapped) > 0) {
                                                /* Store new segment into
                                                   unmapped segment index */
                                                uint32_t *index = 
                                                        (uint32_t*)
                                                        (uintptr_t)
                                                        Seq_remhi(unmapped);
                                                UArray_T to_delete = 
                                                        Seq_put($m, *index,
                                                                UArray_new
                                                                ((uint32_t)
                                                                 ($r)[c], 
                                                                 sizeof
                                                                 (uint32_t)));
                                                ($r)[b] = *index;
                                                
                                                /* Free segment identifier 
                                                   in unmapped stack */
                                                free(index);
                                                if (to_delete != NULL) {
                                                        /* Free array of words
                                                           in the segments 
                                                           identifier */
                                                        UArray_free(&to_delete);
                                                }                
                                        } else {
                                                /* If no unmapped segment,
                                                   create new segment position */
                                                Seq_addhi($m, 
                                                          UArray_new
                                                          ((uint32_t)($r)[c],
                                                           sizeof(uint32_t)));
                                                ($r)[b] = 
                                                        (uint32_t)
                                                        (Seq_length($m) - 1);
                                        }          
                                        
                                        break;   
                                }
                        case 9: 
                                /*Remove segment from mapped sequence and
                                  place the index of the unmapped segments
                                  into the unmapped stack/sequence.*/
                                {
                                /* Add segment identifier into umapped stack */
                                uint32_t *index = malloc(sizeof(uint32_t));

                                *index = (uint32_t)($r)[c];
                                Seq_addhi(unmapped, (void *)(uintptr_t)index);
                                
                                /* Free recently unmapped segment */
                                UArray_T to_unmap = (UArray_T) 
                                        Seq_put($m, ($r)[c], NULL);
                                UArray_free(&to_unmap);
                                
                                break;     
                                }
                        case 10: 
                                /*Output value in register c*/
                                putchar(($r)[c]);
                                break;     
                        case 11: 
                                /*Read standard input into register c.
                                  If EOF, store all 1's.*/
                                {
                                        /* Read byte from standard input */
                                        char extracted_byte = getchar();
                                        
                                        /* Check if EOF */
                                        if (extracted_byte == EOF) {
                                                /* Store all 1s in 
                                                   register c if EOF */
                                                ($r)[c] = ~0;
                                        } else {
                                                /* Store standard input value 
                                                   into register c */
                                                ($r)[c] = 
                                                        (uint32_t)
                                                        extracted_byte;
                                        }
                                        
                                        break;  
                                }
                        case 12: 
                                /*Loads next instruction to be executed.*/
                                if(($r)[b] != 0) {
                                        /* If value in register b is not 0 make
                                           a hard copy of segment identified 
                                           by the identifier stored in the 
                                           register b */
                                        UArray_T to_copy = 
                                                (UArray_T)Seq_get($m, ($r)[b]);
                                        UArray_T duplicate = 
                                                UArray_copy
                                                (to_copy, 
                                                 UArray_length(to_copy));

                                        /* Free segment previously stored in
                                           the 0th segment */
                                        UArray_T to_delete = 
                                                Seq_put($m, 0, duplicate);
                                        UArray_free(&to_delete);
                                }           
                                i = (uint32_t)$r[c];
                                /* Get number of instructions stored in new 
                                   segment */
                                max = (uint32_t)UArray_length((UArray_T)
                                                              Seq_get($m, 0));
                                /* Get next instruction to be executed */
                                program_counter = 
                                        (uint32_t*) UArray_at( 
                                                              (UArray_T)
                                                              Seq_get($m,0), i);
                                break;     
                        case 13: 
                                /*Load value into register a.*/
                                ($r)[a] = (uint32_t)value;
                                break;              
                }
        }       
}

