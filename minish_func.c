#include "minish.h"
#include "wrappers.h"
#include <sys/types.h>
#include <unistd.h>
#include <pwd.h>
#include <stdlib.h>
#include <stdio.h>

extern int last_command_status; 

int builtin_exit (int argc, char ** argv)
{
    if (argc>1){
        int status = atoi(argv[1]);
        exit(status);
    }
    else{
        exit(last_command_status);
    }
}

int builtin_help (int argc, char ** argv)
{
    if (argc>1){
        //ni idea
        printf("%s",argv[1]);
    }
    else{
        printf("These shell commands are defined internally. Type `help' to see this list");
        printf("Type `help name' to find out more about the function `name'");
        printf("Here's the list of commands: help, exit, lookup, history, status, cd, dir, getenv, set env, pid, uid, unsetenv, ejecutar y externo\n");
    }
    return 0;
}
int builtin_pid (){
    pid_t pid = getpid();
    printf("parent process id is %d\n",(unsigned) pid);
    return 0;
}

int builtin_uid (){
    uid_t uid = geteuid();
    struct passwd *access = getpwuid(uid);
    if (access)
        {
            printf("nombre: %s\n",access->pw_name);
            printf("id: %d",uid);
            return 0;
        }
    else{
            fprintf (stderr,"cannot find username for UID %u\n",(unsigned) uid);
            return 1;
        }
}
int builtin_getenv (int argc, char ** argv){ //revisar excepciones
    if(argc>1){
        for(int i=1;i<argc;i++){
            char *variable = argv[i];
            char *valor = getenv(variable);
            if(valor != NULL){
                printf("%s = %s\n",variable,valor);
            }
            else{
                printf("%s %s error: Invalid argument\n",argv[0],argv[1]);
                return 1;
            }
        }
    }
    else{
        //imprimir el valor de todas las variables de entorno

    }
    return 0;
}
int builtin_setenv (int argc, char ** argv){
    if(argc==3){
        char *variable = argv[1];
        char *valor = argv[2];
        setenv(variable,valor,1);
    }
    else{
        printf("setenv error: Invalid argument"); //revisar excepciones
        return 1;
    }
    return 0;
}

int linea2argv(char *linea, int argc, char **argv){
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
            j=0;
            word_count++;
            is_a_space=1;
        }
        i++;
    }
    if (j>0 && is_a_space!=1) word_count++;
    argv[word_count] = NULL;

    return word_count;
}