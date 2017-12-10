#include <math.h>
#include <strings.h>
#include <stdio.h>
#include <gsl/gsl_rng.h>
#include "ars.h"

double poisson_logpmf(double k, void *data)
{
    double lambda = *((double *) data);
    return k*log(lambda) - lgamma(k+1);
}

gsl_rng *rng;

void gsl_urand_setup()
{
    gsl_rng_env_setup();
    const gsl_rng_type *T = gsl_rng_default;
    rng = gsl_rng_alloc (T);
}

double gsl_urand()
{
    return gsl_rng_uniform( rng );
}

void gsl_urand_destroy()
{
    gsl_rng_free(rng);
}

int main(void)
{
    gsl_urand_setup();

    double lambda = 900;
    int M = 10000;

    double *samples = (double *) malloc( M * sizeof(double) );
    double startpoints[] = {2, 901};
    int nstart = 2;
    
    discrete_ars(samples, M, &gsl_urand, &poisson_logpmf, &lambda, 0, HUGE_VAL, startpoints, nstart);

    double mean = 0;
    int i = 0;
    for (i = 0; i < M; i++)
    {
	mean += ((double) samples[i])/M;
    }
    printf("mean = %.4f, sample mean = %.4f\n", lambda, mean);

    free(samples);
    gsl_urand_destroy();
}
