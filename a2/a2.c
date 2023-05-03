#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "a2_helper.h"
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>

typedef struct {
    int thread_no;
 } TH_STRUCT;

sem_t sem1, sem2;


void *threadFn(void* param){
    TH_STRUCT *s = (TH_STRUCT*)param;
    info(BEGIN, 2, s->thread_no);


    info(END, 2, s->thread_no);

    return NULL;
}

int main()
{
    init();
    TH_STRUCT params[5];
    pthread_t tid[5];
    
    info(BEGIN, 1, 0);

    int pid = fork();
    if (pid == 0)
    {
        info(BEGIN, 2, 0);
    
        for(int i = 0; i < 5; i++){
            params[i].thread_no = i+1;
            pthread_create(&tid[i], NULL, threadFn, &params[i]);
        }

        for(int i = 0; i < 5; i++){
            pthread_join(tid[i], NULL);
        }

        int pid3 = fork();
        if (pid3 == 0)
        {
            info(BEGIN, 3, 0);

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
