GCC=gcc
CFLAGS=-Wall

default: test_pois test_binom test_pois_2geo

debug: CFLAGS += -g
debug: default

test_pois_2geo: test_pois_2geo.c ars.c
	$(GCC) $(CFLAGS) $^ -o $@

test_pois: test_pois.c ars.c
	$(GCC) $(CFLAGS) $^ -o $@

test_binom: test_binom.c ars.c
	$(GCC) $(CFLAGS) $^ -o $@

GSLINC=/Library/Frameworks/GSL.framework/Versions/1/unix/include

test_gsl: CFLAGS += -I$(GSLINC)	-framework GSL -lm 
test_gsl: test_gsl.c ars.c
	$(GCC) $(CFLAGS) test_gsl.c ars.c -o test_gsl

clean:
	@(rm -f test_pois test_binom test_gsl test_pois_2geo) || true
	@(rm -rf {test_pois,test_binom,test_gsl,test_pois_2geo}.dSYM)  || true
