
#include "matrix.h"

Cmatrix :: Cmatrix()
{
        iflag = 0;
        ibeg = 0;
        for(int i=0;i<10;i++)
        {
                b[i]=0;
                ipivot[i]=0;
        }       
}
Cmatrix :: ~Cmatrix()
{
}

//using namespace DS;

/********************************************************************
*****                                   MATRIX MULTIPLICATION                                   *****
***** Inputs : matrix a1, matrix a2, rows, columns                              *****
***** Returns: matrix a3.                                       *****
********************************************************************/


void  Cmatrix::mat_mul(double *a1, double *a2, double *a3, int m, int n, int o)
{
        int i, j, k;

        for(i=0; i<m; i++)
                for(k=0; k<o; k++)
                {
                        *(a3 + o*i+k) = 0.0;
                        for(j=0; j<n; j++)
                                *(a3+o*i+k) +=  (*(a1 + n*i+j)) * (*(a2 + o*j+k));
                }
}

/********************************************************************
*****                                   MATRIX TRANSPOSE                                                *****
***** Inputs : matrix a1, matrix a2, rows, columns                              *****
***** Returns: matrix a2 as the transpose of a1                 *****
********************************************************************/

void  Cmatrix::mat_transpose(double *a1, double *a2, int m, int n)
{
        int i,j;
        for(i=0; i<m; i++)
                for(j=0;j<n; j++)
                        *(a2+m*j+i) = *(a1+n*i+j);
}

/********************************************************************
*****                                   MATRIX SUBTRACTION                                              *****
***** Inputs : matrix a1, matrix a2, rows, columns                              *****
***** Returns: matrix a3.                                       *****
********************************************************************/

void  Cmatrix::mat_subtract(double *a1, double *a2, double *a3, int m, int n)
{
        int i,j;

        for(i=0; i<m; i++)
                for(j=0;j<n; j++)
                        *(a3+m*j+i) = *(a1+m*j+i)-*(a2+m*j+i);
}



/********************************************************************
*****                                           MATRIX ADDITION                                         *****
***** Inputs : matrix a1, matrix a2, m rows, n columns                  *****
***** Returns: matrix a3.                                       *****
********************************************************************/

void  Cmatrix::mat_addition(double *a1, double *a2, double *a3, int m, int n)
{
        int i,j;

        for(i=0; i<m; i++)
                for(j=0;j<n; j++)
                        *(a3+n*i+j) = *(a1+n*i+j) + *(a2+n*i+j);
}

void  Cmatrix::factor(double *w, int n)
{
        int i,j,k,istar;
        double awikod, colmax, ratio, rowmax, temp;

        iflag = 1;
        for(i=0; i<n; i++)
        {
                ipivot[i] = i+1;
                rowmax = 0;
                for(j=0; j<n; j++)
                {
                    if(fabs(*(w+10*i+j)) > rowmax)  //changed by neeharika to handle negative values
                    {
                        rowmax = fabs(*(w+10*i+j)); //changed by neeharika to handle negative values
                    }
                }
                if(rowmax == 0)
                {
                        iflag =0;
                        rowmax=1;
                }
                b[i] = rowmax;
        }
        for(k=0; k<n-1; k++)
        {
                colmax = fabs(*(w+11*k)/b[k]);
                istar = k;
                for(i=k+1; i<n; i++)
                {
                        awikod = fabs(*(w+10*i+k)/b[i]);
                        if(awikod > colmax)
                        {
                                colmax = awikod;
                                istar = i;
                        }
                }
                if(colmax ==0)
                {
                        iflag = 0;
                }
                else
                {
                        if(istar > k)
                        {
                                iflag = -iflag;
                                i = ipivot[istar]-1;
                                ipivot[istar] = ipivot[k];
                                ipivot[k] = i+1;
                                temp = b[istar];
                                b[istar] = b[k];
                                b[k] = temp;
                                for(j=0;j<n; j++)
                                {
                                        temp = *(w+10*istar+j);
                                        *(w+10*istar+j) = *(w+10*k+j);
                                        *(w+10*k+j) = temp;
                                }
                        }
                }
                for(i=k+1; i<n; i++)
                {
                        *(w+10*i+k) = *(w+10*i+k)/(*(w+11*k));
                        ratio = *(w+10*i+k);
                        for(j=k+1; j<n; j++)
                        {
                                *(w+10*i+j) -= ratio * (*(w+10*k+j));
                        }
                }
        }
        if(*(w+11*n-11) == 0)
        {
                iflag = 0;
        }
}

