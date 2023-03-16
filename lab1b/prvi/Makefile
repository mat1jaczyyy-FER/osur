SRC=$(wildcard *.c)
OUT=$(patsubst %.c, %.o, $(SRC))

FOLDERS=$(wildcard */.)
MOD_O=$(foreach i,$(subst /.,,$(FOLDERS)),$i/mod-$i.o)

all: $(OUT) $(MOD_O)
	ld -r $^ -o "mod-$(notdir $(shell pwd)).o"

%.o: %.c
	gcc -c $< -o $@

.PHONY: FORCE

mod-%.o: FORCE
	$(MAKE) -C $(word 1,$(subst /, , $@))

FORCE: ;

pokreni: all
	./$(ELF)

brisi:
	rm -f *.o
	for dir in $(FOLDERS); do $(MAKE) -C $$dir brisi; done
