/*
 * wpa_supplicant/hostapd / Internal implementation of OS specific functions
 * Copyright (c) 2005-2006, Jouni Malinen <j@w1.fi>
 *
 * This software may be distributed under the terms of the BSD license.
 * See README for more details.
 */



#ifndef OS_H
#define OS_H


#include <stdlib.h>


/****************************************************************************
 *
 * MEMORY MANAGEMENT OPTIONS
 *
 ****************************************************************************/


#ifndef MTK_WIFI_OS_MM_OPTION
#define MTK_WIFI_OS_MM_OPTION   1
#endif

#if MTK_WIFI_OS_MM_OPTION > 3
#error "Invalid Wi-Fi memory management option"
#endif

#if MTK_WIFI_OS_MM_OPTION == 1
#define MTK_FREERTOS_MM_ENABLE
#endif

#if MTK_WIFI_OS_MM_OPTION == 2
#define MTK_WIFI_OS_ISOLATED_MM_ENABLE
#endif

#if MTK_WIFI_OS_MM_OPTION == 3
#define MTK_FREERTOS_MM_DEBUG_ENABLE
#endif


/****************************************************************************
 *
 * MACROS
 *
 ****************************************************************************/


/* Helper macros for handling struct os_time */

#define os_time_before(a, b) \
    ((a)->sec < (b)->sec || \
     ((a)->sec == (b)->sec && (a)->usec < (b)->usec))

#define os_time_sub(a, b, res) do { \
    (res)->sec = (a)->sec - (b)->sec; \
    (res)->usec = (a)->usec - (b)->usec; \
    if ((res)->usec < 0) { \
        (res)->sec--; \
        (res)->usec += 1000000; \
    } \
} while (0)


/****************************************************************************
 *
 * DATA STRUCTURE
 *
 ****************************************************************************/


typedef long os_time_t;


struct os_time {
    os_time_t sec;
    os_time_t usec;
};


struct os_tm {
    int sec; /* 0..59 or 60 for leap seconds */
    int min; /* 0..59 */
    int hour; /* 0..23 */
    int day; /* 1..31 */
    int month; /* 1..12 */
    int year; /* Four digit year */
};


/****************************************************************************
 *
 * PUBLIC FUNCTIONS
 *
 ****************************************************************************/


/**
 * os_daemonize - Run in the background (detach from the controlling terminal)
 * @pid_file: File name to write the process ID to or %NULL to skip this
 * Returns: 0 on success, -1 on failure
 */
int os_daemonize(const char *pid_file);

/**
 * os_daemonize_terminate - Stop running in the background (remove pid file)
 * @pid_file: File name to write the process ID to or %NULL to skip this
 */
void os_daemonize_terminate(const char *pid_file);


/**
 * os_rel2abs_path - Get an absolute path for a file
 * @rel_path: Relative path to a file
 * Returns: Absolute path for the file or %NULL on failure
 *
 * This function tries to convert a relative path of a file to an absolute path
 * in order for the file to be found even if current working directory has
 * changed. The returned value is allocated and caller is responsible for
 * freeing it. It is acceptable to just return the same path in an allocated
 * buffer, e.g., return strdup(rel_path). This function is only used to find
 * configuration files when os_daemonize() may have changed the current working
 * directory and relative path would be pointing to a different location.
 */
char *os_rel2abs_path(const char *rel_path);

/**
 * os_program_init - Program initialization (called at start)
 * Returns: 0 on success, -1 on failure
 *
 * This function is called when a programs starts. If there are any OS specific
 * processing that is needed, it can be placed here. It is also acceptable to
 * just return 0 if not special processing is needed.
 */
int os_program_init(void);

/**
 * os_program_deinit - Program deinitialization (called just before exit)
 *
 * This function is called just before a program exists. If there are any OS
 * specific processing, e.g., freeing resourced allocated in os_program_init(),
 * it should be done here. It is also acceptable for this function to do
 * nothing.
 */
void os_program_deinit(void);

/**
 * os_setenv - Set environment variable
 * @name: Name of the variable
 * @value: Value to set to the variable
 * @overwrite: Whether existing variable should be overwritten
 * Returns: 0 on success, -1 on error
 *
 * This function is only used for wpa_cli action scripts. OS wrapper does not
 * need to implement this if such functionality is not needed.
 */
int os_setenv(const char *name, const char *value, int overwrite);

/**
 * os_unsetenv - Delete environent variable
 * @name: Name of the variable
 * Returns: 0 on success, -1 on error
 *
 * This function is only used for wpa_cli action scripts. OS wrapper does not
 * need to implement this if such functionality is not needed.
 */
int os_unsetenv(const char *name);