void  Cmatrix::subst(double *w, double *ainv, int n)
{
        int i,j,ip;
        double sum;
        ip = ipivot[0]-1;
        *(ainv+ibeg) = b[ip];

        for(i=1; i<n; i++)
        {
                sum = 0;
                for(j=0; j<=i-1; j++)
                        sum += *(w+10*i+j) * (*(ainv+ibeg+10*j));
                ip = ipivot[i] -1;
                *(ainv+ibeg+10*i) = b[ip] - sum;
        }
        *(ainv+ibeg+10*n-10) = *(ainv+ibeg+10*n-10)/(*(w+11*n-11));
        for(i=n-2; i>=0; i--)
        {
                sum = 0;
                for(j = i+1; j<n; j++)
                        sum += *(w+10*i+j) * (*(ainv+ibeg+10*j));
                *(ainv+ibeg+10*i) = (*(ainv+ibeg+10*i) - sum)/(*(w+11*i));
        }
}

/********************************************************************
*****                                           MATRIX INVERSE                                          *****
***** Inputs : matrix v, matrix y, rows                                                 *****
***** Returns: matrix y.                                        *****
********************************************************************/

void  Cmatrix::mat_inverse(double *v, double *y, int n)
{
        int i,j;
        double w[10][10], ainv[10][10];

        for(i=0; i<n; i++)
                for(j=0; j<n; j++)
                        w[i][j] = *(v+n*i+j);
        factor(*w,n);

        for(i=0; i<n; i++)
                b[i] = 0;
        ibeg = 0;
        for(j=0; j<n; j++)
        {
                b[j] = 1;
                subst(*w,*ainv,n);
                b[j] =0;
                ibeg++;
        }
        if(iflag !=0)
        {
                for(i=0; i<n; i++)
                        for(j=0; j<n; j++)
                                *(y+n*i+j) = ainv[i][j];
        }
}

void  Cmatrix::mat_constmul(double ll, double *a1, double *b1, int m, int n)
{
        //printf("Matrix const_multiplication is called  m=%d n=%d\n",m,n);
        //printf("**********\n a1[0] = %lf a[1] = %lf a1[2] = %lf ll = %lf\n***********\n",*(a1),*(a1+1),*(a1+2),ll);
        int i,j;
        for (i=0; i<m; i++)
                for (j=0; j<n; j++)
                {
                        *(b1 + n*i +j) = (*(a1 + n*i +j))*ll;
                        //printf(" *(b1 + n*i +j)= %lf  (n*i +j)= %d\n", *(b1 + n*i +j),( n*i +j));
                }
}

void  Cmatrix::set_mat(double *m1,double *m2, int k, int w)
{
        int i, j;

        for(i=0;i<k;i++)
        {
                for(j=0;j<k;j++)
                {
                        if((i == j) && ((*(m2+k*i+j)) > 0))
                                *(m1+w*i+j) = *(m2+k*i+j);
                }
        }
}

int Cmatrix :: det(int b[MATDET_ROWS][MATDET_ROWS],int m)
{
        int sum=0,c,subi,subj,i,j;
        int submat[MATDET_ROWS][MATDET_ROWS];
        if(m == 1)
                return b[0][0];
        else if(m == 2)
                return(b[0][0]*b[1][1]-b[0][1]*b[1][0]);
        else 
        {    
                for(c=0;c<m;c++)
                {
                        subi = 0; 
                        for(i=1;i<m;i++)
                        {
                                subj = 0;
                                for(j=0;j<m;j++)
                                {
                                        if(j == c)
                                                continue;
                                        submat[subi][subj] = b[i][j];
                                        subj++;
                                }
                                subi++;
                        }
                        sum=sum+(pow(-1,c)*b[0][c]*det(submat,m-1));
                }
        }
        return sum;
}
