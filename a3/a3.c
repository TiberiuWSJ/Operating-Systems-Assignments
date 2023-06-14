#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <pthread.h>
#include <sys/mman.h>

char *addr;

void write_shm(int reqPipe, int respPipe)
{
    // printf("Aici\n");
    unsigned int offset = -1;
    read(reqPipe, &offset, 4);
    if (offset >= 0 && offset <= 1619935)
    {
        unsigned int value = 0;
        read(reqPipe, &value, 4);
        if (offset + sizeof(value) >= 0 && offset + sizeof(value) <= 1619935)
        {
            unsigned int *no_addr = (unsigned int *)(addr + offset);
            *no_addr = value;

            char sizeWrite = 12;
            write(respPipe, &sizeWrite, 1);
            write(respPipe, "WRITE_TO_SHM", 12);
            char size = 7;
            write(respPipe, &size, 1);
            write(respPipe, "SUCCESS", 7);
        }
        else
        {
            char sizeWrite = 12;
            write(respPipe, &sizeWrite, 1);
            write(respPipe, "WRITE_TO_SHM", 12);
            char size = 5;
            write(respPipe, &size, 1);
            write(respPipe, "ERROR", 5);
        }
    }
    else
    {
        char sizeWrite = 12;
        write(respPipe, &sizeWrite, 1);
        write(respPipe, "WRITE_TO_SHM", sizeWrite);
        char size = 5;
        write(respPipe, &size, 1);
        write(respPipe, "ERROR", size);
    }

    // unsigned int value = -1;
    // read(reqPipe, &value, 4);

    // unsigned int sum = offset + sizeof(value);

    // if (offset < 0 || offset > 1619935 || 1619935 < sum)
    // {
    //     char sizeWrite = 12;
    //     write(respPipe, &sizeWrite, 1);
    //     write(respPipe, "WRITE_TO_SHM", sizeWrite);
    //     char size = 5;
    //     write(respPipe, &size, 1);
    //     write(respPipe, "ERROR", size);
    // }

    // memcpy(&addr[offset], (void *)&value, sizeof(value));

    // char sizeWrite = 12;
    // write(respPipe, &sizeWrite, 1);
    // write(respPipe, "WRITE_TO_SHM", sizeWrite);
    // char size = 7;
    // write(respPipe, &size, 1);
    // write(respPipe, "SUCCESS", size);
}

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

    // while (1)
    // {
    //     char dim1 = 0;

    //     read(reqPipe, &dim1, 1);
    //     char *contents = (char *)calloc(dim1 + 1, sizeof(char));
    //     read(reqPipe, contents, dim1);
    //     // write(respPipe, &dim1, 1);
    //     // write(respPipe, contents, dim1);
    //     // int dim = dim1 - '0';
    //     // contents[dim] = '\0';

    //     if (strncmp("VARIANT", contents, strlen("VARIANT")) == 0)
    //     {
    //         char variantDim = 7;
    //         write(respPipe, &variantDim, 1);
    //         write(respPipe, "VARIANT", variantDim);
    //         char valueDim = 5;
    //         write(respPipe, &valueDim, 1);
    //         write(respPipe, "VALUE", valueDim);
    //         unsigned int number = 99475;
    //         write(respPipe, &number, 4);
    //     }

    //     if (strncmp("CREATE_SHM", contents, strlen("CREATE_SHM")) == 0)
    //     {
    //         unsigned int number = -1;
    //         read(reqPipe, &number, sizeof(unsigned int));
    //         int shmD = shm_open("/UZ5I4DNT", O_CREAT | O_RDWR, 0644);
    //         if (shmD < 0)
    //         {
    //             char size = 5;
    //             write(respPipe, &size, 1);
    //             write(respPipe, "ERROR", size);
    //         }
    //         if (ftruncate(shmD, number) == -1)
    //         {
    //             char size = 5;
    //             write(respPipe, &size, 1);
    //             write(respPipe, "ERROR", size);
    //         }
    //         char *addr = (char *)mmap(NULL, number, PROT_READ | PROT_WRITE, MAP_SHARED, shmD, 0);
    //         if (addr == MAP_FAILED)
    //         {
    //             char size = 5;
    //             write(respPipe, &size, 1);
    //             write(respPipe, "ERROR", size);
    //         }
    //         char sizeCreate = 10;
    //         write(respPipe, &sizeCreate, 1);
    //         write(respPipe, "CREATE_SHM", sizeCreate);

    //         char size = 7;
    //         write(respPipe, &size, 1);
    //         write(respPipe, "SUCCESS", size);
    //     }
    //     if (strncmp("WRITE_TO_SHM", contents, strlen("WRITE_TO_SHM")) == 0)
    //     {
    //         write_shm(reqPipe, respPipe);
    //     }

    //     if (strncmp("EXIT", contents, dim1) == 0)
    //     {
    //         // char size = 5;
    //         // write(respPipe, &size, 1);
    //         // write(respPipe, "12345", size);
    //         close(reqPipe);
    //         close(respPipe);
    //         unlink("RESP_PIPE_99475");
    //         free(contents);
    //         return 0;
    //     }

    //     else
    //     {
    //         return 0;
    //         free(contents);
    //     }

    //     free(contents);
    // }
    shm_unlink("/UZ5I4DNT");

    while (1)
    {
        char dim1 = 0;
        read(reqPipe, &dim1, 1);
        char *contents = (char *)calloc(dim1 + 1, sizeof(char));
        read(reqPipe, contents, dim1);
        printf("%s\n", contents);

        if (strncmp("VARIANT", contents, strlen("VARIANT")) == 0)
        {
            // Handle VARIANT command
            char variantDim = 7;
            write(respPipe, &variantDim, 1);
            write(respPipe, "VARIANT", variantDim);
            char valueDim = 5;
            write(respPipe, &valueDim, 1);
            write(respPipe, "VALUE", valueDim);
            unsigned int number = 99475;
            write(respPipe, &number, 4);
        }
        else if (strncmp("CREATE_SHM", contents, strlen("CREATE_SHM")) == 0)
        {
            // Handle CREATE_SHM command
            unsigned int number = -1;
            read(reqPipe, &number, sizeof(unsigned int));
            int shmD = shm_open("/UZ5I4DNT", O_CREAT | O_RDWR, 0644);
            if (shmD < 0)
            {
                char size = 5;
                write(respPipe, &size, 1);
                write(respPipe, "ERROR", size);
                shm_unlink("/UZ5I4DNT");
            }
            if (ftruncate(shmD, number) == -1)
            {
                char size = 5;
                write(respPipe, &size, 1);
                write(respPipe, "ERROR", size);
                shm_unlink("/UZ5I4DNT");
            }
            char *addr = (char *)mmap(NULL, number, PROT_READ | PROT_WRITE, MAP_SHARED, shmD, 0);
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
        else if (strcmp(contents, "WRITE_TO_SHM") == 0)
        {

            // Handle WRITE_TO_SHM command
            write_shm(reqPipe, respPipe);
        }
        else if (strncmp("MAP_FILE", contents, strlen("MAP_FILE")) == 0)
        {
            char size = 0;
            read(reqPipe, &size, 1);
            char *file = (char *)calloc(size, sizeof(char));
            int fd = open(file, O_RDONLY);
            int size1 = lseek(fd, 0, SEEK_END);
            mmap(NULL, size1, PROT_READ, MAP_PRIVATE, fd, 0);

            write(respPipe, "MAP_FILE", strlen("MAP_FILE"));
            write(respPipe, "SUCCESS", strlen("SUCCESS"));
        }
        else if (strncmp("EXIT", contents, dim1) == 0)
        {
            // Handle EXIT command
            close(reqPipe);
            close(respPipe);
            unlink("RESP_PIPE_99475");
            free(contents);
            shm_unlink("/UZ5I4DNT");
            return 0;
        }
        else
        {
            // Handle unrecognized command
            free(contents);
            return 0;
        }

        free(contents);
    }

    return 0;
}