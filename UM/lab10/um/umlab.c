/*
 * umlab.c
 * 
 * The functions defined in this lab should be linked against umlabwrite.c
 * to produce a unit test writing program. Any additional functions and unit
 * tests written for the lab go here.
 */

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <bitpack.h>

#include <assert.h>
#include <seq.h>


typedef uint32_t Um_instruction;
typedef enum Um_opcode {
        CMOV = 0, SLOAD, SSTORE, ADD, MUL, DIV,
        NAND, HALT, ACTIVATE, INACTIVATE, OUT, IN, LOADP, LV
} Um_opcode;


/* Functions that return the two instruction types */

Um_instruction three_register(Um_opcode op, int ra, int rb, int rc);
Um_instruction loadval(Um_instruction ra, unsigned val);
Um_instruction input(Um_instruction ra);
Um_instruction map(Um_instruction rb, Um_instruction rc);
Um_instruction output(Um_instruction c);


/* Wrapper functions for each of the instructions */

static inline Um_instruction halt(void) 
{
        return three_register(HALT, 0, 0, 0);
}

typedef enum Um_register { r0 = 0, r1, r2, r3, r4, r5, r6, r7 } Um_register;

Um_instruction add(Um_register a, Um_register b, Um_register c) 
{
        return three_register(ADD, a, b, c);
}

Um_instruction map(Um_register b, Um_instruction c)
{
        return three_register(ACTIVATE, 0, b, c);
}

Um_instruction unmap(Um_instruction c)
{
        return three_register(INACTIVATE, 0, 0, c);
}

Um_instruction load_program(Um_register b, Um_instruction c)
{
        return three_register(LOADP, 0, b, c);
}

Um_instruction segment_load(Um_instruction a, Um_instruction b, Um_instruction c)
{
        return three_register(SLOAD, a, b, c);
}

Um_instruction segment_store(Um_instruction a, Um_instruction b, Um_instruction c)
{
        return three_register(SSTORE, a, b, c);
}

Um_instruction multiply(Um_instruction a, Um_instruction b, Um_instruction c)
{
        return three_register(MUL, a, b, c);
}

Um_instruction divide(Um_instruction a, Um_instruction b, Um_instruction c)
{
        return three_register(DIV, a, b, c);
}


Um_instruction nand(Um_instruction a, Um_instruction b, Um_instruction c)
{
        return three_register(NAND, a, b, c);
}

Um_instruction cond_mov(Um_instruction a, Um_instruction b, Um_instruction c)
{
        return three_register(CMOV, a, b, c);
}

Um_instruction output(Um_register c)
{
        return three_register(OUT, 0, 0, c);

}

Um_instruction input(Um_register c)
{
        return three_register(IN, 0, 0, c);
}




/* Functions for working with streams */

static inline void emit(Seq_T stream, Um_instruction inst)
{
        assert(sizeof(inst) <= sizeof(uintptr_t));
        Seq_addhi(stream, (void *)(uintptr_t)inst);
}

extern void Um_write_sequence(FILE *output, Seq_T stream);


/* Unit tests for the UM */

void emit_condmov_test(Seq_T stream)
{
        emit(stream, loadval(r1, 1));
        emit(stream, loadval(r2, 3));
        emit(stream, loadval(r3, 0));
        emit(stream, cond_mov(r1, r2, r3));
        emit(stream, output(r1));

        emit(stream, loadval(r1, 1));
        emit(stream, loadval(r2, 3));
        emit(stream, loadval(r3, 1)); 
        emit(stream, cond_mov(r1, r2, r3));
        emit(stream, output(r1)); 

        emit(stream, halt());              
}

void emit_multiply_test(Seq_T stream)
{
        emit(stream, loadval(r2, 5));
        emit(stream, loadval(r3, 6));
        emit(stream, multiply(r1, r2, r3));
        emit(stream, output(r1));

        emit(stream, halt());

}

void emit_divide_test(Seq_T stream)
{
        emit(stream, loadval(r2, 158));
        emit(stream, loadval(r3, 2));
        emit(stream, divide(r1, r2, r3));
        emit(stream, output(r1));

        emit(stream, halt());

}

void emit_divide_by_zero_test(Seq_T stream)
{
        emit(stream, loadval(r2, 40));
        emit(stream, loadval(r3, 0));
        emit(stream, divide(r1, r2, r3));
        emit(stream, output(r1));

        emit(stream, halt());        
}

void emit_nand_test(Seq_T stream)
{
        emit(stream, loadval(r2, 7));
        emit(stream, loadval(r3, 7));
        emit(stream, nand(r1, r2, r3));
        emit(stream, output(r1));

        emit(stream, loadval(r2, 0));
        emit(stream, loadval(r3, 0));
        emit(stream, nand(r1, r2, r3));
        emit(stream, output(r1));

        emit(stream, loadval(r2, 33554431));
        emit(stream, loadval(r3, (33554431 - 1)));
        emit(stream, nand(r1, r2, r3));
        emit(stream, output(r1));

        emit(stream, halt());
}

