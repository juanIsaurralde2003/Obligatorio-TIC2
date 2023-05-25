#include "minish.h"
#include "wrappers.h"


extern int linea2argv(char *linea, int argc, char **argv){
    int i=0;
    int j=0;
    argc= 1;
    int is_a_space=0;
    while(linea[i]!='\n'){
        if(linea[i]!=' ' && linea[i]!='\t'){
            argv[argc][j] = linea[i]; 
            j++;
            is_a_space=0;
        }
        else if(is_a_space==0)
        {
            argv[argc][j] = '\0';
            argc++;
            is_a_space=1;
        }
        i++;
    }
    argv[argc] = '\0';
    return argc;
}