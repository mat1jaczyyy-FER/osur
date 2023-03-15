SRC=$(wildcard *.c)
OUT=$(patsubst %.c, %.o, $(SRC))

FOLDERS=$(wildcard */.)
MOD_O=$(foreach i,$(subst /.,,$(FOLDERS)),$i/mod-$i.o)

all: $(OUT)
	for dir in $(FOLDERS); do $(MAKE) -C $$dir; done
	ld -r $^ $(MOD_O) -o "mod-$(notdir $(shell pwd)).o"

%.o: %.c
	gcc -c $< -o $@

pokreni: all
	./$(ELF)

brisi:
	rm -f *.o
	for dir in $(FOLDERS); do $(MAKE) -C $$dir brisi; done
