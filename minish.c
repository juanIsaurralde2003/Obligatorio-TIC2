#include <stdio.h>
#include <string.h>
#include "wrappers.h"
#include "minish.h"

#define MAXLINE 1024
#define MAXWORD 50

int main(void){
    char **argv = (char **)malloc_or_exit(10);
    char *buffer = (char *)malloc_or_exit(MAXLINE+1);
    for(int i=0;i<sizeof(argv);i++){
        argv[i] = (char *)malloc_or_exit(MAXWORD+1);
    }
    while(1){
        int i=0;
        if(fgets(buffer,MAXLINE,stdin)!=NULL){
            linea2argv(buffer,0,argv);
        }
        

    }
        
}