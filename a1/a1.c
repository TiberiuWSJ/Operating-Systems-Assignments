#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>

char *parseForPath(char *input) // we assume that input is looking like = "path=........";
{
    // printf("%s", input);
    char *output = calloc(strlen(input) - 5, sizeof(char));
    int j = 0;
    for (int i = 5; i < strlen(input); i++)
    {
        output[j] = input[i];
        j++;
    }
    return output;
}

long int parseForSize(char *input)
{ // we assume that input looks like = "size_smaller=......"
    // printf("%s", input);

    long int returned;
    char valueAsChar[255];
    int j = 0;
    for (int i = 13; i < strlen(input); i++)
    {
        valueAsChar[j] = input[i];
        j++;
    }
    sscanf(valueAsChar, "%ld", &returned);
    return returned;
}

void listIter(char *inputPath, int value, int commander) // commander chooses whether it searches for files with size < value or not
{
    DIR *dir = NULL;
    struct dirent *entry = NULL;
    char filePath[512];
    struct stat statbuf;

    dir = opendir(inputPath);
    if (dir == NULL)
    {
        perror("Could not open directory");
        return;
    }
    while ((entry = readdir(dir)) != NULL)
    {
        if (strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0)
        {
            snprintf(filePath, 512, "%s/%s", inputPath, entry->d_name);
            if (lstat(filePath, &statbuf) == 0)
            {
                if (statbuf.st_size < value && !S_ISDIR(statbuf.st_mode) && commander == 1)
                {
                    printf("%s\n", filePath);
                }
                if ((S_ISREG(statbuf.st_mode) || S_ISDIR(statbuf.st_mode)) && commander == 0)
                {
                    printf("%s\n", filePath);
                }
            }
        }
    }
    closedir(dir);
}

void listRec(char *inputPath, int value, int commander) // commander chooses whether it searches for files with size < value or not
{
    DIR *dir = NULL;
    struct dirent *entry = NULL;
    char filePath[512];
    struct stat statbuf;
    // printf("%s\n", inputPath);
    dir = opendir(inputPath);
    if (dir == NULL)
    {
        perror("Could not open directory");
        return;
    }
    while ((entry = readdir(dir)) != NULL)
    {
        if (strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0)
        {
            snprintf(filePath, 512, "%s/%s", inputPath, entry->d_name);
            if (lstat(filePath, &statbuf) == 0)
            {

                if (S_ISDIR(statbuf.st_mode) && commander == 1)
                {
                    // printf("%s\n", filePath);
                    listRec(filePath, value, commander);
                }
                if (S_ISDIR(statbuf.st_mode) && commander == 0)
                {
                    printf("%s\n", filePath);
                    listRec(filePath, value, commander);
                }
                if (statbuf.st_size < value && S_ISREG(statbuf.st_mode) && commander == 1)
                {
                    printf("%s\n", filePath);
                }
                if (S_ISREG(statbuf.st_mode) && commander == 0)
                {
                    printf("%s\n", filePath);
                }
            }
        }
    }
    closedir(dir);
}

void check_size_and_path(char *first, char *second)
{

    long int value = parseForSize(first);   // contains the value after size_smaller
    char *inputPath = parseForPath(second); // saves the path that the user haad on inpuT
    printf("SUCCESS\n");                    // how can i differentiate whether an error appeared?
    listIter(inputPath, value, 1);
    free(inputPath);
}

void check_size_and_pathREC(char *first, char *second)
{
    long int value = parseForSize(first);   // contains the value after size_smaller
    char *inputPath = parseForPath(second); // saves the path that the user haad on inpuT
    printf("SUCCESS\n");                    // how can i differentiate whether an error appeared?
    listRec(inputPath, value, 1);
    free(inputPath);
}

int main(int argc, char **argv)
{
    int variant = -1, path = -1, recursive = -1, size_smaller = -1;
    for (int i = 1; i < argc; i++)
    {
        if (argv[i] != NULL && strcmp(argv[i], "variant") == 0)
        {
            variant = i;
            continue;
        }
        if (strncmp(argv[i], "path=", 5) == 0)
        {
            path = i;
        }
        if (strncmp(argv[i], "size_smaller=", 13) == 0)
        {
            size_smaller = i;
        }
        if (strncmp(argv[i], "recursive", 9) == 0)
        {
            recursive = i;
        }
        // if (strncmp(argv[i], "permissions=", 12) == 0)
        // {
        //     permissions = i;
        // }
    }
    if (argc >= 2)
    {
        if (variant != -1)
        {
            printf("99475\n");
        }
        if (strcmp(argv[1], "list") == 0)
        {
            if (recursive == -1)
            {
                if (size_smaller == -1)
                {
                    char *inputPath = parseForPath(argv[path]);
                    printf("SUCCESS\n"); ////HARDCODED
                    listIter(inputPath, 0, 0);
                    free(inputPath);
                }
                else
                {
                    check_size_and_path(argv[size_smaller], argv[path]);
                }
            }
            else
            {
                if (size_smaller == -1)
                {
                    char *inputPath = parseForPath(argv[path]);
                    // printf("CALLER: %s\n", inputPath);
                    printf("SUCCESS\n"); ////HARDCODED
                    listRec(inputPath, 0, 0);
                    free(inputPath);
                }
                else
                {
                    check_size_and_pathREC(argv[size_smaller], argv[path]);
                }
            }
        }
    }

    return 0;
}