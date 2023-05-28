#include "minish.h"
#include "wrappers.h"
#include <sys/types.h>
#include <unistd.h>
#include <pwd.h>

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
int builtin_getenv (int argc, char ** argv);
int builtin_setenv (int argc, char ** argv);

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
            word_count++;
            is_a_space=1;
        }
        i++;
    }
    if (j>0 && is_a_space!=1) word_count++;
    argv[word_count] = NULL;

    return word_count;
}