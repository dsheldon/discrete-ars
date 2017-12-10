import cython
import numpy as np
import random
cimport numpy as np

cdef extern from "stdlib.h":
    double drand48()
    void srand48(long)

cdef double crand():
    return drand48()

cdef double pyrand():
    return <double> random.random()

cdef extern from "ars.h":
    ctypedef double (*logpmf_fp) (double, void*)
    ctypedef double (*rand_fp)   ()

    int discrete_ars(double    *samples,
		 int       nsamples,
		 rand_fp   r,
		 logpmf_fp f,
		 void      *params,
		 double    lb,
		 double    ub ,
		 double*   startpoints,
		 int       nstart)

cdef double logp_c(double x, void* args):
    global logp_global
    logp_val = <double> logp_global(x)
    return logp_val

def seed(seedval):
    srand48(<long> seedval)

def sample(logp_python,
           bounds,
           startpoints,
           n_samples,
           use_py_rand = False):

    cdef double lb = <double> float(bounds[0])
    cdef double ub = <double> float(bounds[1])

    n_samples = int(n_samples)
    
    global logp_global
    logp_global = logp_python

    cdef np.ndarray[np.float64_t, ndim=1, mode="c"] startpoints_c = np.unique(startpoints).astype(np.float64)
    cdef np.ndarray[np.float64_t, ndim=1, mode="c"] samples = np.zeros(n_samples, dtype=np.float64)

    cdef int n_start = len(startpoints)

    rand_fun = crand
    if use_py_rand:
        rand_fun = pyrand

    discrete_ars(<double *> samples.data,
                 <int> n_samples,
                 <rand_fp> rand_fun,
                 <logpmf_fp> logp_c,
                 <void *> NULL,
                 <double> lb,
                 <double> ub,
                 <double *> startpoints_c.data,
                 <int> n_start
                 )

    return samples
