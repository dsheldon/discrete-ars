#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <strings.h>
#include "ars.h"

#ifdef MATLAB_MEX_FILE
#include "mex.h"
#define ERR(s) ( mexErrMsgTxt(s) )
#else
#define ERR(s) { fprintf(stderr, s); return 0; }
#endif

/***********************************************************************
 * Adaptive rejection sampling
 ***********************************************************************/
int discrete_ars(double    *samples, 
		 int        nsamples, 
		 rand_fp    urand,
		 logpmf_fp  f, 
		 void      *params, 
		 double     lb, 
		 double     ub , 
		 double*    startpoints, 
		 int        nstart)
{


    // Initiliaze the piecewise linear function with the starting points
    piecewise_linear_fun g;
    g.lb = lb;
    g.ub = ub;

    g.first_piece = 0;
    g.last_piece = 0;
    g.n_pieces = 0;

    // Pick some starting pieces that are always valid
    if ( isfinite(lb) )
	insert_piece( &g, lb, f(lb, params), f, params);

    if ( isfinite(ub) )
	insert_piece( &g, ub-1, f(ub-1, params), f, params);

    if ( !isfinite(lb) && !isfinite(ub) )
	insert_piece( &g, 0, f(0, params), f, params);

    int i;
    for (i = 0; i < nstart; i++)
    {
	insert_piece( &g, startpoints[i], f(startpoints[i], params), f, params );
    }

    if ( !isfinite(lb) && g.first_piece->slope < -ZERO_SLOPE_THRESH )
    {
	ERR("Require a starting point left of mode if unbounded on the left\n");
    }
    if ( !isfinite(ub) && g.last_piece->slope > ZERO_SLOPE_THRESH )
    {
	ERR("Require a starting point right of mode if unbounded on the right\n");
    }
    
    /* Compute intersection points, etc. */
    update_pieces( &g );

    /* Generate the samples */
    i = 0;
    double x; 			// the candidate 
    double g_of_x;              // the value of the piecewise linear
				// uppper bound at x
    while ( i < nsamples )
    {	
	double U = urand()*g.Z;
	
	/* Find the correct piece */
	double cumprob = 0.0;
	piece *p = g.first_piece;
	while ( p && cumprob + p->prob < U)
	{
	    cumprob += p->prob;
	    p = p->next;
	}
	
	/* Now sample within the piece */
	U -= cumprob;
	
	double C = g.shift_val;
	double m = p->slope;
	double b = p->intercept;
	double lb = p->lb;

	if ( m < -ZERO_SLOPE_THRESH )
	{
	    double g_x_plus_one = log( exp( m*lb + b - C) - U * (1 - exp(m)) );
	    x = floor( (g_x_plus_one - b + C) / m);
	    g_of_x = m*x + b;
	}
	else if ( m > ZERO_SLOPE_THRESH )
	{
	    double g_x_plus_one = log ( exp( m*(lb-1) + b - C ) + U * ( 1 - exp(-m)) );
	    x = floor( (g_x_plus_one - b + C) / m + 1 );
	    g_of_x = m*x + b;
	}
	else
	{
	    x = p->lb + floor( U / exp( m*lb + b - C) );
	    g_of_x = b;
	}

	if (! isfinite(x))
	{
	    ERR("Error in discrete_ars. Non-finite candidate generated.\n");
	}
	
        /* Compute the squeeze value */ 
        double squeeze_of_x = -HUGE_VAL; 
        if ( x >= p->x && p->next ) 
        {  
            squeeze_of_x = p->squeeze_slope*x + p->squeeze_intercept;
        }
        else if ( x < p->x && p->prev )
        {
            squeeze_of_x = p->prev->squeeze_slope*x + p->prev->squeeze_intercept;
        }
	
	/* Acceptance test */
	double W = urand();

	/* First check the squeeze */
	if ( squeeze_of_x - g_of_x > log(W) )
 	{ 
 	    samples[i++] = x;
 	} 
	else 
	{
	    double f_of_x = f(x, params);
	    
	    if ( f_of_x - g_of_x > log(W) )
	    {
		samples[i++] = x;
	    }
	    else
	    {
		insert_piece( &g, x, f_of_x, f, params );
		update_pieces( &g );
	    }
	}

    }

    destroy_pieces( &g );

    return 1;
}


/* Insert the piece defined by (x, f_of_x) into the correct spot
   in the doubly-linked list */
