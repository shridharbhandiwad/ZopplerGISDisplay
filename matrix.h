#ifndef matrix_h
#define matrix_h

#include <iostream>
#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <fstream>
#include <string.h>
#define MATDET_ROWS 10

class Cmatrix
{
        
        public:
                 Cmatrix();
                 ~Cmatrix();
                 void mat_mul(double *, double *, double *, int , int , int );
                 void mat_transpose(double *, double *, int , int );
                 void mat_subtract(double *, double *, double *, int , int );
                 void mat_addition(double *, double *, double *, int , int );
                 void mat_inverse(double *, double *, int );
                 void mat_constmul(double , double *, double *, int , int );
                 void set_mat(double *,double *, int , int );
                 void subst(double *w, double *ainv, int n);
                 void factor(double *w, int n);
                 int det(int b[][MATDET_ROWS],int m);
        private:
                int iflag,ibeg,ipivot[MATDET_ROWS];
                double b[MATDET_ROWS];
};

#endif

