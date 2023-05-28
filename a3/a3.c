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
        char dim = 0;

        read(reqPipe, &dim, 0);
        int dimInt = atoi(&dim);
        char *contents = (char *)calloc(dimInt + 1, sizeof(char));
        read(reqPipe, &contents, dimInt);
        contents[dimInt] = '\0';
        if (strncmp("VARIANT", contents, dimInt) == 0)
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
        if (strncmp("EXIT", contents, dimInt) == 0)
        {
            close(reqPipe);
            close(respPipe);
            unlink("RESP_PIPE_99475");
            return 0;
        }
        else{
            return 0;
        }
        
    }

    return 0;
}