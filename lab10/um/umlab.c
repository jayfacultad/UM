/*
 * umlab.c
 * 
 * The functions defined in this lab should be linked against umlabwrite.c
 * to produce a unit test writing program. Any additional functions and unit
 * tests written for the lab go here.
 */

#include <stdint.h>
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

        Um_write_sequence(stdout, stream);
}

Um_instruction loadval(unsigned ra, unsigned val)
{
        return three_register(LV, ra, 0, val);
}

Um_instruction output(Um_register c)
{
        return three_register(OUT, 0, 0, c);
}

Um_instruction three_register(Um_opcode op, int ra, int rb, int rc)
{
        Um_instruction instruction;

        assert(ra > 0 || ra < 7 || rb > 0 || rb < 7 || rc > 0 || rc < 7); 

        Bitpack_newu(uint64_t instruction, unsigned 4, unsigned 28, 
                      uint64_t op);

        Bitpack_newu(uint64_t instruction, unsigned 3, unsigned 6, 
                      uint64_t ra);

        Bitpack_newu(uint64_t instruction, unsigned 3, unsigned 3, 
                      uint64_t rb);

        Bitpack_newu(uint64_t instruction, unsigned 3, unsigned 0, 
                      uint64_t rc);

        return instruction;
}

extern void Um_write_sequence(FILE *output, Seq_T stream)
{
        while(Seq_length(stream)) {
                void *word = Seq_remhi(stream);
                Um_instruction extracted_word = (void *)(Um_instruction)word;

                int byte_1 = Bitpack_getu(extracted_word, 8, 24);
                int byte_2 = Bitpack_getu(extracted_word, 8, 16);
                int byte_3 = Bitpack_getu(extracted_word, 8, 8);
                int byte_4 = Bitpack_getu(extracted_word, 8, 0);

                putc(byte_1, output);
                putc(byte_2, output);
                putc(byte_3, output);
                putc(byte_4, output);
        }
}
