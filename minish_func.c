#include "minish.h"
#include "wrappers.h"
#include <sys/types.h>
#include <string.h>
#include <unistd.h>
#include <pwd.h>
#include <stdlib.h>
#include <stdio.h>
#include <grp.h>
#include <errno.h>       
#include <error.h>
#include <sys/wait.h>
#include <dirent.h>
#define HISTORY_FILE "$HOME/.minish_history"
#define MAX_COMMAND_LENGTH 100


int loaded_history;

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
            error(1,errno,"Lo siento, el comando no existe. Escriba 'help' para ver la lista de comandos");
        }
    }

    else{
        printf("Estos comandos de shell fueron definidos internamente. Escriba 'help' para ver la lista de comandos\n");
        printf("Escriba `help name' para ver mas información acerca de la funcion 'name'");
        printf("Esta es la lista de comandos: help, exit, lookup, history, status, cd, dir, getenv, set env, pid, uid, unsetenv, ejecutar y externo\n");
    }
    return 0;
}
int builtin_pid (int argc,char ** argv){
    if (argv[0] && argc){}

    pid_t pid = getpid();
    if(pid!=0){
        printf("Id del proceso padre es: %d\n",(unsigned) pid);
        return 0;
    }
    error(1,errno,"No se pudo encontrar pid");
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
    error(1,errno,"No se pudo encontrar nombre de usuario para la UID %u ",(unsigned) uid);
    return 1;
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
                error(1,errno,"%s %s error: Argumentos inválidos ",argv[0],argv[1]);
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
        error(1,errno,"setenv error: Argumentos invalidos"); //revisar excepciones
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
   	error(1,errno, "Fork Failed ");
   	return 1;
   }
    else if (pid == 0) { /* child process */
        status = execvp(argv[0], argv);
        error(1,errno,"External function fail ");
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
                error(1,errno,"Error al eliminar la variable de entorno: %s ", variable);
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
            error(1,errno, "cd to %s failed ", new_path);
        }
    } else {
        error(1,errno, "Invalid argument ");
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
        error(1,errno,"Muchos argumentos ");
        return 1;
    }
    return 0;
}



void load_history(){
    FILE* file = fopen(HISTORY_FILE, "r");
    if (file != NULL) {
        char command[MAX_COMMAND_LENGTH];

        while (fgets(command, MAX_COMMAND_LENGTH, file) != NULL) {
            // Eliminar el carácter de nueva línea del final del comando
            command[strcspn(command, "\n")] = '\0';
            deq_append(history,command);
            

        }

        fclose(file);
    }
    loaded_history=history->count;
}

void free_history() { //liberar la memoria utilizada por history
    struct deq_elem* current = history->leftmost;
    while (current != NULL) {
        struct deq_elem* temp = current;
        current = current->next;
        free(temp);
    }
}


void save_history(){ //para guardar en el archivo los ultimos comandos
    FILE *file = fopen(HISTORY_FILE, "a");
    if (file != NULL) {
        struct deq_elem *current = history->leftmost;
        int i=0;
        while (current != NULL) {
            if (i>=loaded_history) fprintf(file, "%s\n", current->str);
            current = current->next;
        }
        fclose(file);
    }
    free_history();
}

 
void show_history(int count) {
    struct deq_elem *current = history->rightmost;
    int commandCount=0;
    while (current != NULL && commandCount < count) {
        printf("%s\n", current->str);
        current = current->prev;
        commandCount++;
    }
}



int builtin_history(int argc,char **argv){
    int default_lines = 10;
    if (argc==1){
        show_history(default_lines);
    }else{
        show_history(argv[2]);
    }    
    return 1;
}

extern struct deq *deq_create1(void){
    return (struct deq*) malloc_or_exit(sizeof(struct deq));
}
void delete_elem(struct deq_elem *elemento){
    free(elemento->str);
    free(elemento);
}

extern struct deq_elem *elem_create(void){
    struct deq_elem *temp= (struct deq_elem*) malloc_or_exit(sizeof(struct deq_elem));
    return temp;
}


struct deq *deq_create(void){
    struct deq *temp= deq_create1();
    temp->count=0;
    temp->leftmost=temp->rightmost= NULL;
    return temp;
}
// create new, empty deq

extern struct deq_elem *deq_append(struct deq *deque, char *s){
    struct deq_elem *elemento=elem_create();
    elemento->str= strdup_or_exit(s);
    if(deque->leftmost==NULL && deque->rightmost==NULL){
        deque->leftmost= elemento;
        deque->rightmost=elemento;
        elemento->prev= NULL;
        deque->count++;
    }
    else{
        struct deq_elem *ultimo= deque->rightmost;
        ultimo->next= elemento;
        elemento->prev= ultimo;
        elemento->next=NULL;
        deque->rightmost= elemento;
        deque->count++;
    }
    return elemento;
}
// append element on the right end, return new elem










