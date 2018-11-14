/*****************************************************************************
*
*   bitpack.c
*
*   Michael Robinson
*   Jay Facultad
*   COMP 40 / Fall 2018
*   Homework 4
*   October 19, 2018
*
*****************************************************************************/

#include <stdbool.h>
#include <stdint.h>
#include <bitpack.h>
#include <stdio.h>

#include "assert.h"
#include "except.h"

const unsigned MAX_BITS = 64;
const unsigned WORD_SIZE = 32;

Except_T Bitpack_Overflow = { "Overflow packing bits" };

/*
* Function: Bitpack_fitsu
* Does: Determines if a value can be represented within a specified bit width.
* Parameters: A value represented in unsigned 64 bits, width.
* Returns: bool
*/
bool Bitpack_fitsu(uint64_t n, unsigned width)
{
        assert(width <= MAX_BITS);
        uint64_t n_shift;

        if (width < MAX_BITS) {
                n_shift = n >> width;
        } else {
                n_shift = 0;
        }
        return (n_shift == (uint64_t)0);
}

/*
* Function: Bitpack_fitss
* Does: Determines if a value can be represented within a specified bit field
        width.
* Parameters: A value represented as a signed 64 bit integer and the fied's 
        width.
* Returns: bool
*/
bool Bitpack_fitss(int64_t n, unsigned width)
{
        assert(width <= MAX_BITS);
        if (width == MAX_BITS) {
                return (width == MAX_BITS);
        }   

        /* negative number check */
        if (n < 0) {
                int range = ((uint64_t)1 << (width)) - 1;
                int max_value = ((uint64_t)1 << (width - 1)) - 1;
                int min = max_value - range;
                return ((n >= min));                     
        }
        
        return Bitpack_fitsu((uint64_t)n, (width - 1));
}

/*
* Function: Bitpack_getu
* Does: Extracts a value from a word.
* Parameters: The word, width of the value's field, and the least significant
        bit location.
* Returns: The value represented as an unsigned 64 bit integer.
*/
uint64_t Bitpack_getu(uint64_t word, unsigned width, unsigned lsb)
{
        assert(width <= MAX_BITS && (width + lsb) <= MAX_BITS);
        if (width == 0) {
                return 0;
        }
        uint64_t mask = ~0;
        /* Calculations in 3 lines below will never shift by 64 bits */
        mask = (mask << (MAX_BITS - width)); 
        mask = (mask >> (MAX_BITS - width));
        mask = (mask << lsb);

        if (lsb < WORD_SIZE) {
                return (word & mask) >> lsb;
        } else {
                uint64_t n_shift = 0;
                return n_shift;
        }    
}

/*
* Function: Bitpack_gets
* Does: Extracts a value from a word.
* Parameters: The word represented as an unsigned 64 bit integer, width of
        the value's field, and the least significant bit location.
* Returns: The value represented as a signed 64 bit integer.
*/
int64_t Bitpack_gets(uint64_t word, unsigned width, unsigned lsb)
{
        assert(width <= MAX_BITS && (width + lsb) <= MAX_BITS);
        if (width == 0) {
                return 0;
        }

        uint64_t flag = 1 << (lsb + (width - 1));

        /* positive number check */
        if ((flag & word) == 0) {
                return Bitpack_getu(word, width, lsb);
        }

        int64_t trans_word = Bitpack_getu(word, width, lsb);
        int64_t s_word = trans_word;
        s_word = s_word << (MAX_BITS - width);
        s_word = s_word >> (MAX_BITS - width);
        int64_t mask = ~0;

        if (width < MAX_BITS) {
                mask = mask << width;
        } else {
                mask = 0;
        }
        return (mask | s_word);
}

/*
* Function: Bitpack_newu
* Does: Adds a field to a word in a specified location.
* Parameters: The word represented as an unsigned 64 bit integer, width of
        the value's field, the least significant bit location, and the value
        to be added (represented as an unsigned 64 bit integer).
* Returns: The word with the value added in the specified field (represented
        as an unsigned 64 bit integer).
*/
uint64_t Bitpack_newu(uint64_t word, unsigned width, unsigned lsb, 
                      uint64_t value)
{
        assert(width <= MAX_BITS && (width + lsb) <= MAX_BITS);
        if (Bitpack_fitsu(value, width) == 0) {
                RAISE(Bitpack_Overflow);
        }
        if (width == 0) {
                return word;
        }

        uint64_t mask = ~0;
        if (MAX_BITS - width - lsb < MAX_BITS) {
                mask = mask << (MAX_BITS - width) >> (MAX_BITS - width - lsb);
        } else if (MAX_BITS - width - lsb == MAX_BITS) {
                mask = 0;
        }

        mask = ~mask;
        word &= mask;

        if (MAX_BITS - width - lsb < MAX_BITS) {
              value = value << (MAX_BITS - width) >> (MAX_BITS - width - lsb); 
        } 
        word |= value;
        return word;
}

/*
* Function: Bitpack_newu
* Does: Adds a field to a word in a specified location.
* Parameters: The word represented as an unsigned 64 bit integer, width of
        the value's field, the least significant bit location, and the value
        to be added (represented as a signed 64 bit integer).
* Returns: The word with the value added in the specified field (represented
        as an unsigned 64 bit integer).
*/
uint64_t Bitpack_news(uint64_t word, unsigned width, unsigned lsb, 
                      int64_t value)
{
        assert(width <= MAX_BITS && (width + lsb) <= MAX_BITS);
        if (Bitpack_fitss(value, width) == 0) {
                RAISE(Bitpack_Overflow);
        }
        uint64_t value_complement = value;
        value_complement = value_complement << (MAX_BITS - width) >> (MAX_BITS
                                               - width);
        return Bitpack_newu((uint64_t)word, width, lsb, value_complement);
}