/**
 * os_readfile - Read a file to an allocated memory buffer
 * @name: Name of the file to read
 * @len: For returning the length of the allocated buffer
 * Returns: Pointer to the allocated buffer or %NULL on failure
 *
 * This function allocates memory and reads the given file to this buffer. Both
 * binary and text files can be read with this function. The caller is
 * responsible for freeing the returned buffer with os_free().
 */
char *os_readfile(const char *name, size_t *len);


/*
 * The following functions are wrapper for standard ANSI C or POSIX functions.
 * By default, they are just defined to use the standard function name and no
 * os_*.c implementation is needed for them. This avoids extra function calls
 * by allowing the C pre-processor take care of the function name mapping.
 *
 * If the target system uses a C library that does not provide these functions,
 * build_config.h can be used to define the wrappers to use a different
 * function name. This can be done on function-by-function basis since the
 * defines here are only used if build_config.h does not define the os_* name.
 * If needed, os_*.c file can be used to implement the functions that are not
 * included in the C library on the target system. Alternatively,
 * OS_NO_C_LIB_DEFINES can be defined to skip all defines here in which case
 * these functions need to be implemented in os_*.c file for the target system.
 */



/**
 * os_memcpy - Copy memory area
 * @dest: Destination
 * @src: Source
 * @n: Number of bytes to copy
 * Returns: dest
 *
 * The memory areas src and dst must not overlap. os_memmove() can be used with
 * overlapping memory.
 */
void *os_memcpy(void *dest, const void *src, size_t n);

/**
 * os_memmove - Copy memory area
 * @dest: Destination
 * @src: Source
 * @n: Number of bytes to copy
 * Returns: dest
 *
 * The memory areas src and dst may overlap.
 */
void *os_memmove(void *dest, const void *src, size_t n);

/**
 * os_memset - Fill memory with a constant byte
 * @s: Memory area to be filled
 * @c: Constant byte
 * @n: Number of bytes started from s to fill with c
 * Returns: s
 */
void *os_memset(void *s, int c, size_t n);


/**
 * os_memcmp - Compare memory areas
 * @s1: First buffer
 * @s2: Second buffer
 * @n: Maximum numbers of octets to compare
 * Returns: An integer less than, equal to, or greater than zero if s1 is
 * found to be less than, to match, or be greater than s2. Only first n
 * characters will be compared.
 */
int os_memcmp(const void *s1, const void *s2, size_t n);

/**
 * os_strlen - Calculate the length of a string
 * @s: '\0' terminated string
 * Returns: Number of characters in s (not counting the '\0' terminator)
 */
size_t os_strlen(const char *s);


/**
 * os_strcasecmp - Compare two strings ignoring case
 * @s1: First string
 * @s2: Second string
 * Returns: An integer less than, equal to, or greater than zero if s1 is
 * found to be less than, to match, or be greatred than s2
 */
int os_strcasecmp(const char *s1, const char *s2);


/**
 * os_strncasecmp - Compare two strings ignoring case
 * @s1: First string
 * @s2: Second string
 * @n: Maximum numbers of characters to compare
 * Returns: An integer less than, equal to, or greater than zero if s1 is
 * found to be less than, to match, or be greater than s2. Only first n
 * characters will be compared.
 */
int os_strncasecmp(const char *s1, const char *s2, size_t n);


/**
 * os_strchr - Locate the first occurrence of a character in string
 * @s: String
 * @c: Character to search for
 * Returns: Pointer to the matched character or %NULL if not found
 */
char *os_strchr(const char *s, int c);


/**
 * os_strrchr - Locate the last occurrence of a character in string
 * @s: String
 * @c: Character to search for
 * Returns: Pointer to the matched character or %NULL if not found
 */
char *os_strrchr(const char *s, int c);


/**
 * os_strcmp - Compare two strings
 * @s1: First string
 * @s2: Second string
 * Returns: An integer less than, equal to, or greater than zero if s1 is
 * found to be less than, to match, or be greatred than s2
 */
int os_strcmp(const char *s1, const char *s2);


/**
 * os_strncmp - Compare two strings
 * @s1: First string
 * @s2: Second string
 * @n: Maximum numbers of characters to compare
 * Returns: An integer less than, equal to, or greater than zero if s1 is
 * found to be less than, to match, or be greater than s2. Only first n
 * characters will be compared.
 */
int os_strncmp(const char *s1, const char *s2, size_t n);


/**
 * os_strncpy - Copy a string
 * @dest: Destination
 * @src: Source
 * @n: Maximum number of characters to copy
 * Returns: dest
 */
char *os_strncpy(char *dest, const char *src, size_t n);


/**
 * os_strstr - Locate a substring
 * @haystack: String (haystack) to search from
 * @needle: Needle to search from haystack
 * Returns: Pointer to the beginning of the substring or %NULL if not found
 */
