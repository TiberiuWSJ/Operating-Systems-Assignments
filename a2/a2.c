#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "a2_helper.h"
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <fcntl.h>
typedef struct
{
    int thread_no;
    sem_t *sem1;
    sem_t *sem2;
} TH_STRUCT;

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
        // sem_wait(s->sem1);
        // sem_wait(x);
        info(BEGIN, 2, s->thread_no);

        info(END, 2, s->thread_no);
        // sem_post(q);
        // sem_post(s->sem2);
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
    TH_STRUCT *s = (TH_STRUCT *)param;
    sem_wait(s->sem1);
    info(BEGIN, 5, s->thread_no);
    info(END, 5, s->thread_no);
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
    }
    else if (s->thread_no == 4)
    {
        // sem_wait(s->sem2);
        // sem_wait(q);

        info(BEGIN, 3, s->thread_no);
        info(END, 3, s->thread_no);
        // sem_post(s->sem2);
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

            for (int i = 0; i < 45; i++)
            {
                params5[i].thread_no = i + 1;
                params5[i].sem1 = &sem5;
                params5[i].sem2 = &sem2;

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
