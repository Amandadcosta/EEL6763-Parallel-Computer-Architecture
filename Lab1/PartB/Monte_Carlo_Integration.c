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


#define PI 3.14               /*setting the value of pi*/
#define MASTER 0               /* taskid of first task */
#define FROM_MASTER 1          /* setting a message type */
#define FROM_WORKER 2          /* setting a message type */
//#define RAND_MAX

int numtasks,            /* number of tasks in partition */
    taskid,                /* a task identifier */
    numworkers,		/* number of worker tasks */
    columns,
    extra,
    mtype=2,
    myN;            
double result;

//clock_t begin, end;
struct timespec begin, end;
double time_spent;

    

void init_rand_seed (){     //seed generation
    static long long int temp=1;
    static int visited=0;
    if(temp==1){
    	temp=2;
    	return;
    }
    else if(temp==2 && visited==0){
    	temp=taskid*myN;
    	visited=1;
    }
    srand(temp);
    temp++;
    }      

double estimate_g(double lower_bound, double upper_bound, long long int N)
{       
    double partial_sum=0.0,z,h;
    
    double g_of_x=0.0;
    
    MPI_Comm_rank(MPI_COMM_WORLD,&taskid);
    MPI_Comm_size(MPI_COMM_WORLD,&numtasks);
    columns = N/numtasks;
    extra = N%numtasks;
    myN = (taskid <= extra) ? columns+1 : columns;

    printf("Number of elements for rank %d is : %d\n",taskid,myN);
    

    begin=now();
    
    for(int i = 0; i < myN; i++){
	init_rand_seed();
        float x=(upper_bound-lower_bound)*((float)rand() / RAND_MAX) + lower_bound;    //random number between 0 and 1 converted to the range (a,b) ///
        //printf("x=%lf\n",x);
        z= (8 * sqrt( 2 * PI )) /( exp(pow (( 2 * x ) , 2)));   		       //z=f(x)
	  
	partial_sum+=z;   
	
	
    }
    
    g_of_x = partial_sum*((float)(upper_bound-lower_bound)/(float)N);        
    printf("G(x) is %lf from rank %d\n",g_of_x,taskid); 
    return g_of_x;
}


void collect_results(double *result){
    //MPI_Comm_rank(MPI_COMM_WORLD,&taskid);
    //MPI_Comm_size(MPI_COMM_WORLD,&numtasks);
    /**************************** master task ************************************/
    if(taskid==MASTER){
        mtype = FROM_WORKER;
        double temp_gfx;
        printf("Number of workers: %d\n",numtasks);
        for (int i=1; i<numtasks; i++)
        {
         int source = i;        
         MPI_Status status;
	 MPI_Recv(&temp_gfx, 1, MPI_DOUBLE, source,mtype, MPI_COMM_WORLD, &status);
	 printf("Received results from task %d\n",source);
         *result+=temp_gfx;
        }
      end=now();
      time_spent=tdiff(begin,end);
      
      printf("\n******************************************************\n");

      printf("Estimated result = %lf \n",*result);
      printf("Time taken = %.8f sec\n", time_spent);

      printf("\n******************************************************\n");

    }
/**************************** worker task ************************************/
    else 
	if(taskid>MASTER)  //worker rank sends to master
    {
	double gfx= *result;
	
        MPI_Send(&gfx, 1, MPI_DOUBLE, MASTER, mtype, MPI_COMM_WORLD);
        printf("Partial results sent back by rank %d is:%lf\n",taskid,gfx);
        //MPI_Send(*result, 1, MPI_DOUBLE, source, mtype, MPI_COMM_WORLD, &status);
    }
}

int main(int argc, char **argv)
{
    MPI_Init(&argc, &argv);
    
    float lower_bound = atof(argv[1]);      // value of a from command line
    float upper_bound = atof(argv[2]);      //value of b from command line
    long long int N = atof(argv[3]);	    //value of N from command line
    init_rand_seed(); // using srand()
    
    result = estimate_g(lower_bound, upper_bound, N);
    collect_results(&result);
    MPI_Finalize();
    return 0;
}
