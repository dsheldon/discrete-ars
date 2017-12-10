discrete_ars.so: discrete_ars.pyx
	python setup.py build_ext --inplace

clean:
	@(rm -rf build) || true
	@(rm -f discrete_ars.c discrete_ars.so) || true
