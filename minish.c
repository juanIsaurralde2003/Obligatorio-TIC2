#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <pwd.h>
#include "wrappers.h"
#include "minish.h"

#define MAXNUMBERWORDS 10
#define HELP_CD      "cd [..|dir] - cambia de directorio corriente"
#define HELP_DIR     "dir [str]- muestra archivos en directorio corriente, que tengan 'str'"
#define HELP_EXIT    "exit [status] - finaliza el minish con un status de retorno (por defecto 0)"
#define HELP_HELP    "help [cd|dir|exit|help|history|getenv|pid|setenv|status|uid]"
#define HELP_HISTORY "history [N] - muestra los últimos N (10) comandos escritos"
#define HELP_GETENV  "getenv var [var] - muestra valor de variable(s) de ambiente"
#define HELP_PID     "pid - muestra Process Id del minish"
#define HELP_SETENV  "setenv var valor - agrega o cambia valor de variable de ambiente"
#define HELP_STATUS  "status - muestra status de retorno de ultimo comando ejecutado"
#define HELP_UID     "uid - muestra nombre y número de usuario dueño del minish"
#define HELP_GID     "gid - muestra el grupo principal y los grupos secundarios del usuario"
#define HELP_UNSETENV   "unsetenv- elimina variables de ambiente."



int globalstatret;
struct builtin_struct builtin_arr[] = {
       // { "cd", builtin_cd, HELP_CD },
      //  { "dir",builtin_dir, HELP_DIR},
        { "exit", builtin_exit, HELP_EXIT},
        { "help", builtin_help, HELP_HELP},
    //    { "history", builtin_history, HELP_HISTORY},
        { "getenv", builtin_getenv, HELP_GETENV},
        { "setenv", builtin_setenv, HELP_SETENV},
        { "pid", builtin_pid, HELP_PID},
        { "gid", builtin_gid, HELP_GID},
        { "status", builtin_status, HELP_STATUS},
        { "uid", builtin_uid, HELP_UID},
        {"unsetenv", builtin_unsetenv,HELP_UNSETENV},
        { NULL, NULL, NULL }
    };

struct builtin_struct * builtin_lookup(char *cmd)
{
    for(int i =0; builtin_arr[i].cmd!=NULL;i++){
        if (strcmp(builtin_arr[i].cmd,cmd)==0){
            return &builtin_arr[i];
        }
    }
    return NULL;
}

void malloc_for_list(char **argv) {
    for (int i = 0; i < MAXNUMBERWORDS; i++) {
        argv[i] = (char *)malloc_or_exit((MAXWORDS+1) * sizeof(char));
    }
}

void free_list(char **argv,int argc){
    for (int i = 0; i < argc; i++) {
        free(argv[i]);
    }
}

int print_minish_with_data(){
    char *dir = (char *)malloc_or_exit(4096*sizeof(char)+1);
    dir = getcwd(dir,4097);
    uid_t uid = geteuid();
    struct passwd *access = getpwuid(uid);
    if(dir != NULL && access){
        printf("(minish) (%s):%s> ",access->pw_name,dir);
        return 0;
    }
    return 1;
}


int main(void){
    char **argv = (char **)malloc_or_exit((MAXNUMBERWORDS+1)*sizeof (char *));
    char *buffer = (char *)malloc_or_exit((MAXLINE+1) * sizeof (char));
    globalstatret = 0;

    while (1) {
        print_minish_with_data();
        fgets(buffer, MAXLINE, stdin);
        malloc_for_list(argv);
        int argc = linea2argv(buffer, MAXNUMBERWORDS, argv);
        if (argc > 0) {
            globalstatret= ejecutar(argc,argv);
            free_list(argv,argc);
        }
    }
  
    free(argv);
    free(buffer);
    return 0;
}

    