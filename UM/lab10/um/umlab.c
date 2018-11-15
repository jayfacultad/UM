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
Um_instruction loadval(unsigned ra, unsigned val);


/* Wrapper functions for each of the instructions */

static inline Um_instruction halt(void) 
{
        return three_register(HALT, 0, 0, 0);
}

typedef enum Um_register { r0 = 0, r1, r2, r3, r4, r5, r6, r7 } Um_register;

static inline Um_instruction add(Um_register a, Um_register b, Um_register c) 
{
        return three_register(ADD, a, b, c);
}

Um_instruction output(Um_register c);

/* Functions for working with streams */

static inline void emit(Seq_T stream, Um_instruction inst)
{
        assert(sizeof(inst) <= sizeof(uintptr_t));
        Seq_addhi(stream, (void *)(uintptr_t)inst);
}

extern void Um_write_sequence(FILE *output, Seq_T stream);


/* Unit tests for the UM */

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
        emit(stream, add(r1, r2, r3));
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

Um_instruction loadval(unsigned ra, unsigned val)
{
        Um_instruction instruction = 0;

        instruction = Bitpack_newu(instruction, 4, 28, LV);
        instruction = Bitpack_newu(instruction, 3, 25, ra);
        instruction = Bitpack_newu(instruction, 25, 0, val);

        return instruction;
}

Um_instruction output(Um_register c)
{
        return three_register(OUT, 0, 0, c);

}

Um_instruction three_register(Um_opcode op, int ra, int rb, int rc)
{
        Um_instruction instruction = 0;

        assert(ra < 7 && rb < 7 && rc < 7); 

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


