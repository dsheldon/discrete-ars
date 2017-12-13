//  gcc -g test_poisson.c ars.c -o test_poisson

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include "ars.h"

typedef struct {
    double y;
    double lambda;
    double log_eps;
} Pois2GeoParams;

double pois_2geo_logpmf(double k, void *data)
{
    Pois2GeoParams params = *((Pois2GeoParams *) data);
    return k*log(params.lambda) - params.lambda - lgamma(k+1) + params.log_eps*fabs(params.y-k);
}

double urand()
{
    return (double)rand() / (double)RAND_MAX;
}

int main(void)
{
    Pois2GeoParams params;
    params.lambda  = 100.0;
    params.log_eps = log(0.2);
    params.y       = 110.0;
    
    int M = 1000000;
    double *samples = malloc(M * sizeof(double));
    double startpoints[] = {100, 110, 200};
    int nstart = 3;

    srand(3);
    
    discrete_ars(samples, M, &urand, &pois_2geo_logpmf, &params, 0, HUGE_VAL, startpoints, nstart);

    double mean = 0;
    int i = 0;
    for (i = 0; i < M; i++)
    {
	mean += ((double) samples[i])/M;
    }
    printf("sample mean = %.4f\n", mean);

    free(samples);
    return 0;
}
