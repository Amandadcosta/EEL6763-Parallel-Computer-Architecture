/*Amanda Kelly Dcosta*/
//#include "mpi.h"
#include "omp.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

/*   ttype: type to use for representing time */
typedef double ttype;
ttype tdiff(struct timespec a, struct timespec b)
/* Find the time difference. */
{
  ttype dt = (( b.tv_sec - a.tv_sec ) + ( b.tv_nsec - a.tv_nsec ) / 1E9);
  return dt;
}

struct timespec now()
/* Return the current time. */
{
  struct timespec t;
  clock_gettime(CLOCK_REALTIME, &t);
  return t;
}

//#define NRA 60                 /* number of rows in matrix A */
//#define NCA_RB 12              /* number of rows in matrix B, should equal to NCA */
//#define NCB 10                 /* number of columns in matrix B */


 int main (int argc, char *argv[])
 {
  //int numtasks,            /* number of tasks in partition */
 int tid;                   /* a task identifier */
 //int nthreads;              /* number of worker tasks */
	
  int NRA=atof(argv[1]);      //NRA: number of rows for Matrix A
 int NCA_RB=atof(argv[2]);   //NCA_RB: number of rows for A and columns for B
 int NCB=atof(argv[3]);      //NCB: number of columns for B
 
 int A[NRA][NCA_RB], 
     B[NCA_RB][NCB], 
     C[NRA][NCB];             /* matrix A and matrix B to be multiplied and stored in resultant matrix C */
 int i, j, k;
 

 //clock_t begin, end;
  struct timespec begin, end;
  double time_spent; 

 printf("Matrix A: #rows %d; #cols %d\n", NRA, NCA_RB);
 printf("Matrix B: #rows %d; #cols %d\n", NCA_RB, NCB);
 printf ("\n");

 printf("Initializing arrays...\n");
 for (i=0; i<NRA; i++)
  for (j=0; j<NCA_RB; j++)
    A[i][j]= i+j;  

 printf (" Contents of matrix A\n");
 for (i=0; i<NRA; i++) {  
  for (j=0; j<NCA_RB; j++)
    printf("%d\t", A[i][j]);
  printf("\n");
  }     
            
 for (i=0; i<NCA_RB; i++)   
  for (j=0; j<NCB; j++)
    B[i][j]= i-j;

 printf (" Contents of matrix B\n");
 for (i=0; i<NCA_RB; i++) {  
  for (j=0; j<NCB; j++)
    printf("%d\t", B[i][j]);
  printf("\n");
  printf("\n");        
  }

 //begin = clock();
 begin = now();
  
 /* Fork a team of threads, each has own copy of variables */
 #pragma omp parallel shared(A,B,C) private(i,j,k,tid)
  {
  tid = omp_get_thread_num();
  //printf("Thread = %d created\n", tid);     
 #pragma omp for 
 for (i=0; i<NRA; i++)
 {
    for (j=0; j<NCB; j++)
    {
    C[i][j] = 0;
    for (k=0; k<NCA_RB; k++)    /* NCA = NRB = NCA_RB*/
        C[i][j] = C[i][j] + A[i][k] * B[k][j];
    }
 }
}
//end = clock();
 end = now();
 time_spent = tdiff(begin, end);
 
 if (tid==0){     /* Only master thread does this */
 
 printf ("\n");
 printf("******************************************************\n");
 printf("Result Matrix:\n");
 for (i=0; i<NRA; i++)
 {
  printf("\n"); 
  for (j=0; j<NCB; j++) 
  printf("%d\t", C[i][j]);
 } 
      
 printf("\n******************************************************\n");
 printf ("\n");
 printf("total time: %.8f sec\n", time_spent);
 printf ("\n");
 }//end if     
}//end main

      
    
   