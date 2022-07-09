#include <string.h>
#include <stdlib.h>

void string_remove(char * source, char * toRemove) {
    char *p = NULL;
    size_t size = 0; 
    char res[100];
    // printf("Looking for: \"%s\" in \"%s\"\n", toRemove, source);
    p = strstr(source, toRemove);
    while(p != NULL) {
        *p = '\0';
        // printf("source: %s\n", source);
        strcpy(res, source);
        // printf("res: %s\n", res);
        size_t search_size = strlen(toRemove);
        strcat(res, p + search_size);
        // printf("res: %s\n", res);
        strcpy(source, res);
        p = strstr(source, toRemove);
    }
    return;
}