void insert_piece(piecewise_linear_fun *g, 
		  double x, 
		  double f_of_x, 
		  logpmf_fp f, 
		  void *params )
{

    /* Bump any points equal to the upper bound down by one so we can
       compute the slope by finite differences  */
    if (x >= g->ub)
    {
	x = g->ub - 1;
	f_of_x = f(x, params);
    }

    /* Find first piece with tangent point strictly greater than x */
    piece *p = g->first_piece;
    piece *lastp = 0;
    while( p && p->x <= x )
    {
	lastp = 0;
	p = p->next;
    }

    /* Don't insert an identical point */
    if (lastp && fabs(lastp->x - x) < 1e-10)
    {
        return;
    }

    /* Construct the piece */
    piece *newp = (piece *) malloc(sizeof(piece));
    newp->x = x;
    newp->f = f(x, params);
    newp->slope = f(x+1, params) - newp->f;
    newp->intercept = newp->f - newp->slope * x;


    /* Insert the new piece before p     
         - Before insert: q <-> p          
         - After insert: q <-> newp <-> p     */

    piece *q;

    if (p)
    {
	q = p->prev;
	p->prev = newp;
    }
    else
    {
	q = g->last_piece;
	g->last_piece = newp;
    }

    newp->prev = q;
    newp->next = p;

    if ( q )
    {
	q->next = newp;
    }
    else
    {
	g->first_piece = newp;
    }

    g->n_pieces++;
}


void update_pieces(piecewise_linear_fun *g )
{

    /*
      Find the intersections between tangents lines to determine
      lower bound and upper bound of each piece
    */
    
    g->first_piece->lb = g->lb;
    g->last_piece->ub = g->ub;

    double split;
    piece *p = g->first_piece;
    while ( p && p->next)
    {
	piece *q = p->next;
	split = floor( - ( q->intercept - p->intercept ) / ( q->slope - p->slope ) );
	p->ub = split;
	q->lb = split+1;

	/* Compute secant lines */
        p->squeeze_slope = (q->f - p->f)/(q->x - p->x);
        p->squeeze_intercept = p->f - p->squeeze_slope * p->x;

	p = p->next;
    }

    /*
      To avoid overflow/underflow later, find the biggest value we plan to
      exponentiate 
    */

    g->shift_val = -HUGE_VAL;
    p = g->first_piece;
    while ( p )
    {	
	double m = p->slope;
	double b = p->intercept;

	g->shift_val = MAX( g->shift_val, m * p->lb + b ); 
	g->shift_val = MAX( g->shift_val, m * p->ub + b ); 

	p = p->next;
    }

    /* 
       Compute cell probabilities
    */

    g->Z = 0.0;

    p = g->first_piece;
    g->squeeze_Z = 0.0;

    while ( p )
    {

	double prob = compute_prob(p->slope, p->intercept, p->lb, p->ub, g->shift_val);
	p->prob = prob;
	g->Z += prob;

	if (p->next)
	{
	    prob = compute_prob(p->squeeze_slope, p->squeeze_intercept, p->x, (p->next->x - 1), g->shift_val);
	    p->squeeze_prob += prob;
	    g->squeeze_Z += prob;
	}

	p = p->next;
    }
}

double compute_prob(double m, double b, double lb, double ub, double C)
{
    double prob = 0.0;

    if ( m < -ZERO_SLOPE_THRESH)
    {
	prob = ( exp( m*lb + b - C ) - exp( m*(ub+1) + b - C) ) / (1 - exp(m));
    }
    else if (m > ZERO_SLOPE_THRESH)
    {
	prob = ( exp( m*ub + b - C) - exp( m*(lb-1) + b - C) ) / (1 - exp(-m));
    }
    else
    {
	prob = exp( m*lb + b - C ) * ( ub - lb + 1); // uniform segent
    }

    return prob;
}

void destroy_pieces(piecewise_linear_fun *g)
{
    piece *p = g->first_piece;
    
    while (p)
    {
	piece *next = p->next;
	free(p);
	p = next;
    }
}

void print_pieces(piecewise_linear_fun *g)
{    
    piece *p = g->first_piece;

    printf("*** Print pieces ***\n");

    printf("Z ratio: %.2f (Z1=%.2e, Z1=%.2e)\n", g->squeeze_Z/g->Z, g->Z, g->squeeze_Z);
    
    while ( p )
    {
	printf("x=%d, f=%.4e, slope=%.4e, intercept=%.4e, lb=%.0f, ub=%.0f, Z=%.4e\n", (int) p->x, p->f, p->slope, p->intercept, p->lb, p->ub, p->prob);
	printf("  squeeze: slope=%.4e, intercept=%.4e, Z=%.4e\n", p->squeeze_slope, p->squeeze_intercept, p->squeeze_prob );
	p = p->next;
    }
}
