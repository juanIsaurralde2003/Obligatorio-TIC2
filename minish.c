#include <stdio.h>
#include <string.h>
#include "wrappers.h"
#include "minish.h"

#define MAXLINE 1024
#define MAXWORD 50
#define MAXNUMBERWORDS 10

void malloc_for_list(char **argv) {
    for (int i = 0; i < MAXNUMBERWORDS; i++) {
        argv[i] = (char *)malloc_or_exit((MAXWORD+1) * sizeof(char));
    }
}

void free_list(char **argv,int argc){
    for (int i = 0; i < argc; i++) {
        free(argv[i]);
    }
}

int last_command_status; 

int main(void){
    char **argv = (char **)malloc_or_exit((MAXNUMBERWORDS+1)*sizeof (char *));
    char *buffer = (char *)malloc_or_exit((MAXLINE+1) * sizeof (char));
    last_command_status = 0;

    while (1) {
        printf("Shell > ");
        fgets(buffer, MAXLINE, stdin);
        malloc_for_list(argv);
        int argc = linea2argv(buffer, MAXNUMBERWORDS, argv);

   
        if (argc > 0) {
            printf("%s\n",argv[0]);
            char *command = argv[0];
         
            if (strcmp(command, "exit") == 0) {
                builtin_exit(argc,argv);
                free_list(argv,argc);
                break;
            } else if (strcmp(command, "help") == 0) {
                last_command_status = builtin_help(argc,argv);
            } else if (strcmp(command, "pid") == 0) {
                last_command_status = builtin_pid();
            } else if (strcmp(command, "uid") == 0){
                last_command_status = builtin_uid();
            } else if (strcmp(command, "getenv") == 0){
                
            } else if (strcmp(command, "setenv") == 0){
                
            } 
            else {
                printf("Comando no reconocido\n");
            }
        }    
        free_list(argv,argc);
    }
  
    free(argv);
    free(buffer);
    return 0;
}