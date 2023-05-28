#include <stdio.h>
#include <string.h>
#include "wrappers.h"
#include "minish.h"

#define MAXLINE 1024
#define MAXWORD 50
#define MAXNUMBERWORDS 10

void malloc_for_list(char **argv) {
    for (int i = 0; i < MAXNUMBERWORDS; i++) {
        argv[i] = (char *)malloc((MAXWORD + 1) * sizeof(char));
    }
}

void free_list(char **argv) {
    for (int i = 0; i < MAXNUMBERWORDS; i++) {
        free(argv[i]);
    }
}

int main(void){
    char **argv = (char **)malloc_or_exit(MAXNUMBERWORDS+1);
    char *buffer = (char *)malloc_or_exit(MAXLINE+1);
    malloc_for_list(argv);
    while(1){
        if(fgets(buffer,MAXLINE,stdin)!=NULL){
            printf("%d",linea2argv(buffer,MAXNUMBERWORDS,argv));
        }
        
    }
    free_list(argv);
    free(argv);
    free(buffer);
    return 0;
}