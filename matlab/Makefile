MEX = $(MATLABDIR)/bin/mex
EXT = $(shell $(MATLABDIR)/bin/mexext)

matlab: discrete_ars.$(EXT)

discrete_ars.$(EXT): discrete_ars.c ../ars.c
	$(MEX) -I.. discrete_ars.c ../ars.c

clean:
	@(rm -rf discrete_ars.$(EXT) )  || true
