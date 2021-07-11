
cSources = $(wildcard *.c)
cTargets = $(cSources:.c=)

pSources = $(wildcard *.md)
pTargets = $(pSources:.md=.html)

# very picky C compiler
vpcc = gcc -std=c99 -g -Wall -Wextra -Wpedantic -Wbad-function-cast \
	-Wconversion -Wwrite-strings -Wstrict-prototypes -Wshadow

.PHONY : all clean

all : $(cTargets) $(pTargets)

clean :
	rm -f $(cTargets) $(pTargets)

% : %.c
	$(vpcc) -o $@ $<


# -Vsecnumdepth=1 --number-sections
%.html : %.md links-to-html.lua
	pandoc  -f markdown -t html -s \
	  --toc --toc-depth=2 \
	  -c media/style.css \
	  --lua-filter=links-to-html.lua \
	  -o $@ $<
