#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "a2_helper.h"
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <fcntl.h>

pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t specialLock = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t lastLock = PTHREAD_MUTEX_INITIALIZER;

pthread_mutex_t lock1 = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;
pthread_cond_t cond2 = PTHREAD_COND_INITIALIZER;
pthread_cond_t cond3 = PTHREAD_COND_INITIALIZER;

int running_threads = 0;
int flag = 0;
int g=0;
int ended = 0;
typedef struct
{
    int thread_no;
    sem_t *sem1;
    sem_t *sem2;
} TH_STRUCT;

sem_t *x;
sem_t *q;
sem_t *waitt;
sem_t *waity;
int flag1 = 0;
int threads_closed = 0;
int was15 = 0;
int started = 0;

void *threadFn(void *param)
{
    TH_STRUCT *s = (TH_STRUCT *)param;

    if (s->thread_no == 2)
    {
        sem_wait(s->sem1);
        info(BEGIN, 2, s->thread_no);
        info(END, 2, s->thread_no);
        sem_post(s->sem2);
    }
    else if (s->thread_no == 3)
    {
        info(BEGIN, 2, s->thread_no);
        sem_post(s->sem1);
        sem_wait(s->sem2);
        info(END, 2, s->thread_no);
    }
    else if (s->thread_no == 5)
    {
        sem_wait(x);
        info(BEGIN, 2, s->thread_no);

        info(END, 2, s->thread_no);
        sem_post(q);
    }
    else
    {

        info(BEGIN, 2, s->thread_no);
        info(END, 2, s->thread_no);
    }

    return NULL;
}

