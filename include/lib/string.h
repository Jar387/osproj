#ifndef STRING_H
#define STRING_H

void memcpy(void *dst, void *src, unsigned int size);
void memset(void *dst, unsigned int c, unsigned int size);
int memcmp(void *dst, void *src, unsigned int size);
char *strcpy(char *dst, const char *src);
char *strncpy(char *dst, const char *src, unsigned int size);
char *strcat(char *dst, const char *src);
char *strncat(char *dst, const char *src, unsigned int size);
int strcmp(const char *dst, const char *src);
int strncmp(const char *dst, const char *src, unsigned int size);

#endif
