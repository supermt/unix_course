#include <stdio.h>
#include <semaphore.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

#define N1 3
#define N2 4
#define M 10

int in = 0;
int out = 0;

int buff[M] = {0};

sem_t empty_sem;
sem_t full_sem;
pthread_mutex_t mutex;

int product_id = 0;
int consumer_id = 0;

int data;
FILE *fp;

void * product()
{
    int id = ++product_id;
    while(1)
    {
        sleep(1);
        sem_wait(&empty_sem);
        pthread_mutex_lock(&mutex);
        //if(feof(fp) != 0) fseek(fp, 0, SEEK_SET);
        if(fscanf(fp, "%d", &data)==EOF)
        {
            fseek(fp, 0, SEEK_SET);
            fscanf(fp, "%d", &data);
        }
        in = in % M;
        buff[in] = data;
        printf("Producter %d produce %d in position %d\n", id, buff[in], in);
        ++in;
        pthread_mutex_unlock(&mutex);
        sem_post(&full_sem);
    }
}


void *consume()
{
    int id = ++consumer_id;

    while(1)
    {
        sleep(1);
        sem_wait(&full_sem);
        pthread_mutex_lock(&mutex);

        out = out % M;
        printf("Consumer %d take product  %d in position %d\n", id, buff[out], out);
        buff[out] = 0;
        ++out;

        pthread_mutex_unlock(&mutex);
        sem_post(&empty_sem);

    }
}


int main()
{
    pthread_t id1[N1];
    pthread_t id2[N2];

    int i;
    int ret1[N1];
    int ret2[N2];

    int ini1 = sem_init(&empty_sem, 0, M);
    int ini2 = sem_init(&full_sem, 0, 0);

    int ini3 = pthread_mutex_init(&mutex, NULL);
    if(ini3 != 0 )
    {
        printf("mutex init failed\n");
        exit(1);
    }

    fp = fopen("./data.txt", "r");
    if(fp == NULL) exit(1);

    for(i = 0; i < N1; i++)
    {
        ret1[i] = pthread_create(&id1[i], NULL, product, (void*)(&i));
        if(ret1[i] != 0)
        {
            printf("product%d creat failed\n", i);
            exit(1);
        }
    }

    for(i = 0; i< N2; i++)
    {
        ret2[i] = pthread_create(&id2[i], NULL, consume, NULL);
        if(ret2[i] != 0)
        {
            printf("consumer%d creat failed\n", i);
            exit(1);
        }
    }


    for(i = 0; i < N1; i++) {pthread_join(id1[i], NULL);}
    for(i = 0; i < N2; i++) {pthread_join(id2[i], NULL);}

    exit(0);


}
