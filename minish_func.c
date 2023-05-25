#include "minish.h"



extern int linea2argv(char *linea, int argc, char **argv){
    int i=0;
    int j=0;
    argc = 0;

    while(linea[i]!='\n'){
        while(linea[i]!=' ' || linea[i]!='\t'){
            argv[argc][j] = linea[i]; 
            j++;
        }
        argc++;
    }
    return argc;
}