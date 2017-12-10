GCC=gcc
CFLAGS=-Wall

default: test_pois test_binom

debug: CFLAGS += -g
debug: default

test_pois: test_pois.c ars.c
	$(GCC) $(CFLAGS) test_pois.c ars.c -o test_pois

test_binom: test_binom.c ars.c
	$(GCC) $(CFLAGS) test_binom.c ars.c -o test_binom

GSLINC=/Library/Frameworks/GSL.framework/Versions/1/unix/include

test_gsl: CFLAGS += -I$(GSLINC)	-framework GSL -lm 
test_gsl: test_gsl.c ars.c
	$(GCC) $(CFLAGS) test_gsl.c ars.c -o test_gsl

clean:
	@(rm -f test_pois test_binom test_gsl) || true
	@(rm -rf {test_pois,test_binom,test_gsl}.dSYM)  || true
