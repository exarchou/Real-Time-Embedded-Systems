/* Dimitrios - Marios Exarchou 8805 

   Embedded Systems 2020 

   Aristotle University of Thessaloniki

   Department of Electrical and Computer Engineering

   My version of pthreads consumer and producer problem, with a round buffer of work Functions */


// Libraries
#include <pthread.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h> 
#include <math.h>
#include <sys/time.h>


// Constants
#define QUEUESIZE 10
#define PI 3.14159265
#define P_MAX 1
#define Q_MAX 60
#define MEASUREMENTS 10000


// Struct that includes argument and timestapm.
struct argument
{
	struct timeval time;
  	void *funArg;
};


struct workFunction 
{
  	void * (*work)(void *);
  	void * arg;
};


typedef struct 
{
  	struct workFunction buf[QUEUESIZE];
  	long head, tail;
  	int full, empty;
  	pthread_mutex_t *mut;
  	pthread_cond_t *notFull, *notEmpty;
} queue;


// Global variables
void* testFunctions[5];
void* testArgs[5];
long long structAddresses;
int sum, LOOP;
FILE *fd; // Files to save time statistics.


// Declaration of routines.
void *producer (void *args);
void *consumer (void *args);
queue *queueInit (void);
void queueDelete (queue *q);
void queueAdd (queue *q, struct workFunction in);
void queueDel (queue *q, struct workFunction *out);
void* fun1(void *x);
void* fun2(void *x);
void* fun3(void *x);
void* fun4(void *x);
void* fun5(void *x);


// Main routine.
int main ()
{
  	int i, P, Q;
  	char fileName[30];
  	double arg0 = 0, arg1 = PI/6, arg2 = PI/3, arg3 = PI/2, arg4 = PI;
  	double avgTime;
  	pthread_t pro[P_MAX], con[Q_MAX];
  	struct workFunction LastElement; 
  	queue *fifo;
  	FILE *fp;

  	// Creating a NULL element that means the end of producers.
  	LastElement.work = NULL;
  	LastElement.arg = NULL;

	// Initialization of functions and arguments arrays.
  	testFunctions[0] = fun1;
  	testFunctions[1] = fun2;
  	testFunctions[2] = fun3;
  	testFunctions[3] = fun4;
  	testFunctions[4] = fun5;

  	testArgs[0] = &arg0;
  	testArgs[1] = &arg1;
  	testArgs[2] = &arg2;
  	testArgs[3] = &arg3;
  	testArgs[4] = &arg4;

  	// Open a file to save average elapsed time.
  	fp = fopen("Average_Time_P_Q.txt", "w");

  	printf("Producers-Consumers Problem using a queue of functions.\n");


  	for (P = 1; P <= P_MAX; P = P + 1)
  	{
  		for (Q = 2; Q <= Q_MAX; Q = Q + 2)
  		{

  			printf("%d Producers ~ %d Consumers\n", P, Q);
  			LOOP = floor(MEASUREMENTS / P);
  			sum = 0;
  			fifo = queueInit ();

  			if (fifo ==  NULL) 
  			{
    			fprintf (stderr, "main: Queue Init failed.\n");
    			exit (1);
  			}

  			sprintf(fileName, "Time_Statistics_P_%d_Q_%02d.txt", P, Q);
  			fd = fopen(fileName, "w");

  			for (i = 0; i < P; i++) 
  			{
    			pthread_create (&pro[i], NULL, producer, fifo);
  			}

  			for (i = 0; i < Q; i++) 
  			{
    			pthread_create (&con[i], NULL, consumer, fifo);
  			}

  			for (i = 0; i < P; i++) 
  			{
    			pthread_join (pro[i], NULL);
  			}
  
  			// No more producers.
  			while (!fifo->empty); //  Waiting for the queue to get empty, then adding Last Element.
  			pthread_mutex_lock (fifo->mut);
  			queueAdd (fifo, LastElement); // Banner that informs consumers for the end of producers.
  			pthread_mutex_unlock (fifo->mut);
  			pthread_cond_signal (fifo->notEmpty);

  			for (i = 0; i < Q; i++) 
  			{
    			pthread_join (con[i], NULL);
  			}

  			queueDelete (fifo);
  			avgTime = (double) sum / (P * LOOP);
  			fprintf(fp, "%d\t%d\t%.2lf\n", P, Q, avgTime);
  			fclose(fd);
  			printf("\nAverage Time between Producing and Consuming is %.2lf usec.\n\n", avgTime);
  			printf("Exiting...\n\n\n");
  		}

  	}

  	fclose(fp);
  	return 0;
}

////////////////////////////////////////////////////////////////////////////////////////////////////// PRODUCER /////

// Producer routine.
void *producer (void *q)
{
	int i, num1, num2, x = 0;
	queue *fifo;
  	struct workFunction randomFunction;
  	struct argument *myArg; // This variable points to an address on shared Heap.
  	struct timeval tp;

  	fifo = (queue *)q;
  	myArg = (struct argument *)malloc (LOOP * sizeof (struct argument));
  	if (myArg == NULL) return (NULL);

  	for (i = 0; i < LOOP; i++) 
  	{
    	pthread_mutex_lock (fifo->mut);

    	while (fifo->full) 
    	{
      		printf ("producer: queue FULL.\n");
      		pthread_cond_wait (fifo->notFull, fifo->mut);
    	}

		// Creating random numbers in range [0,4].
    	num1 = rand() % 5; 
    	num2 = rand() % 5;
    	randomFunction.work = testFunctions[num1];
    	myArg[i].funArg = testArgs[num2];
    	// Getting time stamp of the moment that a fucntion is added to queue.
    	gettimeofday(&tp, NULL);
    	myArg[i].time = tp;
    	randomFunction.arg = &myArg[i];

    	// Adding element to queue.
    	queueAdd (fifo, randomFunction);

    	pthread_mutex_unlock (fifo->mut);
    	pthread_cond_signal (fifo->notEmpty);
  	}

  	return (NULL);
}

