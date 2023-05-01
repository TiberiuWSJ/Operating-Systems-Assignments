#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "a2_helper.h"
#include <stdlib.h>

int main()
{
    init();

    info(BEGIN, 1, 0);

    int pid = fork();
    if (pid == 0)
    {
        info(BEGIN, 2, 0);
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
