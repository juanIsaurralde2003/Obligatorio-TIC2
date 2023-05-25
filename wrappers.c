/*
 * wrappers.c
 *
 * contiene funciones "wrapper", que invocan una función de la biblioteca estándar,
 * y controlan el valor de retorno.
 * Si es el valor esperado, lo retornan,
 * Si el valor indica un error, en general usan la función "error(3)" para finalizar
 * la ejecución.
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <error.h>
#include <errno.h>
#include <string.h>

// wrapper para fopen
FILE *
fopen_or_exit(const char *pathname, const char *mode)
{
    FILE *fp;

    if ( (fp = fopen(pathname, mode)) == NULL ) {
        error(EXIT_FAILURE, errno, "fopen error file '%s', mode '%s' - exit\n", pathname, mode);
    }
    return fp;
}

size_t
fread_or_exit(void *ptr, size_t size, size_t nmemb, FILE *stream)
// wrapper para fread
{
    size_t n_read;

    n_read = fread(ptr, size, nmemb, stream);
    if (n_read != nmemb ) {
        if (feof(stream)) {
            error(EXIT_FAILURE, 0, "fread EOF - exit\n");
        } else {
            error(EXIT_FAILURE, errno, "fread error - exit\n");
        }
    }
    return n_read;
}

// wrapper para fwrite
size_t
fwrite_or_exit(const void *ptr, size_t size, size_t nmemb, FILE *stream)
{
    size_t n_written;

    n_written = fwrite(ptr, size, nmemb, stream);
    if (n_written != nmemb ) {
        error(EXIT_FAILURE, errno, "fwrite error - exit\n");
    }
    return n_written;
}

// wrapper para calloc
void *
calloc_or_exit(size_t nmemb, size_t size)
{
    void *p = calloc(nmemb, size);
    if (p == NULL) {        // no hay memoria para asignar
        error(EXIT_FAILURE, errno, "calloc error - exit\n");
    }
    return p;
}

// wrapper para malloc
void *
malloc_or_exit(size_t size)
{
    void *p = malloc(size);
    if (p == NULL) {        // no hay memoria para asignar
        error(EXIT_FAILURE, errno, "malloc error - exit\n");
    }
    return p;
}

// wrapper para realloc
void *
realloc_or_exit(void *ptr, size_t size)
{
    void *p = realloc(ptr, size);
    if (p == NULL) {        // no hay memoria para asignar
        error(EXIT_FAILURE, errno, "realloc error - exit\n");
    }
    return p;
}

// wrapper para strdup - se implementa "a mano" porque no es parte del standard actual C99

char *
strdup_or_exit(const char *s)
{
    char *p;
    p = malloc_or_exit(strlen(s) + 1);
    (void) strcpy(p, s);
    return p;
}
