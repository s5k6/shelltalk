
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

index.html : README
	pandoc -s -c style.css -o index.html -fmarkdown README 
