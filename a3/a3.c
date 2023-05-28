#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <pthread.h>
#include <sys/mman.h>

int main(int argc, char **argv)
{

    // unlink("RESP_PIPE_99475");

    if (0 != mkfifo("RESP_PIPE_99475", 0600))
    {
        printf("ERROR\ncannot create the response pipe\n");
        return -1;
    }

    int reqPipe = open("REQ_PIPE_99475", O_RDONLY);
    if (reqPipe == -1)
    {
        printf("ERROR\ncannot open the request pipe\n");
        return -1;
    }
    int respPipe = open("RESP_PIPE_99475", O_WRONLY);

    char dim = 5;

    write(respPipe, &dim, 1);

    if (5 == write(respPipe, "START", 5))
    {
        printf("SUCCESS\n");
    }

    while (1)
    {
        char dim1 = 0;

        read(reqPipe, &dim1, 1);
        char *contents = (char *)calloc(dim1+1, sizeof(char));
        read(reqPipe, contents, dim1);
        // write(respPipe, &dim1, 1);
        // write(respPipe, contents, dimInt);
        //contents[dim1] = '\0';

        if (strncmp("VARIANT", contents, dim1) == 0)
        {
            char variantDim = 7;
            write(respPipe, &variantDim, 1);
            write(respPipe, "VARIANT", variantDim);
            char valueDim = 5;
            write(respPipe, &valueDim, 1);
            write(respPipe, "VALUE", valueDim);
            unsigned int number = 99475;
            write(respPipe, &number, 4);
        }

        else if (strncmp("CREATE_SHM", contents, dim1) == 0)
        {
            unsigned int number = -1;
            read(reqPipe, &number, sizeof(unsigned int));
            int shmD = shm_open("/UZ5I4DNT", O_CREAT | O_RDWR, 0644);
            if (shmD < 0)
            {
                char size = 5;
                write(respPipe, &size, 1);
                write(respPipe, "ERROR", size);
            }
            if (ftruncate(shmD, number) == -1)
            {
                char size = 5;
                write(respPipe, &size, 1);
                write(respPipe, "ERROR", size);
            }
            void *addr = mmap(NULL, number, PROT_READ | PROT_WRITE, MAP_SHARED, shmD, 0);
            if (addr == MAP_FAILED)
            {
                char size = 5;
                write(respPipe, &size, 1);
                write(respPipe, "ERROR", size);
            }
            char sizeCreate = 10;
            write(respPipe, &sizeCreate, 1);
            write(respPipe, "CREATE_SHM", sizeCreate);

            char size = 7;
            write(respPipe, &size, 1);
            write(respPipe, "SUCCESS", size);
        }

        if (strncmp("EXIT", contents, dim1) == 0)
        {
            // char size = 5;
            // write(respPipe, &size, 1);
            // write(respPipe, "12345", size);
            close(reqPipe);
            close(respPipe);
            unlink("RESP_PIPE_99475");
            free(contents);
            return 0;
        }

        else
        {
           return 0;
           free(contents);
        }

        free(contents);
    }

    return 0;
}