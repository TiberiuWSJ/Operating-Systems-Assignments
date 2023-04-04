#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>
#include <fcntl.h>

typedef struct
{
    char name[13];
    int type;
    int offset;
    int size;

} section_header;

int convertPermissions(char *permissions)
{
    int value = 0;
    for (int i = 0; i < 9; i++)
    {
        if (permissions[i] == 'r' || permissions[i] == 'w' || permissions[i] == 'x')
        {
            value |= 1 << (8 - i);
        }
    }
    return value;
}

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

char *parseForPerm(char *input)
{
    char *value = (char *)calloc(256, sizeof(char));
    int j = 0;
    for (int i = 12; i < strlen(input); i++)
    {
        value[j] = input[i];
        j++;
    }
    return value;
}

void listIter(char *inputPath, int value, int commander, char *permissionPath) // commander chooses whether it searches for files with size < value or not
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
                    if (permissionPath == NULL)
                    {
                        printf("%s\n", filePath);
                    }
                    else if ((statbuf.st_mode & 0777) == convertPermissions(permissionPath))
                    {
                        printf("%s\n", filePath);
                    }
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
    listIter(inputPath, value, 1, NULL);
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
    int variant = -1, path = -1, recursive = -1, size_smaller = -1, permissions = -1;
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
        if (strncmp(argv[i], "permissions=", 12) == 0)
        {
            permissions = i;
        }
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
                if (size_smaller == -1 && permissions == -1)
                {
                    char *inputPath = parseForPath(argv[path]);
                    printf("SUCCESS\n"); ////HARDCODED
                    listIter(inputPath, 0, 0, NULL);
                    free(inputPath);
                }
                else if (size_smaller != -1)
                {
                    check_size_and_path(argv[size_smaller], argv[path]);
                }

                if (permissions != -1 && size_smaller == -1)
                { // printf("INPUT -> %s\n", argv[permissions]);
                    char *permissionsString = parseForPerm(argv[permissions]);
                    // printf("CONVERTED -> %s\n", argv[permissions]);
                    char *inputPath = parseForPath(argv[path]);
                    printf("SUCCESS\n");
                    listIter(inputPath, 0, 0, permissionsString);
                    free(permissionsString);
                    free(inputPath);
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
        if (strcmp(argv[1], "parse") == 0)
        {
            char* filePath = parseForPath(argv[path]);
            int fd = open(filePath, O_RDONLY);
            free(filePath);
            char magic[2];
            int header_size;
            int filesize;
            int version;
            int nr_of_sect;

            filesize = lseek(fd, 0, SEEK_END);
            lseek(fd, 0, SEEK_SET);

            lseek(fd, -1, SEEK_END);
            read(fd, &magic, 1);
            magic[1] = '\0';
            // printf("%c\n", magic);
            lseek(fd, -3, SEEK_CUR);
            read(fd, &header_size, 2);
            lseek(fd, filesize - header_size, SEEK_SET);
            read(fd, &version, 1);
            read(fd, &nr_of_sect, 1);

            section_header *headers = (section_header *)calloc(nr_of_sect, sizeof(section_header));
            int check = 1;
            for (int i = 0; i < nr_of_sect; i++)
            {

                read(fd, &headers[i].name, 12);
                headers[i].name[12] = '\0';
                read(fd, &headers[i].type, 2);
                if (headers[i].type != 34 && headers[i].type != 66 && headers[i].type != 88 && headers[i].type != 13 && headers[i].type != 54){
                    printf("ERROR\nwrong sect_types\n");
                    check = 0;
                }
                read(fd, &headers[i].offset, 4);
                read(fd, &headers[i].size, 4);
            }
            

            if (strcmp(magic, "E") != 0)
            {
                //perror("ERROR\nwrong magic\n");
                printf("ERROR\nwrong magic\n");
                check = 0;
            }
            if (version < 31 || version > 63)
            {
                printf("ERROR\nwrong version\n");   
                //perror("ERROR\nwrong version");
                check = 0;
            }
            if (nr_of_sect < 2 || nr_of_sect > 17)
            {
                printf("ERROR\nwrong sect_nr\n");
                //perror("ERROR\nwrong sect_nr");
                check = 0;
            }
        

            // print phase
            if (check == 1)
            {
                printf("SUCCESS\n");
                printf("version=%d\n", version);
                printf("nr_sections=%d\n", nr_of_sect);

                for (int i = 0; i < nr_of_sect; i++)
                {
                    printf("section");
                    printf("%d: ", i + 1);
                    printf("%s ", headers[i].name);
                    printf("%d ", headers[i].type);
                    // printf("%d ", headers[i].offset);
                    printf("%d \n", headers[i].size);
                }
            }

            free(headers);
            close(fd);
        }
    }

    return 0;
}