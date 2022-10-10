#ifndef _LIB_H_INCLUDED_
#define _LIB_H_INCLUDED_

extern int is_debug;

#define DEBUG_CHAR(msg)            \
	do {                       \
		if (is_debug)      \
			puts(msg); \
	} while (0)

#define DEBUG_XVAL(value, column)               \
	do {                                    \
		if (is_debug)                   \
			putxval(value, column); \
	} while (0)

#define DEBUG_NEWLINE()             \
	do {                        \
		if (is_debug)       \
			puts("\n"); \
	} while (0)

// int putc(int c, FILE *stream);
int putc(unsigned char c);

// int puts(const char *s);
int puts(unsigned char *str);

// int getc(FILE *stream);
unsigned char getc(void);

// char *gets(char *s);
// このライブラリ関数では読み出した文字列の長さを返している。
int gets(unsigned char *buf);

int putxval(unsigned long value, int column);

// void *memset(void *s, int c, size_t n);
void *memset(void *s, int c, long len);

// void *memcpy(void *dest, const void *src, size_t n);
void *memcpy(void *dest, const void *src, long n);

// int memcmp(const void *s1, const void *s2, size_t n);
int memcmp(const void *s1, const void *s2, long n);

// size_t strlen(const char *s);
int strlen(const char *s);

// char *strcpy(char *dest, const char *src);
char *strcpy(char *dest, const char *src);

// int strcmp(const char *s1, const char *s2);
int strcmp(const char *s1, const char *s2);

// int strncmp(const char *s1, const char *s2, size_t n);
int strncmp(const char *s1, const char *s2, int n);

#endif
