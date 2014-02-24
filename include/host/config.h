/* include/config.h.  Generated by configure.  */
/* include/config.h.in.  Generated from configure.ac by autoheader.  */

#define __WINE_CONFIG_H

#define HAVE_SPAWNVP 1

/* Define to 1 if you have the `z' library (-lz). */
#define HAVE_ZLIB 1

/* Define to 1 if you have the <zlib.h> header file. */
#define HAVE_ZLIB_H 1

/* Define to 1 if you have the <float.h> header file. */
#define HAVE_FLOAT_H 1

/* Define to 1 if you have the `getpagesize' function. */
#define HAVE_GETPAGESIZE 1

/* Define to 1 if you have the <inttypes.h> header file. */
#define HAVE_INTTYPES_H 1

/* Define to 1 if the system has the type `long long'. */
#define HAVE_LONG_LONG 1

/* Define to 1 if you have the `memmove' function. */
#define HAVE_MEMMOVE 1

/* Define to 1 if you have the <memory.h> header file. */
#define HAVE_MEMORY_H 1

/* Define to 1 if the system has the type `mode_t'. */
#define HAVE_MODE_T 1

/* Define to 1 if the system has the type `size_t'. */
#define HAVE_SIZE_T 1

/* Define to 1 if you have the `snprintf' function. */
#define HAVE_SNPRINTF 1

/* Define to 1 if the system has the type `ssize_t'. */
#define HAVE_SSIZE_T 1

/* Define to 1 if you have the <stdint.h> header file. */
#define HAVE_STDINT_H 1

/* Define to 1 if you have the <stdlib.h> header file. */
#define HAVE_STDLIB_H 1

/* Define to 1 if you have the `strcasecmp' function. */
#if !defined(_WIN32) && !defined(_WIN64)
#define HAVE_STRCASECMP 1
#endif

/* Define to 1 if you have the `strerror' function. */
#define HAVE_STRERROR 1

/* Define to 1 if you have the <strings.h> header file. */
#define HAVE_STRINGS_H 1

/* Define to 1 if you have the <string.h> header file. */
#define HAVE_STRING_H 1

/* Define to 1 if you have the `strncasecmp' function. */
#if !defined(_WIN32) && !defined(_WIN64)
#define HAVE_STRNCASECMP 1
#endif

/* Define to 1 if you have the <tiffio.h> header file. */
#define HAVE_TIFFIO_H 1

/* Define to 1 if you have the <sys/param.h> header file. */
#define HAVE_SYS_PARAM_H 1

/* Define to 1 if you have the <sys/stat.h> header file. */
#define HAVE_SYS_STAT_H 1

/* Define to 1 if you have the <sys/time.h> header file. */
#ifndef _MSC_VER
  #define HAVE_SYS_TIME_H 1
#endif

/* Define to 1 if you have the <sys/types.h> header file. */
#define HAVE_SYS_TYPES_H 1

/* Define if you have the timezone variable */
#define HAVE_TIMEZONE 1

/* Define to 1 if you have the <unistd.h> header file. */
#if !defined(_MSC_VER)
#define HAVE_UNISTD_H 1
#endif

/* Define to 1 if you have the <utime.h> header file. */
#define HAVE_UTIME_H 1

/* Define if we have va_copy */
#define HAVE_VA_COPY 1

/* Define to 1 if you have the `vsnprintf' function. */
#define HAVE_VSNPRINTF 1

/* Define to 1 if you have the `_snprintf' function. */
#define HAVE__SNPRINTF 1

/* Define to 1 if you have the `_stricmp' function. */
#if defined(_WIN32) || defined(_WIN64)
#define HAVE__STRICMP 1
#endif

/* Define to 1 if you have the `_strnicmp' function. */
#if defined(_WIN32) || defined(_WIN64)
#define HAVE__STRNICMP 1
#endif

/* Define to 1 if you have the `_vsnprintf' function. */
#define HAVE__VSNPRINTF 1

/* Define if we have __va_copy */
#define HAVE___VA_COPY 1
// 
/* Define to the address where bug reports for this package should be sent. */
#define PACKAGE_BUGREPORT "ros-dev@reactos.org"

/* Define to the full name of this package. */
#define PACKAGE_NAME "ReactOS"

/* Define to the one symbol short name of this package. */
#define PACKAGE_TARNAME "ReactOS"

/* Define to the version of this package. */
#define PACKAGE_VERSION "<undefined version>"

/* Define to the full name and version of this package. */
#define PACKAGE_STRING PACKAGE_NAME " " PACKAGE_VERSION

/* Define to 1 if you have the ANSI C header files. */
#define STDC_HEADERS 1

/* Define to 1 if the X Window System is missing or not being used. */
#define X_DISPLAY_MISSING 1

/* Define to 1 if `lex' declares `yytext' as a `char *' by default, not a
   `char[]'. */
#define YYTEXT_POINTER 1

#if defined(_MSC_VER) || defined(__WATCOMC__)
#define inline __inline
#endif

#if defined(__WATCOMC__)
#define __attribute__(x)  /*nothing */
#endif