void *threadFn5(void *param)
{

    // TH_STRUCT *s = (TH_STRUCT *)param;

    // sem_wait(s->sem1);
    // info(BEGIN, 5, s->thread_no);

    // pthread_mutex_lock(&lock);
    // running_threads++;
    // // printf("RUNNING THREADS %d  for thread : %d \n", running_threads, s->thread_no);
    // pthread_mutex_unlock(&lock);

    // if (s->thread_no == 15)
    // {
    //     flag = 1;
    //     while (1)
    //     {
    //         // printf("DIFF IS %d\n", running_threads - threads_closed);
    //         pthread_mutex_lock(&lock1) ;
    //         if (running_threads - threads_closed == 4)
    //         {
    //             flag = 0;
    //             info(END, 5, s->thread_no);
    //             sem_post(s->sem2);
    //             return NULL;
    //             break;
    //         }
    //         pthread_mutex_unlock(&lock1);
    //     }
    // }

    // if (flag != 0)
    // {
    //     sem_wait(s->sem2);
    //     if (s->thread_no != 15)
    //     {
    //         info(END, 5, s->thread_no);
    //         pthread_mutex_lock(&lock);
    //         threads_closed++;
    //         pthread_mutex_unlock(&lock);
    //     }
    // }
    // else
    // {
    //     if (s->thread_no != 15)
    //     {
    //         info(END, 5, s->thread_no);
    //         pthread_mutex_lock(&lock);
    //         threads_closed++;
    //         pthread_mutex_unlock(&lock);
    //     }
    // }

    // sem_post(s->sem1);
    // // printf("THREADS_CLOSED %d for thread number : %d \n", threads_closed, s->thread_no);

    // TH_STRUCT *s = (TH_STRUCT *)param;
    // sem_wait(s->sem1);

    // info(BEGIN, 5, s->thread_no);
    // pthread_mutex_lock(&lock1);
    // running_threads++;
    // if (s->thread_no == 15)
    // {

    //     // started++;
    //     // sem_post(waity);
    //     // sem_post(waity);
    //     // sem_post(waity);
    //     flag = 1;
    //     was15 = 1;
    //     //printf("15 a intrat din while\n");

    //     while (running_threads != 4)
    //     {
    //         pthread_cond_wait(&cond, &lock1);
    //     }
    //     //printf("15 a iesit din while\n");
    //     sem_post(s->sem2);
    //     sem_post(s->sem2);
    //     sem_post(s->sem2);
    // }
    // pthread_mutex_unlock(&lock1);

    // pthread_mutex_lock(&lock);
    // if (flag == 1 && running_threads == 4)
    // {
    //     flag = 0;
    //     pthread_cond_broadcast(&cond);
    // }
    // pthread_mutex_unlock(&lock);

    // // if (threads_closed > 41 && started == 0)
    // // {
    // //     printf("%d s-a intrat aici\n", s->thread_no);
    // //     sem_wait(waity);
    // // }

    // if (was15 == 1 && s->thread_no != 15)
    // {
    //     printf("%d a intrat pe wait\n", s->thread_no);
    //     sem_wait(s->sem2);
    //     //printf("%d pentru thread %d\n", i, s->thread_no);
    //     info(END, 5, s->thread_no);
    //     pthread_mutex_lock(&lock);
    //     running_threads--;
    //     threads_closed++;
    //     pthread_mutex_unlock(&lock);

    //     was15 = 0;
    // }
    // else
    // {
    //     //printf(" %d a intrat pe simplu\n", s->thread_no);
    //     info(END, 5, s->thread_no);

    //     pthread_mutex_lock(&lock);
    //     running_threads--;
    //     threads_closed++;
    //     pthread_mutex_unlock(&lock);
    // }

    // sem_post(s->sem1);

    TH_STRUCT *s = (TH_STRUCT *)param;
    sem_wait(s->sem1);
    info(BEGIN, 5, s->thread_no);

    pthread_mutex_lock(&lock);
    running_threads++;
    flag1++;
    pthread_mutex_unlock(&lock);
   

    if (s->thread_no == 15)
    {
        pthread_mutex_lock(&specialLock);
        was15 = 1;
        started = 1;
        printf("%d a intrat pe while\n", s->thread_no);

        while (running_threads != 4)

        {
            printf("15 saw running  %d; \n", running_threads);
            pthread_cond_wait(&cond, &specialLock);
            break;
        }
        printf("%d a iesit de pe while\n", s->thread_no);
        was15 = 0;
        ended = 1;
        pthread_cond_broadcast(&cond2);
        pthread_cond_broadcast(&cond3);

        pthread_mutex_unlock(&specialLock);
    }
    pthread_mutex_lock(&lock);
    if (running_threads == 4 && was15 != 0)
    {
        pthread_cond_broadcast(&cond);
    }
    pthread_mutex_unlock(&lock);

    pthread_mutex_lock(&lastLock);
    
    while (flag1 >= 41 && started == 0) 
    {
        printf("%d a intrat pe ultimele \n", s->thread_no);
        printf("flag1: %d\n", flag1);
       
        pthread_cond_wait(&cond3, &lastLock);
       
        break;
    }
     pthread_mutex_unlock(&lastLock);
    
    pthread_mutex_lock(&lock1);
    while (was15 != 0 && s->thread_no != 15 && started == 1 && ended == 0)
    {
        printf("%d a intrat pe wait\n", s->thread_no);
        printf("flag1: %d\n", flag1);
        pthread_cond_wait(&cond2, &lock1);
        was15 = 0;
        break;
    }
    pthread_mutex_unlock(&lock1);

    if (s->thread_no == 15)
    {
        pthread_cond_broadcast(&cond2);
        was15 = 0;
    }
    info(END, 5, s->thread_no);
    pthread_mutex_lock(&lock);
    running_threads--;
    threads_closed++;
    pthread_mutex_unlock(&lock);

    sem_post(s->sem1);

    return NULL;
}

