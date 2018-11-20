#
# Makefile 
# 
CC = gcc


IFLAGS  = -I/comp/40/include -I/usr/sup/cii40/include/cii -I/comp/40/public_html/solutions/bitpack.h 
CFLAGS  = -g -O3 -std=gnu99 -Wall -Wextra -Werror -pedantic $(IFLAGS)
LDFLAGS = -g -L/comp/40/lib64 -L/usr/sup/cii40/lib64
LDLIBS  = -l40locality -lm -lcii40


EXECS   = um

all: $(EXECS)

um: machine.o segments.o bitpack.o
	$(CC) $(LDFLAGS) $^ -o $@ $(LDLIBS)

writetests: umlabwrite.o umlab.o bitpack.o
	$(CC) $(LDFLAGS) $^ -o $@ $(LDLIBS)

# To get *any* .o file, compile its .c file with the following rule.
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(EXECS)  *.o
