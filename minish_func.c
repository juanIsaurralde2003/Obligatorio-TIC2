#include "minish.h"
#include "wrappers.h"


extern int linea2argv(char *linea, int argc, char **argv){
    int i=0;
    int j=0;
    int word_count=0;
    int is_a_space=0;
    while(linea[i]!='\n' && word_count<argc){
        if(linea[i]!=' ' && linea[i]!='\t'){
            argv[word_count][j] = linea[i]; 
            j++;
            is_a_space=0;
        }
        else if(is_a_space==0)
        {
            argv[word_count][j] = '\0';
            word_count++;
            is_a_space=1;
        }
        i++;
    }
    argv[word_count] = '\0';
    if (word_count>0) word_count++;
    return word_count;
}