# discrete-ars

This package contains an easy-to-use C implementation of the
discrete adaptive rejection sampling (ARS) algorithm, a MATLAB
mex interface to the same function, and examples. All you need to
provide is a function to compute the log of the probability mass
function (pmf), and a guarantee that the log pmf is concave.

See below for instructions on:

* Using the C function
* Compilation
* Using the MATLAB function
* Using the Python wrapper

# Using the C function

The quickest way to get started is by looking at the examples
`test_binom.c` and `test_pois.c` and modifying them accordingly.

The C implementation is found in the files `ars.c` and `ars.h`. To
generate samples from your distribution, simply call the function
`discrete_ars()`, with prototype:

~~~ .C
int discrete_ars(double    *samples, 
		 int       nsamples, 
		 rand_fp   r,
		 logpmf_fp f, 
		 void      *params, 
		 double    lb, 
		 double    ub , 
		 double*   startpoints, 
		 int       nstart);
~~~

~~~ .text
RETURN VALUE

  The return value is 1 upon success, and 0 upon failure.

INPUT ARGUMENTS

 *samples      Storage for the returned samples. The values will all be
               integers (to floating point precision) but they are
               stored as doubles.
	       
 nsamples      The number of samples to draw from the distribution
	       
 rand_fp       Pointer to a function that takes no arguments and returns
 	       a uniform random number in [0,1] as a double. E.g.:
	       
               double urand() {
	         return (double)rand() / (double)RAND_MAX;
	       }
               rand_fp r = &urand;
	       
 logpmf_fp     Pointer to your function for computing the log pmf. E.g.:
	       
	       double my_logpmf(double k, void *params) {
                 ...
	       }
	       logpmf_fp f = &my_logpmf;
	       
 *params       Any parameters for your probability mass function. These
 	       will be passed as the second argument during all calls to
 	       your log pmf function.
	       
 lb	       The lower bound of the support of the pmf. Should be
 	       an integer value expressed as a double. Can be (the
 	       numerical equivalent) of infinity (e.g. HUGE_VAL on a
 	       linux system).
 	       
 ub            The upper bound of the support of the pmf. Same
 	       conventions as lb.

 *startpoints  An array of starting points for the discrete_ars
 	       algorithm. In general, points near the mode on either
 	       side will improve the performance, but no points are
 	       required for finite support distributions. 

	       If the support is unbounded below, at least one point
	       smaller than the mode is required. (The log pmf should
	       be increasing---i.e. have positive slope---at the
	       specified point.)

	       If the support is unbounded above, at least one point
	       larger than the mode is required. (The log pmf should
	       be decreasing---i.e. have negative slope---at the
	       specified point.)
 
 nstart	       The number of startpoints provided      
~~~

# Compilation

Compile the test programs like this:

~~~ .bash
$ gcc -g test_binom.c ars.c -o test_binom
~~~

Modify this to suit your needs. 

The provided Makefile includes some more examples, including one that
uses the Gnu Scientific Library (GSL) random number generator (on Mac OS
X).

# Using the MATLAB function

To compile, open MATLAB, change the the `matlab` subdirectory, and execute
the script `make.m`. From MATLAB:

~~~ .bash
>> cd matlab
>> make
~~~

The MATLAB function is also called `discrete_ars()`.

The quickest way to start is with the example in the file `example.m`. 

The usage is documented in the online help:

~~~ .text
>> help discrete_ars 

  DISCRETE_ARS Discrete adaptive rejection sampler
    samples = discrete_ars( logp, bounds, points, nsamples)
 
  INPUT ARGUMENTS
 
    logp      Anonymous function to compute the log pmf   
 	      
    bounds    A two-element vector. The pmf support is the set of all k
              such that bounds(1) <= k <= bounds(2). Use -Inf and +Inf to
              specify infinite support.
 	      
    points    The starting points for the ARS routine. Can be empty if the
              support is finite. If unbounded below, at least one point
              smaller than the mode is required (log-pmf is
              increasing). If unbounded above, at least one point greater
              than the mode is required (log-pmf is decreasing).
    	      
    nsamples  How many samples to return.
 
  RETURN VALUE
 
    samples   A vector of numbers drawn from the distribution.

~~~

# Python wrapper

The Python wrapper is contained in the `python` subdirectory. Try this:

~~~ .bash
$ cd python
$ python setup.py build_ext --inplace
$ python example.py
~~~
