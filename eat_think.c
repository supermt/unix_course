#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <string.h>
#include <semaphore.h>
#include <sys/types.h>
#include <errno.h>
#include <unistd.h>
#include <signal.h>

#define NUM_THREADS_P 5
#define CHAIR_NUM 4
#define CHOP_NUM  5

int chairflg[CHAIR_NUM][2],dining_num = 0;
sem_t chair,chopstics[CHOP_NUM],mutex,mutex1,print_mutex;
pthread_t threads_p[NUM_THREADS_P];

void *philosopher_thread(void *args);

int main(void)
{
	int i;
	
	sem_init(&chair, 0, CHAIR_NUM);
	for(i=0; i<CHOP_NUM; i++)
	{
		sem_init(&chopstics[i], 0, 1);
	}
	sem_init(&mutex, 0, 1);
	sem_init(&mutex1, 0, 1);
	sem_init(&print_mutex, 0, 1);
	for(i=0; i<CHAIR_NUM; i++)
		chairflg[i][0] = 0;
		
	for(i=0; i<NUM_THREADS_P; i++)
	{
		pthread_create(&threads_p[i], NULL, philosopher_thread, (void *)i);
	}	
	for(i=0; i<NUM_THREADS_P; i++)
	{
		pthread_join(threads_p[i], NULL);
	}
	
	// sem_destroy(&chair);
	// sem_destroy(&chopstics[CHOP_NUM]);
	// sem_destroy(&mutex);
	// sem_destroy(&mutex1);
	// sem_destroy(&print_mutex);
	
	return 0;
}

void *philosopher_thread(void *args)
{
	while(1)
	{
	int i,j;
	i = (int)args;
	printf("philosopher %d is thinking\n",i);
	sleep(rand()%3);
	sem_wait(&chair);
	sem_wait(&mutex);
	
	for(j=0; j<CHAIR_NUM;j++)
	{
		if(chairflg[j][0] == 0)
		{
			chairflg[j][0] = 1;
			chairflg[j][1] = i;
			break;
		}
	}
	dining_num++;
	sem_post(&mutex);
	sem_wait(&chopstics[i]);
	printf("philosopher %d get chopstics %d\n",i,i);
	sem_wait(&chopstics[(i+1)%CHOP_NUM]);
	printf("philosopher %d get chopstics %d\n",i,(i+1)%CHOP_NUM);
	
	sleep(rand()%3);
	sem_wait(&print_mutex);
	printf("philosopher %d is dining,when he is dining,there are %d philosophers at table.\n",i,dining_num);
	
	for(j=0; j<CHAIR_NUM; j++)
	{
		if(chairflg[j][1] == i)
		{
			printf("philosopher %d in chair %d\n",i,j);
		}
	}
	printf("\n\n");
	sem_post(&print_mutex);
	sleep(rand()%3);
	sem_post(&chopstics[i]);
	sem_post(&chopstics[(i+1)%CHOP_NUM]);
	sem_wait(&mutex1);
	
	for(j=0; j<CHAIR_NUM; j++)
	{
		if(chairflg[j][1] == i)
		{
			chairflg[j][0] = 0;
			break;
		}
	}
	dining_num--;
	sem_post(&mutex1);
	sem_post(&chair);
	printf("philosopher %d leaves\n",i);
	
	}
}