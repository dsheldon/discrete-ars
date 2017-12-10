#define MAX(a, b) ( a > b ? a : b )
#define ZERO_SLOPE_THRESH ( 1e-12 )

/* Function pointers */
typedef double (*logpmf_fp) (double, void*); /* log pmf  */
typedef double (*rand_fp)   ();		     /* uniform random variables */

int discrete_ars(double    *samples, 
		 int       nsamples, 
		 rand_fp   r,
		 logpmf_fp f, 
		 void      *params, 
		 double    lb, 
		 double    ub , 
		 double*   startpoints, 
		 int       nstart);

typedef struct piece
{
    double x;
    double f;

    /* The parameters of the tangent line */
    double lb;
    double ub;
    double slope;
    double intercept;
    double prob;

    /* The parameters of the secant line between x and the next segment */
    double squeeze_slope;
    double squeeze_intercept;
    double squeeze_prob;

    struct piece  *next;
    struct piece  *prev;
} piece;

typedef struct piecewise_linear_fun
{
    double lb;
    double ub;
    double Z;
    double squeeze_Z;
    double shift_val;
    piece *first_piece;
    piece *last_piece;
    int    n_pieces;

} piecewise_linear_fun;

void   print_pieces(piecewise_linear_fun *g);
void  update_pieces(piecewise_linear_fun *g);
void destroy_pieces(piecewise_linear_fun *g);
void   insert_piece(piecewise_linear_fun *g, double x, double f_of_x, logpmf_fp f, void *params );
double compute_prob(double m, double b, double lb, double ub, double C);
