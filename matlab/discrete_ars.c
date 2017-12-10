#include "mex.h"
#include "ars.h"
#include <math.h>

#define CACHESIZE 1024

double urand()
{
    return (double)rand() / (double)RAND_MAX;
}

/*-------------------------------------------------------------*/
/* MATLAB callback for log pmf                                 */
/*-------------------------------------------------------------*/
double matlab_call_logp(double k, void *data)
{
    mxArray *logp = (mxArray *) data;

    mxArray *plhs[1], *prhs[2];
    prhs[0] = logp;
    prhs[1] = mxCreateDoubleScalar(k);
    if (mexCallMATLAB(1, plhs, 2, prhs, "feval")){
	mexErrMsgTxt("mexCallMATLAB(logp) failed");	
    }
    mxDestroyArray(prhs[1]);
    
    return mxGetPr(plhs[0])[0];
}

/*-------------------------------------------------------------*/
/* Main routine                                                */
/*-------------------------------------------------------------*/
void mexFunction(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{

    if (nrhs < 4)
	mexErrMsgTxt("Four inputs required");

    if( !mxIsClass( prhs[0] , "function_handle")) {
	mexErrMsgTxt("First argument must be a function handle.");
    }
    if( ! (mxIsDouble(prhs[1]) && mxGetNumberOfElements(prhs[1])==2) )
    {
	mexErrMsgTxt("Second argument must be a matrix with two elements.");
    }
    if( ! mxIsDouble(prhs[2]) ) {
	mexErrMsgTxt("Third argument must be of type double.");
    }
    if( ! ( mxIsDouble(prhs[3]) && mxGetNumberOfElements(prhs[3]) == 1) ) {
	mexErrMsgTxt("Fourth argument must be scalar");
    }

    mxArray *logp = (mxArray *) prhs[0];

    double lb = mxGetPr(prhs[1])[0];
    double ub = mxGetPr(prhs[1])[1];

    double *startpoints = mxGetPr(prhs[2]);
    int nstart = mxGetNumberOfElements(prhs[2]);
    int n = (int) mxGetScalar(prhs[3]);
	
    plhs[0] = mxCreateDoubleMatrix(n, 1, mxREAL);
    discrete_ars(mxGetPr(plhs[0]), n, &urand, &matlab_call_logp, logp, lb, ub, startpoints, nstart);
}
