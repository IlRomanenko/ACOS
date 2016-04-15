#include <dirent.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

char* concat(const char* str1, const char* str2)
{
    char* res = (char*)malloc(strlen(str1) + strlen(str2) + 1);
    strcpy(res, str1);
    strcat(res, str2);
    return res;
}

void ls(const char* directory, int tabs)
{
    DIR *dir = opendir(directory);
    if (dir == NULL)
        return;
    struct dirent *ent;
    while ((ent = readdir(dir)) != NULL)
    {
        char *s, *ts;
        if (ent->d_name[0] == '.')
            continue;
        int i;
        for (i = 0; i < tabs; i++)
            printf("\t");
        printf("%s\n", ent->d_name);
        ts = concat(directory, "/");
        s = concat(ts, ent->d_name);
        ls(s, tabs + 1);
        free(ts);
        free(s);
    }
    closedir(dir);
    return;
}


int main(int argc, char** argv)
{
    if (argc != 1)
        ls(argv[1], 0);
    else
        printf("Error");
    return 0;
}
