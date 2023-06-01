#include "minish.h"
#include "wrappers.h"
#include <sys/types.h>
#include <unistd.h>
#include <pwd.h>
#include <stdlib.h>
#include <stdio.h>
#include <grp.h>
#include <sys/wait.h>




int builtin_exit (int argc, char ** argv)
{
    if (argc>1){
        int status = atoi(argv[1]);
        exit(status);
    }
    else{
        exit(globalstatret);
    }
}

int builtin_help (int argc, char ** argv)
{
    if (argc>1){
        struct builtin_struct *cmd=builtin_lookup(argv[1]);
        if (cmd!=NULL){
            printf("%s\n",cmd->help_txt);
        }
        else{
            fprintf(stderr,"Sorry, command not found. Type 'help' to see this list\n");
        }
    }

    else{
        printf("These shell commands are defined internally. Type 'help' to see this list");
        printf("Type `help name' to find out more about the function `name'");
        printf("Here's the list of commands: help, exit, lookup, history, status, cd, dir, getenv, set env, pid, uid, unsetenv, ejecutar y externo\n");
    }
    return 0;
}
int builtin_pid (int argc,char ** argv){
    if (argv[0] && argc){}

    pid_t pid = getpid();
    if(pid!=0){
        printf("parent process id is %d\n",(unsigned) pid);
        return 0;
    }
    return 1;
}

int builtin_uid (int argc, char ** argv){
    if (argv[0] && argc){}

    uid_t uid = geteuid();
    struct passwd *access = getpwuid(uid);
    if (access)
        {
            printf("nombre: %s\n",access->pw_name);
            printf("id: %d\n",uid);
            return 0;
        }
    else{
            fprintf (stderr,"cannot find username for UID %u\n",(unsigned) uid);
            return 1;
        }
}
int builtin_gid (int argc, char ** argv){
    if (argv[0] && argc){}

    gid_t group_id = getgid();
    printf("Grupo principal: %d\n", group_id);
    
    int num_groups = getgroups(0, NULL);
    gid_t group_list[num_groups];
    getgroups(num_groups, group_list);
    
    printf("Grupos secundarios: ");
    for (int i = 0; i < num_groups; i++) {
        struct group *grp = getgrgid(group_list[i]);
        if (grp != NULL) {
            printf("%s ", grp->gr_name);
        }
    }
    printf("\n");
    
    return 0;
}

//dsadsa
int builtin_getenv (int argc, char ** argv){ //revisar excepciones
    if(argc>1){
        for(int i=1;i<argc;i++){
            char *variable = argv[i];
            char *valor = getenv(variable);
            if(valor != NULL){
                printf("%s = %s\n",variable,valor);
            }
            else{
                fprintf(stderr,"%s %s error: Invalid argument\n",argv[0],argv[1]);
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
        fprintf(stderr,"setenv error: Invalid argument"); //revisar excepciones
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
        if(linea[i]!= '\0' && linea[i]!=' ' && linea[i]!='\t'){
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
    if (is_a_space==0){
        argv[word_count][j] = '\0';
        word_count++;
    }
    argv[word_count] = NULL;
    return word_count;
}

int builtin_status (int argc, char ** argv){
    if (argv[0] && argc){}
    printf("%d\n",globalstatret);
    return 0;
}


int ejecutar (int argc, char ** argv)
{
    struct builtin_struct *cmd=builtin_lookup(argv[0]);
    if (cmd==NULL){
        return externo(argc,argv);
    }
    return cmd->func(argc,argv);
}

int externo (int argc, char ** argv)
{
    pid_t pid = fork();
    int status=1;

   if (pid < 0 || argc==0) { 
   	fprintf(stderr, "Fork Failed");
   	return 1;
   }
    else if (pid == 0) { /* child process */
        status = execvp(argv[0], argv);
        fprintf(stderr,"Exteranl function fail\n");
        exit(1);
    }
    else { /* parent process */
   	/* parent will wait for the child to complete */
   	  wait(NULL);
    }
    return status;

}

int builtin_unsetenv(int argc, char **argv){
    int retorno=0;
    if(argc<2){
        return 1;
    }
    else{
        for(int i=1;i<argc;i++){
            char *variable= argv[i];
            int result= unsetenv(variable); //Llamo a la funcion para eliminar la variable y verifico si ocurrio algun error.
            if(result!=0){
                fprintf(stderr,"Error al eliminar la variable de entorno: %s\n", variable);
                retorno=1;
            }
            else{
                printf("Variable de entorno eliminada: %s\n", variable);
            }
        }
        return retorno;
    }
}
