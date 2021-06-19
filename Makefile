
sources = $(wildcard *.c)
targets = $(sources:.c=)

# very picky C compiler
vpcc = gcc -std=c99 -g -Wall -Wextra -Wpedantic -Wbad-function-cast \
	-Wconversion -Wwrite-strings -Wstrict-prototypes -Wshadow

.PHONY : all clean

all : $(targets)

clean :
	rm -f $(targets)

% : %.c
	$(vpcc) -o $@ $<


# -Vsecnumdepth=1 --number-sections
%.html : %.md
	pandoc  -f markdown -t html -s --toc -c media/style.css \
	  -o $@ $<
