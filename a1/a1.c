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

    char *output = calloc(strlen(input) - 4, sizeof(char));
    int j = 0;
    for (int i = 5; i < strlen(input); i++)
    {
        output[j] = input[i];
        j++;
    }
    output[j] = '\0';
    return output;
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
    value[j] = '\0';
    return value;
}

void listIter(char *inputPath, int value, int commander, char *permissionPath) // commander chooses whether it searches for files with size < value or not
{
    DIR *dir = NULL;
    struct dirent *entry = NULL;
    char filePath[4096];
    struct stat statbuf;
    static int show = 0;
    dir = opendir(inputPath);
    if (dir == NULL)
    {

        printf("ERROR\ninvalid directory path");
        return;
    }
    if (show == 0)
    {
        printf("SUCCESS\n");
        show++;
    }
    while ((entry = readdir(dir)) != NULL)
    {
        if (strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0)
        {
            snprintf(filePath, 4096, "%s/%s", inputPath, entry->d_name);
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
    static int show = 0;

    dir = opendir(inputPath);
    if (dir == NULL)
    {

        // perror("ERROR\ninvalid directory path");
        printf("ERROR\ninvalid directory path");
        return;
    }
    if (show == 0)
    {
        printf("SUCCESS\n");
        show++;
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
                if (commander == 1 && statbuf.st_size < value && S_ISREG(statbuf.st_mode))
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

    long int returned = 0;
    char *valueAsChar = (char*)calloc(255, sizeof(char));
    int j = 0;
    for (int i = 13; i < strlen(first); i++)
    {
        valueAsChar[j] = first[i];
        j++;
    }
    sscanf(valueAsChar, "%ld",&returned);
    char *inputPath = parseForPath(second); // saves the path that the user haad on inpuT

    listIter(inputPath, returned, 1, NULL);
    free(valueAsChar);
    free(inputPath);
}

void check_size_and_pathREC(char *first, char *second)
{
    long int returned = 0;
    char *valueAsChar = (char*)calloc(255, sizeof(char));
    int j = 0;
    for (int i = 13; i < strlen(first); i++)
    {
        valueAsChar[j] = first[i];
        j++;
    }
    sscanf(valueAsChar, "%ld",&returned);
    char *inputPath = parseForPath(second); // saves the path that the user haad on inpuT

    listRec(inputPath, returned, 1);
    free(valueAsChar);
    free(inputPath);
}

section_header *parseSectionFile(char *path, int *commander)
{
    int fd = open(path, O_RDONLY);
    char magic[2];
    int header_size = 0;
    int filesize = 0;
    int version = 0;
    int nr_of_sect = 0;

    filesize = lseek(fd, 0, SEEK_END);
    lseek(fd, 0, SEEK_SET);

    lseek(fd, -1, SEEK_END);
    read(fd, &magic, 1);
    magic[1] = '\0';
    lseek(fd, -3, SEEK_CUR);
    read(fd, &header_size, 2);
    lseek(fd, filesize - header_size, SEEK_SET);
    read(fd, &version, 1);
    read(fd, &nr_of_sect, 1);

    section_header *headers = (section_header *)calloc(nr_of_sect, sizeof(section_header));
    if (headers == NULL)
    {
        perror("no allocation");
        return NULL;
    }

    int check = 1;
    for (int i = 0; i < nr_of_sect; i++)
    {

        read(fd, &headers[i].name, 12);
        headers[i].name[12] = '\0';
        read(fd, &headers[i].type, 2);
        if (headers[i].type != 34 && headers[i].type != 66 && headers[i].type != 88 && headers[i].type != 13 && headers[i].type != 54)
        {
            printf("ERROR\nwrong sect_types\n");
            check = 0;
        }
        read(fd, &headers[i].offset, 4);
        read(fd, &headers[i].size, 4);
    }

    if (strcmp(magic, "E") != 0)
    {
        printf("ERROR\nwrong magic\n");
        check = 0;
    }
    if (version < 31 || version > 63)
    {
        printf("ERROR\nwrong version\n");
        check = 0;
    }
    if (nr_of_sect < 2 || nr_of_sect > 17)
    {
        printf("ERROR\nwrong sect_nr\n");
        check = 0;
    }

    // print phase
    if (check == 1 && *commander == 1)
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
            printf("%d \n", headers[i].size);
        }
    }

    *commander = nr_of_sect;
    return headers;
    // free(headers);
    close(fd);
}

section_header *parseSectionFileAux(char *path, int *commander)
{
    // printf("%s\n \n ", path);
    int fd = open(path, O_RDONLY);
    char magic[2];
    int header_size = 0;
    int filesize = 0;
    int version = 0;
    int nr_of_sect = 0;

    filesize = lseek(fd, 0, SEEK_END);
    lseek(fd, 0, SEEK_SET);

    lseek(fd, -1, SEEK_END);
    read(fd, &magic, 1);
    magic[1] = '\0';
    lseek(fd, -3, SEEK_CUR);
    read(fd, &header_size, 2);
    lseek(fd, filesize - header_size, SEEK_SET);
    read(fd, &version, 1);
    read(fd, &nr_of_sect, 1);
    int worked = 0;
    section_header *headers;

    if (nr_of_sect != 0)
    {
        headers = (section_header *)calloc(nr_of_sect, sizeof(section_header));
        if (headers == NULL)
        {
            perror("no allocation");
            close(fd);
            return NULL;
        }
        worked = 1;
    }

    if (worked == 1)
    {
        int check = 1;
        for (int i = 0; i < nr_of_sect; i++)
        {

            read(fd, &headers[i].name, 12);
            headers[i].name[12] = '\0';
            read(fd, &headers[i].type, 2);
            if (headers[i].type != 34 && headers[i].type != 66 && headers[i].type != 88 && headers[i].type != 13 && headers[i].type != 54)
            {

                check = 0;
            }
            read(fd, &headers[i].offset, 4);
            read(fd, &headers[i].size, 4);
        }

        // printf("%s\n", magic);

        if (strcmp(magic, "E") != 0)
        {

            check = 0;
        }
        // printf("%d\n", version);
        if (version < 31 || version > 63)
        {

            check = 0;
        }
        // printf("%d\n", nr_of_sect);
        if (nr_of_sect < 2 || nr_of_sect > 17)
        {

            check = 0;
        }

        if (check == 0)
        {
            free(headers);
            return NULL;
        }
        *commander = nr_of_sect;
        close(fd);
        return headers;
    }
    // free(headers);
    close(fd);
    return NULL;
}

void findAll(char *inputPath)
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

                if (S_ISDIR(statbuf.st_mode))
                {
                    findAll(filePath);
                }
                if (S_ISREG(statbuf.st_mode))
                {
                    int no_of_sect = 0;
                    section_header *headers = parseSectionFileAux(filePath, &no_of_sect);
                    for (int i = 0; i < no_of_sect; i++)
                    {
                        if (headers[i].type == 66)
                        {
                            printf("%s\n", filePath);
                            break;
                        }
                    }
                    free(headers);
                }
            }
        }
    }
    closedir(dir);
}

