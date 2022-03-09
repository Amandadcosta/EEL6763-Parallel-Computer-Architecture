/*Amanda Kelly Dcosta */
#include "mpi.h"
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

#define MASTER 0               /* taskid of first task */
#define FROM_MASTER 1          /* setting a message type */
#define FROM_WORKER 2          /* setting a message type */

void initialize_data (int *A, int N);
int distribute_data (int *A, int N);
void mask_operation (int *rec_buf, int N);
void collect_results (int *rec_buf2, int N);
int rows,
    myrows,             //number of rows for a particular worker rank
    mtype=FROM_WORKER,    
    rank, R,         // for storing this process' rank, and the number of processes
    sum = 0,            // Sum of counts. Used to calculate displacements
    i, j, k,
    N,
    rem;            
int *A;
int *rec_buf, 
    *Ap,          //buffer where received data is stored and then processed
    *sendcounts,
    *displs,
    *temp,
    *rec_buf2;
int rec_buf_size;
int *new_sendcounts;
int *new_displs;

//clock_t begin, end;
struct timespec begin, end;
double time_spent;



int main(int argc, char **argv)
{ 
N = atof(argv[1]);
MPI_Init(&argc, &argv);
MPI_Comm_rank(MPI_COMM_WORLD, &rank);
MPI_Comm_size(MPI_COMM_WORLD, &R);    
A = (int*)malloc(N*N*sizeof(int));		      //single dimensional data array of length N^2
Ap = (int *)malloc((N)*(N)*sizeof(int));          // array describing how many elements to send to each process 
temp=(int *)malloc((N-2)*(N)*sizeof(int));   

initialize_data(A, N);
rec_buf_size=distribute_data (A, N);             // use scatterv

mask_operation(rec_buf, N);
collect_results(rec_buf2, N);                   // use gatherv
MPI_Finalize();
 
return 0;
}

void initialize_data (int *A, int N)
{ 
    srand(1);
if (0 == rank) {
    begin=now();
    for(int i=0;i<N;i++){
        for(int j=0;j<N;j++)
            *((A+i*N)+j)=(rand()%256);}                 //passing 2 dimensional array as argument to function
    
 }
}
int distribute_data (int *A, int N)
{
    sendcounts =(int*) malloc(sizeof(int)*R);          // array describing how many elements to send to each process
    displs = (int*)malloc(sizeof(int)*R);              // array describing the displacements where each segment begins
    sum=0;
    rem = (N-2)%R;     //number of extra rows
    
    for (i = 0; i < R; i++) {

        sendcounts[i] = (rem>0) ? (N/R + 1)*N : (N/R)*N ;
        sendcounts[i]=sendcounts[i]+(2*N);     	     // number of elements= myrow elements +2 extra rows(overlapping with other worker ranks)
        if(rem>0){
          rem--;}
        

	displs[i] = sum;
        sum += (sendcounts[i]-2*N);
    }
    
    if(rank==MASTER){
    //printf ("\n**********************************sendcounts[%d]: %d\n",rank, sendcounts[rank]);
    
    for (i = 0; i < R; i++) {
        printf("sendcounts[%d] = %d\tdispls[%d] = %d\n", i, sendcounts[i], i, displs[i]);
        }
 	}
    rec_buf=(int*)malloc(sizeof(int)*(sendcounts[rank]));                                //1D array of rec_buf_size
    


    // divide the data among processes as described by sendcounts and displs
    MPI_Scatterv(A, sendcounts, displs, MPI_INT, rec_buf,sendcounts[rank], MPI_INT, 0, MPI_COMM_WORLD);
    
    
    //print what each process received
    printf("***************Received by rank %d:*************************\n", rank);
    for (int i = 0; i < ((sendcounts[rank])/N); i++) {
        for(int j = 0; j < N; j++){
            printf("%d\t", rec_buf[i*N+j]);
	}
        printf("\n");
    }
   
   return sendcounts[rank];
   }
void mask_operation(int *rec_buf, int N)
{
   // printf("rec_buf_size=========== %d\t", rec_buf_size);
rec_buf2 = (int *)malloc(rec_buf_size*sizeof(int));
						
int myrows = rec_buf_size/N;		//same as sendcounts[rank]/N
int mycolumns= N-2;
int mysize=myrows*mycolumns;
int* arr=(int*) malloc((N-2)*(myrows-2)*sizeof(int));
int count=0;
for (int x = 0; x < myrows-2; x++) 
{
    for (int y = 0; y < N-2; y++) 
    {
        int sum1=0;
        for (int r = x; r < x+3; r++) 		// creating a 3x3 sliding window
        {       
            for (int c = y; c < y+3; c++) 
                sum1+=*(rec_buf+r*N+c);
        }
     arr[count]=(sum1+*(rec_buf+((x+1)*N+(y+1))))/10; 
     count++;
    //*(rec_buf2+x*(N-2)+y)=(sum1+*(rec_buf+(x+1)*N+(y+1)))/10;               //remember to set the dimensions right
        
    }
}
// printf("\nFinal processed Masked Matrix Calculated for rank:%d\n",rank);
int k=0;
printf("\n------------------rank%d----------------\n",rank);

for(int i = 0 ; i < myrows-2 ; i++){
   if(k==mysize){
	break;}
    for(int j = 0 ; j < N; j++){
    if(j==0 || j==N-1 )
        *(rec_buf2+i*N+j)=0;
    else    
	{
        *(rec_buf2+i*N+j)=arr[k];
        k++;
        }
	
    printf("%d\t",*(rec_buf2+i*N+j));	
    
	}			     
    printf("\n");
}


    
}

void collect_results (int *rec_buf2, int N)
{
    new_sendcounts = (int*)malloc(sizeof(int)*R);          // array describing how many elements to send to each process
    new_displs = (int*)malloc(sizeof(int)*R);              // array describing the displacements where each segment begins
    int sum2=0;
    for(int i=0;i<R;i++){
     
     new_sendcounts[i]=sendcounts[i]-(2*N);
     new_displs[i] = sum2;
     sum2 += new_sendcounts[i];
     
    }
    
    MPI_Gatherv(rec_buf2, new_sendcounts[rank], MPI_INT, temp, new_sendcounts, new_displs, MPI_INT,  0, MPI_COMM_WORLD); 
    
    
    								
    if(rank==MASTER){
    printf ("\n");
    printf ("**********************%d X %d data array*******************", N, N);
    printf ("\n");       
    
    for (i = 0; i < N; i++) {
        for (j = 0; j < N; j++) {
            if(i==0 || i==(N-1))
            	*(Ap +i*N+j)=0;							//padding row of zeros to top and bottom of matrix Ap to avoid extra for loops
            else
                *(Ap +i*N+j)=*(temp+(i-1)*N+j);						
            printf ("%d\t", *(A+i*N+j));
            }printf ("\n");
        }
        
    printf ("*******************************************************************");
    printf ("\n");
    printf ("\n");
    printf ("*****************%d X %d Updated data array****************", N, N);
    printf ("\n");       
 

    for (i = 0; i < N; i++) {
        for (j = 0; j < N; j++) {
            
            printf ("%d\t", *(Ap +i*N+j));
        }printf ("\n");
    }
      end=now();
      time_spent=tdiff(begin,end);
      
      printf("\n******************************************************\n");

      
      printf("Time taken = %.8f sec\n", time_spent);

      printf("\n******************************************************\n");
    }
    
}
    