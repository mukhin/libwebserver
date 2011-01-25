// basicmacros.h
// Base library
// 
// Copyright 2010 LibWebserver Authors. All rights reserved.

#ifndef BASE_BASICMACROS_H
#define BASE_BASICMACROS_H

#ifdef ENABLE_DEBUG
# include <pthread.h> // for mutexes
# include <assert.h>
# include <iostream> // for std::cerr, std::endl
# include <time.h> // for struct tm, ::time, ::ctime
# define ASSERT(x) assert(x)
# define ASSERT_DECLARATION(x) x
# define LOG(x) std::cerr << x << std::endl << std::flush
# define LOG2(x,y) std::cerr << x << y << std::endl << std::flush

# define TIMELOG(x) { time_t rawtime; \
    ::time(&rawtime); \
    std::cerr << ::ctime(&rawtime) << "\t" << x << std::endl; }

# define SAFELOG(x) { \
    pthread_mutex_t safelog_mutex__; \
    int ret__ = pthread_mutex_init(&safelog_mutex__, 0); \
    pthread_mutex_lock(&safelog_mutex__); \
    std::cerr << x << std::endl << std::flush; \
    pthread_mutex_unlock(&safelog_mutex__); \
    pthread_mutex_destroy(&safelog_mutex__); \
    (void)ret__; }

#else // !ENABLE_DEBUG
# define ASSERT(x)
# define ASSERT_DECLARATION(x)
# define LOG(x)
# define LOG2(x,y)
# define SAFELOG(x)
# define TIMELOG(x)
#endif // ENABLE_DEBUG

// Compile time assertion.
#ifndef static_assert
# define static_assert(a) switch (a) case 0: case (a):
#endif // static_assert

// OS specific macros.
#ifdef __LINUX__
# ifndef OS_LINUX
#  define OS_LINUX
# endif // OS_LINUX
#elif __MACOSX__
# ifndef OS_MACOSX
#  define OS_MACOSX
# endif // OS_MACOSX
#elif __APPLE__
# ifndef OS_MACOSX
#  define OS_MACOSX
# endif // OS_MACOSX
#endif


#endif // BASE_BASICMACROS_H
