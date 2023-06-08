#include "minish.h"
#include "wrappers.h"
#include <sys/types.h>
#include <string.h>
#include <unistd.h>
#include <pwd.h>
#include <stdlib.h>
#include <stdio.h>
#include <grp.h>
#include <sys/wait.h>
#include <dirent.h>
#define HISTORY_FILE "$HOME/.minish_history"
#define MAX_COMMAND_LENGTH 100




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
        fprintf(stderr,"External function fail\n");
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

int builtin_cd (int argc, char ** argv)
{
    char *new_path = NULL;

    if (argc == 1) {
        new_path = getenv("HOME");
    } else if (strcmp(argv[1], "-") == 0) {
        new_path = getenv("OLDPWD");
    } else {
        char *current_path = getenv("PWD");
        size_t current_path_len = strlen(current_path);
        size_t arg_len = strlen(argv[1]);

        if (arg_len > 0 && argv[1][0] == '/') {
            // Absolute path
            new_path = argv[1];
        } else {
            // Relative path
            size_t new_path_len = current_path_len + arg_len + 2; 
            new_path = malloc(new_path_len);
            snprintf(new_path, new_path_len, "%s/%s", current_path, argv[1]);
        }
    }

    if (new_path != NULL) {
        char *old_path = getenv("PWD");

        if (chdir(new_path) == 0) {
            setenv("OLDPWD", old_path, 1);
            setenv("PWD", new_path, 1);
        } else {
            fprintf(stderr, "cd to %s failed\n", new_path);
            return 1;
        }
    } else {
        fprintf(stderr, "Invalid argument\n");
        return 1;
    }

    return 0;
}

void print_files(DIR *dir){
    struct dirent *entry;
     while ((entry = readdir(dir)) != NULL) {
        printf("%s\n", entry->d_name);
    }
}
void print_files_with_name(DIR *dir, char *name){
    struct dirent *entry;
     while ((entry = readdir(dir)) != NULL) {
        char *nombre_archivo = entry->d_name;
        if(strstr(nombre_archivo, name) != NULL){
            printf("%s\n", nombre_archivo);
        }
    }
}

int builtin_dir (int argc, char ** argv){

    if(argc==1){
        char *path = getenv("PWD");
        DIR *home_dir = opendir(path);
        print_files(home_dir);
        closedir(home_dir);
    }

    else if(argc==2){
        DIR *dir = opendir(argv[1]);
        if(dir != NULL){
            print_files(dir);
            closedir(dir);
        }
        else{
            char *path_dir_actual = getenv("PWD");
            DIR *dir_actual = opendir(path_dir_actual);
            print_files_with_name(dir_actual,argv[1]);
            closedir(dir_actual);

        }
    }

    else{
        fprintf(stderr,"Muchos argumentos");
        return 1;
    }
    return 0;
}

typedef struct Node {
    char command[MAX_COMMAND_LENGTH];
    struct Node* next;
} Node;

Node *history=NULL;

void freeHistory() { //para liberar la lista creada para history
    Node* current = history;
    while (current != NULL) {
        Node* temp = current;
        current = current->next;
        free(temp);
    }
}

void chargeHistory(){
    FILE* file = fopen(HISTORY_FILE, "r");
    if (file != NULL) {
        char command[MAX_COMMAND_LENGTH];

        while (fgets(command, MAX_COMMAND_LENGTH, file) != NULL) {
            // Eliminar el carácter de nueva línea del final del comando
            command[strcspn(command, "\n")] = '\0';

            addCommandToHistory(command);
        }

        fclose(file);
}
}




int builtin_history(int argc,char **argv){

}





