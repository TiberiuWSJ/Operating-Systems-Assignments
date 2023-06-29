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

void pipeVariant(int respPipe)
{
    // printf("AAAAAAAAasdasdasdasdsadsadas");

    char dimVar = 7;
    write(respPipe, &dimVar, 1);
    write(respPipe, "VARIANT", 7);

    char dimValue = 5;
    write(respPipe, &dimValue, 1);
    write(respPipe, "VALUE", 5);
    unsigned int variant = 99475;
    write(respPipe, &variant, sizeof(unsigned int));
}

void pipeShm(int reqPipe, int respPipe)
{
    unsigned int number = 0;
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
    addr = (char *)mmap(NULL, number, PROT_READ | PROT_WRITE, MAP_SHARED, shmD, 0);
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

void pipeWriteShm(int reqPipe, int respPipe)
{
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
}

void pipeMapFile(int reqPipe, int respPipe)
{
    char size = 0;
    read(reqPipe, &size, 1);
    char *file = (char *)calloc(size + 1, sizeof(char));
    for (int i = 0; i < size; i++)
    {
        read(reqPipe, &file[i], 1);
    }

    int fd = open(file, O_RDONLY);

    if (fd < 0)
    {
        char dimFile = 8;
        write(respPipe, &dimFile, 1);
        write(respPipe, "MAP_FILE", strlen("MAP_FILE"));

        char dimError = 7;
        write(respPipe, &dimError, 1);
        write(respPipe, "ERROR", strlen("ERROR"));
    }

    int size1 = lseek(fd, 0, SEEK_END);
    lseek(fd, 0, SEEK_SET);
    mmap(NULL, size1, PROT_READ, MAP_PRIVATE, fd, 0);

    char *data = (char *)mmap(NULL, size, PROT_READ, MAP_PRIVATE, fd, 0);
    if (data == (void *)-1)
    {
        char dimFile = 8;
        write(respPipe, &dimFile, 1);
        write(respPipe, "MAP_FILE", strlen("MAP_FILE"));

        char dimError = 5;
        write(respPipe, &dimError, 1);
        write(respPipe, "ERROR", strlen("ERROR"));
        close(fd);
        return;
    }
    char dimFile = 8;
    write(respPipe, &dimFile, 1);
    write(respPipe, "MAP_FILE", strlen("MAP_FILE"));

    char dimSuccess = 7;
    write(respPipe, &dimSuccess, 1);
    write(respPipe, "SUCCESS", strlen("SUCCESS"));
}

void pipeExit(int reqPipe, int respPipe)
{
    close(reqPipe);
    close(respPipe);
}

int main(int argc, char **argv)
{

    unlink("RESP_PIPE_99475");

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
        char dimReq = -1;
        read(reqPipe, &dimReq, 1);
        char *compar = (char *)calloc(250, sizeof(char));

        for (int i = 0; i < dimReq; i++)
        {
            read(reqPipe, &compar[i], 1);
        }

        printf("%s", compar);

        if (strcmp(compar, "VARIANT") == 0)
        {

            pipeVariant(respPipe);
        }
        if (strcmp(compar, "CREATE_SHM") == 0)
        {
            pipeShm(reqPipe, respPipe);
        }

        if (strcmp(compar, "WRITE_TO_SHM") == 0)
        {
            pipeWriteShm(reqPipe, respPipe);
        }

        if (strcmp(compar, "MAP_FILE") == 0)
        {
            pipeMapFile(reqPipe, respPipe);
        }

        if (strcmp(compar, "EXIT") == 0)
        {
            pipeExit(reqPipe, respPipe);
            break;
        }
    }

    return 0;
}