char *os_strstr(const char *haystack, const char *needle);


/**
 * os_snprintf - Print to a memory buffer
 * @str: Memory buffer to print into
 * @size: Maximum length of the str buffer
 * @format: printf format
 * Returns: Number of characters printed (not including trailing '\0').
 *
 * If the output buffer is truncated, number of characters which would have
 * been written is returned. Since some C libraries return -1 in such a case,
 * the caller must be prepared on that value, too, to indicate truncation.
 *
 * Note: Some C library implementations of snprintf() may not guarantee null
 * termination in case the output is truncated. The OS wrapper function of
 * os_snprintf() should provide this guarantee, i.e., to null terminate the
 * output buffer if a C library version of the function is used and if that
 * function does not guarantee null termination.
 *
 * If the target system does not include snprintf(), see, e.g.,
 * http://www.ijs.si/software/snprintf/ for an example of a portable
 * implementation of snprintf.
 */
int os_snprintf(char *str, size_t size, const char *format, ...);


/**
 * os_strlcpy - Copy a string with size bound and NUL-termination
 * @dest: Destination
 * @src: Source
 * @siz: Size of the target buffer
 * Returns: Total length of the target string (length of src) (not including
 * NUL-termination)
 *
 * This function matches in behavior with the strlcpy(3) function in OpenBSD.
 */
size_t os_strlcpy(char *dest, const char *src, size_t siz);

/**
 * memrlen - Returns the number of bytes until the last 0 in the string.
 * @in_src: byte array
 * @in_max_len: max length
 * Returns: Returns the number of bytes until the last 0 in the string.
 */
size_t os_memrlen(const void *in_src, size_t in_max_len);


/**
 * os_sleep - Sleep (sec, usec)
 * @sec: Number of seconds to sleep
 * @usec: Number of microseconds to sleep
 */
void os_sleep(os_time_t sec, os_time_t usec);


/**
 * os_get_time - Get current time (sec, usec)
 * @t: Pointer to buffer for the time
 * Returns: 0 on success, -1 on failure
 */
int os_get_time(struct os_time *t);


/**
 * os_random - Get pseudo random value (not necessarily very strong)
 * Returns: Pseudo random value
 */
unsigned long os_random(void);


/**
 * os_get_random - Get cryptographically strong pseudo random data
 * @buf: Buffer for pseudo random data
 * @len: Length of the buffer
 * Returns: 0 on success, -1 on failure
 */
int os_get_random(unsigned char *buf, size_t len);


#ifdef MTK_WIFI_OS_ISOLATED_MM_ENABLE
/*
 * This set of MM API uses isolated memory pool to separate Wi-Fi memory
 * from other general purpose memory requirements. Useful for evaluating
 * Wi-Fi memory requirements.
 */
void * wifi_os_mem_malloc  (size_t size);
void   wifi_os_mem_free    (void *ptr);
void * wifi_os_mem_realloc (void *ptr, size_t size);
void * wifi_os_mem_zalloc  (size_t size);
char * wifi_os_mem_strdup  (const char *s);

void   wifi_os_mem_dump    (void);

#define os_malloc_api(_b_)          wifi_os_mem_malloc (_b_)
#define os_free_api(_p_)            wifi_os_mem_free   (_p_)
#define os_realloc_api(_p_, _b_)    wifi_os_mem_realloc(_p_, _b_)
#define os_zalloc_api(_b_)          wifi_os_mem_zalloc (_b_)
#define os_strdup_api(_s_)          wifi_os_mem_strdup (_s_)
#endif


#ifdef MTK_FREERTOS_MM_ENABLE
/*
 * This set of MM API uses FreeRTOS API with realloc() implemented by
 * MediaTek because WPA Supplicant needs it but FreeRTOS didn't provide.
 */
void * os_mem_malloc  (size_t size);
void   os_mem_free    (void *ptr);
void * os_mem_realloc (void *ptr, size_t size);
void * os_mem_zalloc  (size_t size);
char * os_mem_strdup  (const char *s);

#define os_malloc_api(_b_)          os_mem_malloc (_b_)
#define os_free_api(_p_)            os_mem_free   (_p_)
#define os_realloc_api(_p_, _b_)    os_mem_realloc(_p_, _b_)
#define os_zalloc_api(_b_)          os_mem_zalloc (_b_)
#define os_strdup_api(_s_)          os_mem_strdup (_s_)
#endif


#ifdef MTK_FREERTOS_MM_DEBUG_ENABLE
/*
 * This set of MM API uses FreeRTOS API with realloc() implemented by
 * MediaTek because WPA Supplicant needs it but FreeRTOS didn't provide.
 */
