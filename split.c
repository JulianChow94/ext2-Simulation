#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char **split(char *path, int *size){
    char *tmp;
    char **splitted = NULL;
    int i, length;

    if (!path){
        goto Exit;
    }

    tmp = strdup(path);
    length = strlen(tmp);

    *size = 1;
    for (i = 0; i < length; i++) {
        if (tmp[i] == '/') {
            tmp[i] = '\0';
            (*size)++;
        }
    }

    splitted = (char **)malloc(*size * sizeof(*splitted));
    if (!splitted) {
        free(tmp);
        goto Exit;
    }

    for (i = 0; i < *size; i++) {
        splitted[i] = strdup(tmp);
        tmp += strlen(splitted[i]) + 1;
    }
    return splitted;

Exit:
    *size = 0;
    return NULL;
}

int main(){
    int size, i;
    char **splitted;

    splitted = split("/foo/foobar/file.txt", &size);

    //for (i = 0; i < size; i++) {
    //    printf("%d: %s\n", i + 1, splitted[i]);
    //}

    printf("%s\n", splitted[1]);
    char test[4] = "foo";
    int ret = strcmp(splitted[1], test);
    printf("%d\n", ret);
    // TODO: free splitted
    return 0;
}