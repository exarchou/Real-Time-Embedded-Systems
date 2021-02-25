/* Dimitrios - Marios Exarchou

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
#define P_MAX 3
#define Q_MAX 60
#define TIME_IN_SEC 3600


// Struct of Timer.
typedef struct
{
    int Period, TasksToExecute, StartDelay;
    void *(*StartFcn)(void *);
    void *(*StopFcn)(void *);
    void *(*TimerFcn)(void *);
    void *(*ErrorFcn)(void *);
    void *UserData;
} Timer;


// Struct that includes argument, timestapm & Timer.
struct argument
{
	struct timeval time;
  	void *funArg;
    Timer t;
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



// Struct of Producer's argument.
typedef struct
{
	queue* queue;
	Timer timer;
} ProdArg;




// Global variables
void *testFunctions[5];
void *testArgs[5];
long long structAddresses;
int sum, LOOP, Q = 10;
FILE *fd1, *fd2, *fd3, *fp1, *fp2, *fp3; // Files to save time statistics.
pthread_t pro[P_MAX], con[Q_MAX];
ProdArg myProdArg[3];
queue *fifo;
int counter = 0;



// Declaration of routines.
void *producer (void *args);
void *consumer (void *args);

queue *queueInit (void);
void queueDelete (queue *q);
void queueAdd (queue *q, struct workFunction in);
void queueDel (queue *q, struct workFunction *out);

void *fun1(void *x);
void *fun2(void *x);
void *fun3(void *x);
void *fun4(void *x);
void *fun5(void *x);

void *StartFunction(void *arg);
void *StopFunction(void *arg);
void *ErrorFunction(void *arg);
void timerInit(Timer *t, int Period, int TasksToExecute, int StartDelay, void *(*StartFcn)(void *), void *(*StopFcn)(void *), void *(*TimerFcn)(void *), void *(*ErrorFcn)(void *), void *UserData);
void start(Timer t);




// Execution's startpoint.
int main ()
{
    int i, P;
    double arg0 = 0, arg1 = PI/6, arg2 = PI/3, arg3 = PI/2, arg4 = PI;
    struct timeval tc;
    suseconds_t initTime, endTime;
    Timer t1, t2, t3;
    // Creating a NULL element that means the end of producers.
    struct workFunction LastElement;
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


    // Initialize Queue.
    fifo = queueInit();
    if (fifo ==  NULL) 
    {
        fprintf (stderr, "main: Queue Init failed.\n");
        exit (1);
    }

    // Start Consumers.
    for (i = 0; i < Q; i++) 
    {
        pthread_create (&con[i], NULL, consumer, fifo);
    }


    // Open a file to save in-queue time and timedrift.
    fd1 = fopen("In-Queue-time-4-1.txt", "w");
    fp1 = fopen("Timedrift-4-1.txt", "w");
    fd2 = fopen("In-Queue-time-4-2.txt", "w");
    fp2 = fopen("Timedrift-4-2.txt", "w");
    fd3 = fopen("In-Queue-time-4-3.txt", "w");
    fp3 = fopen("Timedrift-4-3.txt", "w");

    printf("\n=======================================================\n");
    printf("Producers-Consumers Problem using timers.\n\n © Dimitrios-Marios Exarchou\n\n");


    // Create Timers
    timerInit(&t1, 1000000, ceil(TIME_IN_SEC*1), 0, StartFunction, StopFunction, testFunctions[2], ErrorFunction, testArgs[3]);
    timerInit(&t2, 100000, ceil(TIME_IN_SEC*10), 0, StartFunction, StopFunction, testFunctions[2], ErrorFunction, testArgs[3]);
    timerInit(&t3, 10000, ceil(TIME_IN_SEC*100), 0, StartFunction, StopFunction, testFunctions[2], ErrorFunction, testArgs[3]);

    printf("=======================================================\n\n");
    //usleep(5000000);

    // Measure init time.
    gettimeofday(&tc, NULL);
    initTime = tc.tv_sec * 1000000 + tc.tv_usec;

    // Start Timers
    start(t1);
    start(t2);
    start(t3);


    // Joining producer threads.
    for (i = 0; i < counter; i++) 
    {
        pthread_join (pro[i], NULL);
    }

    // No more producers.
    while (!fifo->empty); //  Waiting for the queue to get empty, then adding Last Element.
    pthread_mutex_lock (fifo->mut);
    queueAdd (fifo, LastElement); // Banner that informs consumers for the end of producers.
    pthread_mutex_unlock (fifo->mut);
    pthread_cond_signal (fifo->notEmpty);

    // Joining consumer threads.
    for (i = 0; i < Q; i++) 
    {
        pthread_join (con[i], NULL);
    }

    // Measure end time.	
    gettimeofday(&tc, NULL);
    endTime = tc.tv_sec * 1000000 + tc.tv_usec;

    // Delete Queue.
    queueDelete(fifo);
    printf("\nExiting...\n\n");

    printf("Total Execution Time: %f sec\n", (float)(endTime - initTime) / 1000000);
    printf("Time divergence: %lf sec\n", (double)((endTime - initTime) - t3.TasksToExecute * t3.Period) / 1000000);

    fclose(fp1);
    fclose(fd1);
    fclose(fp2);
    fclose(fd2);
    fclose(fp3);
    fclose(fd3);

    return 0;
}



// Producer routine.
void *producer (void *q)
{
    int i, num1, num2, x = 0, sleep_time, timedrift;
    suseconds_t initTime, endTime, elapsedTime;
    queue *fifo;
    ProdArg argumentStruct;
    struct workFunction randomFunction;
    struct argument *myArg; // This variable points to an address on shared Heap.
    struct timeval tc;

    argumentStruct = *((ProdArg *) q);
    fifo = (queue *) argumentStruct.queue;
    myArg = (struct argument *) malloc(argumentStruct.timer.TasksToExecute * sizeof (struct argument));

    if (myArg == NULL) return (NULL);

    gettimeofday(&tc, NULL);
    initTime = tc.tv_sec * 1000000 + tc.tv_usec;
    initTime = initTime - argumentStruct.timer.Period;

    sleep_time = argumentStruct.timer.Period; // initial sleep time

    for (i = 0; i < argumentStruct.timer.TasksToExecute; i++) 
    {
        printf("%d \n", i);

        pthread_mutex_lock (fifo->mut);

        while (fifo->full) 
        {
            printf ("producer: queue FULL.\n");
            pthread_cond_wait (fifo->notFull, fifo->mut);
        }

        // Creating random numbers in range [0,4].
        num1 = rand() % 5; 
        num2 = rand() % 5;
        //randomFunction.work = testFunctions[0]; //// t.TimerFcn = fun1;
        randomFunction.work = argumentStruct.timer.TimerFcn;
        myArg[i].funArg = testArgs[num2];

        // Getting time stamp of the moment that a fucntion is added to queue.
        gettimeofday(&tc, NULL);
        myArg[i].time = tc;
        myArg[i].t = argumentStruct.timer;

        randomFunction.arg = &myArg[i];

        queueAdd (fifo, randomFunction);

        pthread_mutex_unlock (fifo->mut);
        pthread_cond_signal (fifo->notEmpty);

        endTime = tc.tv_sec * 1000000 + tc.tv_usec;
        elapsedTime = endTime - initTime;
        initTime = endTime;

        timedrift = elapsedTime - argumentStruct.timer.Period;

        if (argumentStruct.timer.Period == 1000000)
        {
            fprintf(fp1, "%d\n", timedrift);
        }
        else if (argumentStruct.timer.Period == 100000)
        {
            fprintf(fp2, "%d\n", timedrift);
        }
        else 
        {
            fprintf(fp3, "%d\n", timedrift);
        }

        //printf("Timedrift: %d usec\n", timedrift);
        sleep_time = sleep_time - timedrift;
        //printf("Sleep Time: %d usec\n", sleep_time);
        usleep(sleep_time);	
    }

    return (NULL);
}



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
            // printf("Found NULL\n");
            return (NULL);
        }

        myArg = ExecFun.arg;
        // Getting time stamp of the moment that a fucntion is deleted of queue.
        gettimeofday(&tc, NULL);
        elapsedTime = (tc.tv_sec * 1000000 + tc.tv_usec) - (myArg->time.tv_sec * 1000000 + myArg->time.tv_usec);

        if (myArg->t.Period == 1000000)
        {
            fprintf(fd1, "%ld\n", elapsedTime);
        }
        else if (myArg->t.Period == 100000)
        {
            fprintf(fd2, "%ld\n", elapsedTime);
        }
        else
        {
            fprintf(fd3, "%ld\n", elapsedTime);
        }
        
        //printf("In-Queue Time: %ld usec\n", elapsedTime);
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
void *fun1 (void *x) 
{
	printf("This is just a message from printf!\n");
	return (NULL);
}


void *fun2 (void *x) 
{
    double *init, *result;
    init = (double *)x;
    double square = (*init) * (*init);

    result = (double *)malloc (sizeof (double)); // The address is referred to heap memory, where all threads have access.
    if (result == NULL) return (NULL);
    result = &square;

    return (result);
}


void *fun3 (void *x)
{
    double *rads_pointer, *result;
    rads_pointer = (double *)x;
    double rads_value = *rads_pointer;
    double mysin = sin(rads_value);

    result = (double *)malloc (sizeof (double));
    if (result == NULL) return (NULL);
    result = &mysin;

    printf("sin(%.3lf) = %.3lf\n", rads_value, mysin);

    return (result);
}


void *fun4 (void *x) 
{
    double *rads, *result;
    rads = (double *)x;
    double mycos = cos(*rads);

    result = (double *)malloc (sizeof (double));
    if (result == NULL) return (NULL);
    result = &mycos;

    return (result);
}


void *fun5 (void *x) 
{
    double *rads, *result;
    rads = (double *)x;
    double mytan = tan(*rads);

    result = (double *)malloc (sizeof (double));
    if (result == NULL) return (NULL);
    result = &mytan;

    return (result);
}



// Timer Functions
void *StartFunction (void *arg)
{
    printf("Start Function: Data Initialization.\n");
    return NULL;
}


void *StopFunction (void *arg)
{
    printf("Stop Function: Session ended.\n");
    return NULL;
    }


void *ErrorFunction (void *arg)
{
    printf("Error Function: Queue is full.\n");
    return NULL;
}


void timerInit (Timer *t, int Period, int TasksToExecute, int StartDelay, void *(*StartFcn)(void *), void *(*StopFcn)(void *), void *(*TimerFcn)(void *), void *(*ErrorFcn)(void *), void *UserData)
{
    printf("Timer Initialization: Period: %d us,  Tasks: %d\n", Period, TasksToExecute);
    t->Period = Period;
    t->TasksToExecute = TasksToExecute;
    t->StartDelay = StartDelay;
    t->StartFcn = StartFcn;
    t->TimerFcn = TimerFcn;
    t->ErrorFcn = ErrorFcn;
    t->StopFcn = StopFcn;

    return;
}


void start (Timer t)
{
    myProdArg[counter].queue = fifo;
    myProdArg[counter].timer = t;

    usleep(t.StartDelay);
    printf("Starting Timer %d...\n", counter);
    pthread_create (&pro[counter], NULL, producer, &myProdArg[counter]);
    counter++;

    return;
}


void startat (Timer t, int y, int m, int d, int h, int min, int sec)
{
	int delay;
	double delay_sec;
	time_t now;
	struct tm info;
	now = time(NULL);
	info = *localtime(&now);
	info.tm_year = y - 1900;
	info.tm_mon = m - 1;
	info.tm_mday = d;
	info.tm_hour = h;
	info.tm_min = min;
	info.tm_sec = sec;
	delay_sec = difftime(mktime(&info), now);
	delay = (int) delay_sec;

	if (delay >= 0)
	{
		printf("Waiting for %0.f seconds\n", delay_sec);
		sleep(delay);
		start(t);
	}
	else 
	{
		printf("Invalid Future timestamp!\n");
	}

	return;
}