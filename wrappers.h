/*
 * wrappers.h
 *
 * declaración de funciones de wrappers.c
 *
 */

#include <stdio.h>
#include <stdlib.h>

extern FILE * fopen_or_exit(const char *pathname, const char *mode);
extern size_t fread_or_exit(void *ptr, size_t size, size_t nmemb, FILE *stream);
extern size_t fwrite_or_exit(const void *ptr, size_t size, size_t nmemb, FILE *stream);
extern void * calloc_or_exit(size_t nmemb, size_t size);
extern void * malloc_or_exit(size_t size);
extern void * realloc_or_exit(void *ptr, size_t size);
extern char * strdup_or_exit(const char *s);
