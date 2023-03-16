MAIN=main.c #SRC=$(wildcard *.c)
ELF=$(patsubst %c, %elf, $(MAIN))

FOLDERS=$(wildcard */.)
MOD_O=$(foreach i,$(subst /.,,$(FOLDERS)),$i/mod-$i.o)

.SECONDARY:

all: $(ELF)

%.elf: %.o $(MOD_O)
	gcc $^ -o $@

%.o: %.c
	gcc -c $< -o $@

.PHONY: FORCE

mod-%.o: FORCE
	$(MAKE) -C $(word 1,$(subst /, , $@))

FORCE: ;

pokreni: $(ELF)
	./$<

brisi:
	rm -f *.o *.elf
	for dir in $(FOLDERS); do $(MAKE) -C $$dir brisi; done
