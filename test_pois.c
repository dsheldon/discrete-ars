//  gcc -g test_poisson.c ars.c -o test_poisson

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include "ars.h"

double poisson_logpmf(double k, void *data)
{
    double lambda = *((double *) data);
    return k*log(lambda) - lambda - lgamma(k+1);
}

double urand()
{
    return (double)rand() / (double)RAND_MAX;
}

int main(void)
{
    double lambda = 900;
    int M = 5000000;
    double *samples = malloc(M * sizeof(double));
    double startpoints[] = {2, 901};
    int nstart = 2;

    srand(3);
    
    discrete_ars(samples, M, &urand, &poisson_logpmf, &lambda, 0, HUGE_VAL, startpoints, nstart);

    double mean = 0;
    int i = 0;
    for (i = 0; i < M; i++)
    {
        //	printf("%.0f ", samples[i]);
	mean += ((double) samples[i])/M;
    }
    printf("\nmean = %.4f, sample mean = %.4f\n", lambda, mean);

    free(samples);
    return 0;
}