void *threadFn3(void *param)
{
    TH_STRUCT *s = (TH_STRUCT *)param;

    if (s->thread_no == 3)
    {
        info(BEGIN, 3, s->thread_no);
        info(END, 3, s->thread_no);
        sem_post(x);
    }
    else if (s->thread_no == 4)
    {

        sem_wait(q);
        info(BEGIN, 3, s->thread_no);
        info(END, 3, s->thread_no);
    }
    else
    {
        info(BEGIN, 3, s->thread_no);
        info(END, 3, s->thread_no);
    }

    return NULL;
}

int main()
{
    init();
    TH_STRUCT params[5];
    pthread_t tid[5];

    TH_STRUCT params5[45];
    pthread_t tid5[45];

    TH_STRUCT params3[4];
    pthread_t tid3[4];

    x = sem_open("x", O_CREAT, 0644, 0);
    q = sem_open("y", O_CREAT, 0644, 0);

    info(BEGIN, 1, 0);

    int pid = fork();
    if (pid == 0)
    {
        info(BEGIN, 2, 0);

        sem_t sem1, sem2;
        sem_t third1, third2;
        sem_init(&sem1, 0, 0);
        sem_init(&sem2, 0, 0);
        sem_init(&third1, 1, 0);
        sem_init(&third2, 1, 0);

        int pid3 = fork();
        if (pid3 == 0)
        {
            info(BEGIN, 3, 0);

            for (int i = 0; i < 4; i++)
            {
                params3[i].thread_no = i + 1;
                params3[i].sem1 = &third1;
                params3[i].sem2 = &third2;
                pthread_create(&tid3[i], NULL, threadFn3, &params3[i]);
            }

            for (int i = 0; i < 4; i++)
            {
                pthread_join(tid3[i], NULL);
            }

            info(END, 3, 0);
            exit(0);
        }
        else
        {

            for (int i = 0; i < 5; i++)
            {
                params[i].thread_no = i + 1;

                if (i == 4)
                {
                    params[i].sem1 = &third1;
                    params[i].sem2 = &third2;
                }
                else
                {
                    params[i].sem1 = &sem1;
                    params[i].sem2 = &sem2;
                }

                pthread_create(&tid[i], NULL, threadFn, &params[i]);
            }

            for (int i = 0; i < 5; i++)
            {
                pthread_join(tid[i], NULL);
            }

            sem_destroy(&sem1);
            sem_destroy(&sem2);
        }
        int pid4 = fork();
        if (pid4 == 0)
        {
            info(BEGIN, 4, 0);

            info(END, 4, 0);
            exit(0);
        }
        int pid5 = fork();

        if (pid5 == 0)
        {
            info(BEGIN, 5, 0);
            sem_t sem5;
            sem_init(&sem5, 0, 4);
            waitt = sem_open("bariera", O_CREAT, 0644, 3);
            waity = sem_open("bariera2", O_CREAT, 0644, 0);

            for (int i = 0; i < 45; i++)
            {
                params5[i].thread_no = i + 1;
                params5[i].sem1 = &sem5;
                params5[i].sem2 = waitt;

                pthread_create(&tid5[i], NULL, threadFn5, &params5[i]);
            }

            for (int i = 0; i < 45; i++)
            {
                pthread_join(tid5[i], NULL);
            }

            sem_destroy(&sem1);
            sem_destroy(&sem2);

            int pid7 = fork();
            if (pid7 == 0)
            {
                info(BEGIN, 7, 0);

                info(END, 7, 0);
                exit(0);
            }
            waitpid(pid7, NULL, 0);

            info(END, 5, 0);
            exit(0);
        }

        int pid6 = fork();

        if (pid6 == 0)
        {
            info(BEGIN, 6, 0);

            info(END, 6, 0);
            exit(0);
        }
        waitpid(pid6, NULL, 0);
        waitpid(pid5, NULL, 0);
        waitpid(pid4, NULL, 0);
        waitpid(pid3, NULL, 0);

        info(END, 2, 0);
        exit(0);
    }

    wait(NULL);

    info(END, 1, 0);
    return 0;
}