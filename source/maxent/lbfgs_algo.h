#ifndef _LBFGS_ALGO_H
#define LBFGS_ALGO_H
#include "f2c.h"

int lbfgs_(integer *n, integer *m, doublereal *x, doublereal 
	*f, doublereal *g, integer *diagco, doublereal *diag, integer *iprint,
		 doublereal *eps, doublereal *xtol, doublereal *w, integer *iflag, 
		 	integer *niter, integer *nfuns);

#endif
