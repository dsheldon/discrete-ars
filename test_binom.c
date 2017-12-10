//  gcc -g test_binom.c ars.c -o test_binom

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include "ars.h"

typedef struct {
    int n;
    double p;
} BinomialParams;

double binomial_logpmf(double k, void *data)
{
    BinomialParams *params = (BinomialParams *) data;
    double p = params->p;
    double n = params->n;
    return k*log(p/(1-p)) - lgamma(k+1) - lgamma(n-k+1); /* unnormalized */
}

double urand()
{
    return (double)rand() / (double)RAND_MAX;
}

int main(void)
{
    // Create Binomial distribution
    BinomialParams params;
    params.n = 1000;
    params.p = 0.6;    

    int M = 1e5;
    double *samples = (double *) malloc(M*sizeof(double));
    double startpoints[] = {10, 60};
    int nstart = 2;

    discrete_ars(samples, M, &urand, &binomial_logpmf, &params, 0, params.n, startpoints, nstart);

    double mean = 0;
    int i;
    for (i = 0; i < M; i++)
    {
	mean += ((double) samples[i])/M;
    }
    printf("mean = %.4f, sample mean = %.4f\n", params.n*params.p, mean);
    free(samples);
    
    return 0;
}
