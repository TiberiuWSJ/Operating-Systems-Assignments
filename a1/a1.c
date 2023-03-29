#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>

void listIter(char *inputPath, int value, int commander) //commander chooses whether it searches for files with size < value or not
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
                if (statbuf.st_size < value && S_ISREG(statbuf.st_mode) && commander == 1)
                {
                    printf("%s\n", filePath);
                }
                if(S_ISREG(statbuf.st_mode) && commander == 0){
                    printf("%s\n",filePath);
                }
            }
        }
    }
    closedir(dir);
}

int main(int argc, char **argv)
{
    if (argc >= 2)
    {
        if (strcmp(argv[1], "variant") == 0)
        {
            printf("99475\n");
        }
        if (strcmp(argv[1], "list") == 0)
        {
            if (strncmp(argv[2], "path=", 5) == 0)
            {
                char *inputPath = calloc(strlen(argv[2]) - 5, sizeof(char));
                int j = 0;
                for (int i = 5; i < strlen(argv[2]); i++)
                {
                    inputPath[j] = argv[2][i];
                    j++;
                }
                printf("SUCCESS\n");
                listIter(inputPath,0,0);
            }

            if (strncmp(argv[2], "size_smaller=", 13) == 0 && strncmp(argv[3], "path=", 5) == 0)
            {
                long int value;                                              // contains the value after size_smaller
                char *inputPath = calloc(strlen(argv[3]) - 5, sizeof(char)); // saves the path that the user haad on input
                int j = 0;

                for (int i = 5; i < strlen(argv[3]); i++)
                {
                    inputPath[j] = argv[3][i];
                    j++;
                }
                char valueAsChar[255];
                j = 0;
                for (int i = 13; i < strlen(argv[2]); i++)
                {
                    valueAsChar[j] = argv[2][i];
                    j++;
                }
                sscanf(valueAsChar, "%ld", &value);
                // tested corectly until here;
                printf("SUCCESS\n"); // how can i differentiate whether an error appeared?
                listIter(inputPath, value,1);
                free(inputPath);
            }
        }
    }

    return 0;
}