int main(int argc, char **argv)
{
    int variant = -1, path = -1, recursive = -1, size_smaller = -1, permissions = -1, section = -1, line = -1;
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
        if (strncmp(argv[i], "section=", 8) == 0)
        {
            section = i;
        }
        if (strncmp(argv[i], "line=", 5) == 0)
        {
            line = i;
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
                    listIter(inputPath, 0, 0, NULL);
                    free(inputPath);
                }
                else if (size_smaller != -1)
                {
                    check_size_and_path(argv[size_smaller], argv[path]);
                }

                if (permissions != -1 && size_smaller == -1)
                {
                    char *permissionsString = parseForPerm(argv[permissions]);
                    char *inputPath = parseForPath(argv[path]);
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
                    // printf("SUCCESS\n"); ////HARDCODED
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
            char *filePath = parseForPath(argv[path]);
            int one = 1;
            free(parseSectionFile(filePath, &one));
            free(filePath);
        }

        if (strcmp(argv[1], "extract") == 0)
        {
            int zero = 0;
            char *filePath = parseForPath(argv[path]);
            int fd = open(filePath, O_RDONLY);
            section_header *headers = parseSectionFile(filePath, &zero);
            free(filePath);
            int sectionNr = 0, lineNr = 0;

            char *output = calloc(strlen(argv[section]) - 8, sizeof(char));
            int j = 0;
            for (int i = 8; i < strlen(argv[section]); i++)
            {
                output[j] = argv[section][i];
                j++;
            }
            sscanf(output, "%d", &sectionNr);
            free(output);

            char *out = calloc(strlen(argv[line]) - 5, sizeof(char));
            j = 0;
            for (int i = 5; i < strlen(argv[line]); i++)
            {
                out[j] = argv[line][i];
                j++;
            }
            sscanf(out, "%d", &lineNr);
            free(out);
            lseek(fd, headers[sectionNr - 1].offset + headers[sectionNr - 1].size, SEEK_SET);

            // we are at the end of the section right now
            int counter = 0;
            int seek = -1;
            printf("SUCCESS\n");
            while (counter != lineNr - 1)
            {
                char buff;
                lseek(fd, seek, SEEK_CUR);
                read(fd, &buff, 1);
                lseek(fd, -seek - 1, SEEK_CUR);
                if (buff == 0x0A)
                {
                    counter++;
                }
                seek--;
            }
            while (1)
            {
                char buff;
                lseek(fd, seek, SEEK_CUR);
                read(fd, &buff, 1);
                printf("%c", buff);
                lseek(fd, -seek - 1, SEEK_CUR);
                if (buff == 0x0A)
                {
                    break;
                }
                seek--;
            }

            close(fd);
            free(headers);
        }
        if (strcmp(argv[1], "findall") == 0)
        {
            char *filePath = parseForPath(argv[path]);
            printf("SUCCESS\n");
            findAll(filePath);
            free(filePath);
        }
    }

    return 0;
}