////////////////////////////////////////////////////////////////////////////////////////////////////// CONSUMER /////

// Consumer routine.
void *consumer (void *q)
{
  	queue *fifo;
	struct workFunction ExecFun, LastElement;
  	struct timeval tc;
  	struct argument *myArg;
  	suseconds_t elapsedTime;

  	LastElement.work = NULL;
  	LastElement.arg = NULL;
  	fifo = (queue *)q;
  	myArg = (struct argument *)malloc (sizeof (struct argument));
  	if (myArg == NULL) return (NULL);

  	while(1) 
  	{
    	pthread_mutex_lock (fifo->mut);

    	while (fifo->empty) 
		{
      		printf ("consumer: queue EMPTY.\n");
      		pthread_cond_wait (fifo->notEmpty, fifo->mut);
    	}

    	queueDel (fifo, &ExecFun);

    	if (ExecFun.work == NULL) //Finded flag that informs for the end of producers.
    	{
      		queueAdd (fifo, LastElement); // Adding banner to queue so other consumers be informed of the end of producers.
      		pthread_mutex_unlock (fifo->mut);
      		pthread_cond_signal (fifo->notEmpty);
      		return (NULL);
    	}

    	myArg = ExecFun.arg;
    	// Getting time stamp of the moment that a fucntion is deleted of queue.
    	gettimeofday(&tc, NULL);
   	 	elapsedTime = (tc.tv_sec * 1000000 + tc.tv_usec) - (myArg->time.tv_sec * 1000000 + myArg->time.tv_usec);
   	 	fprintf(fd, "%ld\n", elapsedTime);

		sum = sum + elapsedTime;

    	pthread_mutex_unlock (fifo->mut);
    	pthread_cond_signal (fifo->notFull);

    	// Execute Function 
    	ExecFun.work(myArg->funArg);
  	}

  	free (myArg);
  	return (NULL);
}


// Functions for editing queue.
queue *queueInit (void)
{
  	queue *q;

  	q = (queue *)malloc (sizeof (queue));
 	if (q == NULL) return (NULL);

  	q->empty = 1;
 	q->full = 0;
 	q->head = 0;
  	q->tail = 0;
  	q->mut = (pthread_mutex_t *) malloc (sizeof (pthread_mutex_t));
 	pthread_mutex_init (q->mut, NULL);
 	q->notFull = (pthread_cond_t *) malloc (sizeof (pthread_cond_t));
 	pthread_cond_init (q->notFull, NULL);
  	q->notEmpty = (pthread_cond_t *) malloc (sizeof (pthread_cond_t));
  	pthread_cond_init (q->notEmpty, NULL);
  
  	return (q);
}


void queueDelete (queue *q)
{
  	pthread_mutex_destroy (q->mut);
  	free (q->mut);  
  	pthread_cond_destroy (q->notFull);
  	free (q->notFull);
  	pthread_cond_destroy (q->notEmpty);
  	free (q->notEmpty);
  	free (q);
}


void queueAdd (queue *q, struct workFunction in)
{
  	q->buf[q->tail] = in;
  	q->tail++;
  	if (q->tail == QUEUESIZE)
    	q->tail = 0;
  	if (q->tail == q->head)
    	q->full = 1;
  	q->empty = 0;

  	return;
}


void queueDel (queue *q, struct workFunction *out)
{
  	*out = q->buf[q->head];

  	q->head++;
  	if (q->head == QUEUESIZE)
    	q->head = 0;
  	if (q->head == q->tail)
    	q->empty = 1;
  	q->full = 0;

  	return;
}


// Testing functions.

void* fun1(void* x) 
{
  	printf("This is just a message from printf!\n");

  	return (NULL);
}


void* fun2(void* x) 
{
  	double *init, *result;
 	init = (double *)x;
  	double square = (*init) * (*init);

  	result = (double *)malloc (sizeof (double)); // The address is referred to heap memory, where all threads have access.
  	if (result == NULL) return (NULL);

  	result = &square;

  	return (result);
}


void* fun3(void* x)
{
  	double *rads, *result;
  	rads = (double *)x;
  	double mysin = sin(*rads);

  	result = (double *)malloc (sizeof (double));
  	if (result == NULL) return (NULL);

  	result = &mysin;

  	return (result);
}


void* fun4(void* x) 
{
  	double *rads, *result;
  	rads = (double *)x;
  	double mycos = cos(*rads);

  	result = (double *)malloc (sizeof (double));
  	if (result == NULL) return (NULL);

  	result = &mycos;

  	return (result);
}


void* fun5(void* x) 
{
  	double *rads, *result;
  	rads = (double *)x;
  	double mytan = tan(*rads);

  	result = (double *)malloc (sizeof (double));
  	if (result == NULL) return (NULL);

  	result = &mytan;

  	return (result);
}