void * os_mem_d_malloc  (const char *file, const int line, size_t size);
void   os_mem_d_free    (const char *file, const int line, void *ptr);
void * os_mem_d_realloc (const char *file, const int line, void *ptr, size_t size);
void * os_mem_d_zalloc  (const char *file, const int line, size_t size);
char * os_mem_d_strdup  (const char *s);

#define os_malloc_api(_b_)          os_mem_d_malloc (__FILE__, __LINE__, _b_)
#define os_free_api(_p_)            os_mem_d_free   (__FILE__, __LINE__, _p_)
#define os_realloc_api(_p_, _b_)    os_mem_d_realloc(__FILE__, __LINE__, _p_, _b_)
#define os_zalloc_api(_b_)          os_mem_d_zalloc (__FILE__, __LINE__, _b_)
#define os_strdup_api(_s_)          os_mem_d_strdup (_s_)
#endif


/**
 * os_zalloc - Allocate and zero memory
 * @size: Number of bytes to allocate
 * Returns: Pointer to allocated and zeroed memory or %NULL on failure
 *
 * Caller is responsible for freeing the returned buffer with os_free().
 */
#define os_zalloc   os_zalloc_api


/**
 * os_malloc - Allocate dynamic memory
 * @size: Size of the buffer to allocate
 * Returns: Allocated buffer or %NULL on failure
 *
 * Caller is responsible for freeing the returned buffer with os_free().
 */
#define os_malloc   os_malloc_api


/**
 * os_realloc - Re-allocate dynamic memory
 * @ptr: Old buffer from os_malloc() or os_realloc()
 * @size: Size of the new buffer
 * Returns: Allocated buffer or %NULL on failure
 *
 * Caller is responsible for freeing the returned buffer with os_free().
 * If re-allocation fails, %NULL is returned and the original buffer (ptr) is
 * not freed and caller is still responsible for freeing it.
 */
#define os_realloc  os_realloc_api


/**
 * os_free - Free dynamic memory
 * @ptr: Old buffer from os_malloc() or os_realloc(); can be %NULL
 */
#define os_free     os_free_api


/**
 * os_strdup - Duplicate a string
 * @s: Source string
 * Returns: Allocated buffer with the string copied into it or %NULL on failure
 *
 * Caller is responsible for freeing the returned buffer with os_free().
 */
#define os_strdup   os_strdup_api


/**
 * os_mktime - Convert broken-down time into seconds since 1970-01-01
 * @year: Four digit year
 * @month: Month (1 .. 12)
 * @day: Day of month (1 .. 31)
 * @hour: Hour (0 .. 23)
 * @min: Minute (0 .. 59)
 * @sec: Second (0 .. 60)
 * @t: Buffer for returning calendar time representation (seconds since
 * 1970-01-01 00:00:00)
 * Returns: 0 on success, -1 on failure
 *
 * Note: The result is in seconds from Epoch, i.e., in UTC, not in local time
 * which is used by POSIX mktime().
 */
int os_mktime(int year, int month, int day, int hour, int min, int sec,
              os_time_t *t);


int os_gmtime(os_time_t t, struct os_tm *tm);


/****************************************************************************
 *
 * For OS_REJECT_C_LIB_FUNCTIONS
 *
 ****************************************************************************/


#if (defined(malloc))
    #undef malloc
    #define malloc OS_DO_NOT_USE_malloc
#endif

#define realloc OS_DO_NOT_USE_realloc

#if (defined(free))
    #undef free
    #define free OS_DO_NOT_USE_free
#endif

#define memcpy OS_DO_NOT_USE_memcpy
#define memmove OS_DO_NOT_USE_memmove

#if (!defined(__CC_ARM)) && (!defined(__ICCARM__))//#ifndef __CC_ARM
    #define memset OS_DO_NOT_USE_memset
#endif

#define memcmp OS_DO_NOT_USE_memcmp

#if 0
    #undef strdup
    #define strdup OS_DO_NOT_USE_strdup
    #define strlen OS_DO_NOT_USE_strlen
    #define strcasecmp OS_DO_NOT_USE_strcasecmp
    #define strncasecmp OS_DO_NOT_USE_strncasecmp
    #undef strchr
    #define strchr OS_DO_NOT_USE_strchr
    #undef strcmp
    #define strcmp OS_DO_NOT_USE_strcmp
    #undef strncmp
    #define strncmp OS_DO_NOT_USE_strncmp
    #undef strncpy
    #define strncpy OS_DO_NOT_USE_strncpy
    #define strrchr OS_DO_NOT_USE_strrchr
    #define strstr OS_DO_NOT_USE_strstr
    #undef snprintf
    #define snprintf OS_DO_NOT_USE_snprintf
    #define strcpy OS_DO_NOT_USE_strcpy
#endif


#endif /* OS_H */
