#include "seq.h"
#include "uarray.h"

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <inttypes.h>

#include <unistd.h>


void execute_instructions(FILE**, Seq_T, Seq_T);
void conditional_move(uint32_t (*$r)[], short a, short b, short c);
void segmented_load(uint32_t (*$r)[], Seq_T $m, short a, short b, short c);
void segmented_store(uint32_t (*$r)[], Seq_T $m, short a, short b, short c);
void addition(uint32_t (*$r)[], short a, short b, short c);
void multiplication(uint32_t (*$r)[], short a, short b, short c);
void division(uint32_t (*$r)[], short a, short b, short c);
void bitwise_nand(uint32_t (*$r)[], short a, short b, short c);
void halt();
void map_segment(uint32_t (*$r)[], Seq_T $m, short b, short c, Seq_T unmapped);
void unmap_segment(uint32_t (*$r)[], Seq_T $m, short c, Seq_T unmapped);
void output(uint32_t (*$r)[], short c);
void input(uint32_t (*$r)[], short c);
void load_program(uint32_t (*$r)[], Seq_T $m, short b);
void load_value(uint32_t (*$r)[], short a, uint32_t value);
void free_instructions(Seq_T mapped, Seq_T unmapped);
