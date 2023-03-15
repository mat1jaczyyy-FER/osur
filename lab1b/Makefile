MAIN=main.c #SRC=$(wildcard *.c)
ELF=$(patsubst %c, %elf, $(MAIN))

FOLDERS=$(wildcard */.)
MOD_O=$(foreach i,$(subst /.,,$(FOLDERS)),$i/mod-$i.o)

.SECONDARY:

all: $(ELF)

%.elf: %.o
	for dir in $(FOLDERS); do $(MAKE) -C $$dir; done
	gcc $^ $(MOD_O) -o $@

%.o: %.c
	gcc -c $< -o $@

pokreni: $(ELF)
	./$<

brisi:
	rm -f *.o *.elf
	for dir in $(FOLDERS); do $(MAKE) -C $$dir brisi; done
