// c_format.h
// Base library
// 
// Based on libxorp 1.4.
//
// Copyright (c) 2001-2007 International Computer Science Institute.
// Permission is hereby granted, free of charge, to any person obtaining a
// copy of this software and associated documentation files (the "Software")
// to deal in the Software without restriction, subject to the conditions
// listed in the XORP LICENSE file. These conditions include: you must
// preserve this copyright notice, and you cannot mention the copyright
// holders in advertising related to the Software without their permission.
// The Software is provided WITHOUT ANY WARRANTY, EXPRESS OR IMPLIED. This
// notice is a summary of the XORP LICENSE file; the license in that file is
// legally binding.

#ifndef BASE_C_FORMAT_H__
#define BASE_C_FORMAT_H__

#include <cstdarg> // for va_****
#include <vector>
#include <string>

#define FORMAT_BUFSIZE 4096

using std::vector;
using std::string;

//
// c_format is a macro that creates a string from a c-style format
// string.  It takes the same arguments as printf, but %n is illegal and
// will cause abort to be called.
//
// Pseudo prototype:
//   string c_format(const char* format, ...);
//
// In practice c_format is a nasty macro, but by doing this we can check
// the compile time arguments are sane and the run time arguments.
//

#define c_format(format, args...)                 \
    (c_format_validate(format, arg_count(args)), do_c_format(format, ## args))

//
// Template magic to allow us to count the number of varargs passed to
// the macro c_format.  We could also count the size of the var args data
// for extra protection if we were doing the formatting ourselves...
//

// Comment this out to find unnecessary calls to c_format()
int arg_count();

template <class A>
int arg_count(A) { return 1; }

template <class A, class B>
int arg_count(A,B) { return 2; }

template <class A, class B, class C>
int arg_count(A,B,C) { return 3; }

template <class A, class B, class C, class D>
int arg_count(A,B,C,D) { return 4; }

template <class A, class B, class C, class D, class E>
int arg_count(A,B,C,D,E) { return 5; }

template <class A, class B, class C,class D, class E, class F>
int arg_count(A,B,C,D,E,F) { return 6; }

template <class A, class B, class C, class D, class E, class F, class G>
int arg_count(A,B,C,D,E,F,G) { return 7; }

template <class A, class B, class C, class D, class E, class F, class G,
    class H>
int arg_count(A,B,C,D,E,F,G,H) { return 8; }

template <class A, class B, class C, class D, class E, class F, class G,
    class H, class I>
int arg_count(A,B,C,D,E,F,G,H,I) { return 9; }

template <class A, class B, class C, class D, class E, class F, class G,
    class H, class I, class J>
int arg_count(A,B,C,D,E,F,G,H,I,J) { return 10; }

template <class A, class B, class C, class D, class E, class F, class G,
    class H, class I, class J, class K>
int arg_count(A,B,C,D,E,F,G,H,I,J,K) { return 11; }

template <class A, class B, class C, class D, class E, class F, class G,
    class H, class I, class J, class K, class L>
int arg_count(A,B,C,D,E,F,G,H,I,J,K,L) { return 12; }

template <class A, class B, class C, class D, class E, class F, class G,
    class H, class I, class J, class K, class L, class M>
int arg_count(A,B,C,D,E,F,G,H,I,J,K,L,M) { return 13; }

template <class A, class B, class C, class D, class E, class F, class G,
    class H, class I, class J, class K, class L, class M, class N>
int arg_count(A,B,C,D,E,F,G,H,I,J,K,L,M,N) { return 14; }

void c_format_validate(const char* fmt, int exp_count);

#if defined(__printflike)
std::string do_c_format(const char* fmt, ...) __printflike(1, 2);
#elif (defined(__GNUC__))
std::string do_c_format(const char* fmt, ...) __attribute__((__format__(printf, 1, 2)));
#else
std::string do_c_format(const char* fmt, ...);
#endif

#endif // BASE_C_FORMAT_H__