// Still need to test
void emit_loadprogram_test(Seq_T stream)
{

        emit(stream, loadval(r0, 1));
        emit(stream, loadval(r1, 5));
        emit(stream, map(r0, r1));

        emit(stream, loadval(r0, 0));
        emit(stream, loadval(r1, 1));
        emit(stream, loadval(r2, 2));
        emit(stream, loadval(r3, 3));
        emit(stream, loadval(r4, 4));

        emit(stream, loadval(r6, 28));
        emit(stream, segment_load(r7, r0, r6));
        emit(stream, segment_store(r1, r0, r7)); // put halt  here

        emit(stream, loadval(r6, 29));
        emit(stream, segment_load(r7, r0, r6));
        emit(stream, segment_store(r1, r1, r7)); // put load value 37 to r7 here

        emit(stream, loadval(r6, 30));
        emit(stream, segment_load(r7, r0, r6));
        emit(stream, segment_store(r1, r2, r7)); // put r6 = r7 + r7 here

        emit(stream, loadval(r6, 31));
        emit(stream, segment_load(r7, r0, r6));
        emit(stream, segment_store(r1, r3, r7)); // put output r6 here

        emit(stream, loadval(r6, 32));
        emit(stream, segment_load(r7, r0, r6));
        emit(stream, segment_store(r1, r4, r7)); // put halt here 

        emit(stream, load_program(r1, r1));
 
        emit(stream, loadval(r7, 22));
        emit(stream, add(r6, r7, r7));
        emit(stream, output(r6));
        emit(stream, halt());

        emit(stream, halt());
        emit(stream, loadval(r7, 37));
        emit(stream, add(r6, r7, r7));
        emit(stream, output(r6));
        emit(stream, halt());

        emit(stream, loadval(r7, 24));
        emit(stream, add(r6, r7, r7));
        emit(stream, output(r6));
        emit(stream, halt());

}

void emit_seg_load_test(Seq_T stream)
{
        emit(stream, loadval(r0, 2));
        emit(stream, loadval(r1, 1));
        emit(stream, loadval(r2, 0));
        emit(stream, loadval(r3, 75));
        emit(stream, loadval(r4, 74));
        emit(stream, map(r1, r0));

        emit(stream, segment_store(r1, r2, r3));
        emit(stream, segment_store(r1, r1, r4));
        
        emit(stream, segment_load(r5, r1, r1));
        emit(stream, output(r5));

        emit(stream, halt());

}

void emit_umnap_test(Seq_T stream)
{
        emit(stream, loadval(r0, 2));
        emit(stream, loadval(r1, 1));
        emit(stream, loadval(r2, 0));
        emit(stream, loadval(r3, 75));
        emit(stream, loadval(r4, 74));
        emit(stream, map(r1, r0));

        emit(stream, segment_store(r1, r2, r3));
        emit(stream, segment_store(r1, r1, r4));

        emit(stream, unmap(r1));

        emit(stream, loadval(r0, 2));
        emit(stream, loadval(r1, 1));
        emit(stream, loadval(r2, 0));
        emit(stream, loadval(r3, 75));
        emit(stream, loadval(r4, 74));
        emit(stream, map(r1, r0));

        emit(stream, segment_store(r1, r2, r3));
        emit(stream, segment_store(r1, r1, r4));

        emit(stream, segment_load(r5, r1, r2));
        emit(stream, output(r5));
        emit(stream, segment_load(r5, r1, r1));
        emit(stream, output(r5));

        emit(stream, output(r6));

        // emit(stream, segment_store(r1, r2, r3));
        // emit(stream, loadval(r5, 78));
        // emit(stream, output(r5));

        emit(stream, halt());

}

void emit_halt_test(Seq_T stream)
{
        emit(stream, halt());
}

void emit_verbose_halt_test(Seq_T stream)
{
        emit(stream, halt());
        emit(stream, loadval(r1, 'B'));
        emit(stream, output(r1));
        emit(stream, loadval(r1, 'a'));
        emit(stream, output(r1));
        emit(stream, loadval(r1, 'd'));
        emit(stream, output(r1));
        emit(stream, loadval(r1, '!'));
        emit(stream, output(r1));
        emit(stream, loadval(r1, '\n'));
        emit(stream, output(r1));


}

void emit_add_test(Seq_T stream)
{
        emit(stream, loadval(r2, 73));
        emit(stream, loadval(r3, 74));
        emit(stream, add(r1, r2, r3));
        emit(stream, output(r1));

        emit(stream, halt());

}

void emit_printsix_test(Seq_T stream)
{
        emit(stream, loadval(r1, 48));
        emit(stream, loadval(r2, 6));
        emit(stream, add(r3, r1, r2));
        emit(stream, output(r3));

        emit(stream, halt());
}

void emit_input_and_output_test(Seq_T stream)
{
        emit(stream, input(r1));
        emit(stream, output(r1));

        emit(stream, halt());
}

Um_instruction loadval(unsigned ra, unsigned val)
{
        Um_instruction instruction = 0;

        instruction = Bitpack_newu(instruction, 4, 28, LV);
        instruction = Bitpack_newu(instruction, 3, 25, ra);
        instruction = Bitpack_newu(instruction, 25, 0, val);

        return instruction;
}


Um_instruction three_register(Um_opcode op, int ra, int rb, int rc)
{
        Um_instruction instruction = 0;

        assert(ra < 8 && rb < 8 && rc < 8); 

        instruction = Bitpack_newu(instruction, 4, 28, op);
        instruction = Bitpack_newu(instruction, 3, 6, ra);
        instruction = Bitpack_newu(instruction, 3, 3, rb);
        instruction = Bitpack_newu(instruction, 3, 0, rc);

        return instruction;
}

extern void Um_write_sequence(FILE *output, Seq_T stream)
{

        while(Seq_length(stream)) {

                Um_instruction word = (uintptr_t)Seq_remlo(stream);

                int byte_1 = Bitpack_getu(word, 8, 24);
                int byte_2 = Bitpack_getu(word, 8, 16);
                int byte_3 = Bitpack_getu(word, 8, 8);
                int byte_4 = Bitpack_getu(word, 8, 0);

                putc(byte_1, output);
                putc(byte_2, output);
                putc(byte_3, output);
                putc(byte_4, output);

                fprintf(stderr, "word: %x %x %x %x\n", byte_1, byte_2, byte_3, byte_4);
        }
}


