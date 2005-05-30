/*=========================================================================

  Program:   Visualization Toolkit
  Module:    $RCSfile$

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
// .NAME vtkSystemIncludes - transition VTK to ANSI C++, centralize
// inclusion of system files
// .SECTION Description
// The vtkSystemIncludes centralizes the inclusion of system include
// files. (This is particularly important as VTK moves towards ANSI
// C++.)  For example, this include file enables user's to build VTK
// with STL (i.e., use std: :ostream and other standard ANSI C++
// functionality).  A configured flag in vtkConfigure.h
// (VTK_USE_ANSI_STDLIB) must be set to enable ANSI C++ compliance.

#ifndef __vtkSystemIncludes_h
#define __vtkSystemIncludes_h

/* first include the local configuration for this machine */
#define __VTK_SYSTEM_INCLUDES__INSIDE
#include "vtkWin32Header.h"
#undef __VTK_SYSTEM_INCLUDES__INSIDE

// The language wrapper files do not need the real streams.  They
// define VTK_STREAMS_FWD_ONLY so that the streams are only
// forward-declared.  This significantly improves compile time on some
// platforms.
#if defined(VTK_STREAMS_FWD_ONLY)
# include "vtkIOStreamFwd.h" // Forward-declare the C++ streams.
#else
# include "vtkIOStream.h"    // Include the real C++ streams.
#endif

// define the type of floating point interface used for old and new versions
// of VTK VTK42 and older use float and VTK 44 and newer use double for most
// of the API calls
#define vtkFloatingPointType vtkFloatingPointType 
typedef double vtkFloatingPointType;

// this should be removed at some point
#define VTK_USE_EXECUTIVES

// Some constants used throughout the code
#define VTK_LARGE_INTEGER VTK_INT_MAX
#define VTK_LARGE_FLOAT VTK_FLOAT_MAX

// Choose an implementation for vtkIdType.
#define VTK_HAS_ID_TYPE
#ifdef VTK_USE_64BIT_IDS
# if defined(VTK_SIZEOF_LONG) && VTK_SIZEOF_LONG == 8 && 0
typedef long vtkIdType;
#  define VTK_SIZEOF_ID_TYPE VTK_SIZEOF_LONG
#  define VTK_LARGE_ID VTK_LONG_MAX
# elif defined(VTK_TYPE_USE_LONG_LONG) && VTK_SIZEOF_LONG_LONG == 8
typedef long long vtkIdType;
#  define VTK_SIZEOF_ID_TYPE VTK_SIZEOF_LONG_LONG
#  define VTK_LARGE_ID VTK_LONG_LONG_MAX
# elif defined(VTK_TYPE_USE___INT64) && VTK_SIZEOF___INT64 == 8
typedef __int64 vtkIdType;
#  define VTK_SIZEOF_ID_TYPE VTK_SIZEOF___INT64
#  define VTK_LARGE_ID VTK___INT64_MAX
# else
#  error "VTK_USE_64BIT_IDS is ON but no 64-bit integer type is available."
# endif
#else
typedef int vtkIdType;
# define VTK_SIZEOF_ID_TYPE VTK_SIZEOF_INT
# define VTK_LARGE_ID VTK_INT_MAX
#endif

#define __VTK_SYSTEM_INCLUDES__INSIDE
#include "vtkOStreamWrapper.h"    // Include the ostream wrapper.
#include "vtkOStrStreamWrapper.h" // Include the ostrstream wrapper.
#undef __VTK_SYSTEM_INCLUDES__INSIDE

// Include generic stuff.
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Borland C++ defines several of the stdlib.h and string.h symbols in
// sub-headers search.h and mem.h.  These sub-headers have using
// declarations to pull functions from the std namespace to the global
// namespace, but they are defined only if the header was not included
// through the C++-style cstdlib or cstring header.  These outer
// headers are included by the streams library in C++-style and
// include blockers are put in place that prevent including the
// C-style versions from ever including the sub-headers.  Therefore we
// have to include the sub-headers here to get the using declarations.
#if defined(__BORLANDC__)
# include <mem.h>    /* mem... functions from string.h */
# include <search.h> /* search functions from stdlib.h */
#endif

// These types are returned by GetDataType to indicate pixel type.
#define VTK_VOID            0
#define VTK_BIT             1 
#define VTK_CHAR            2
#define VTK_SIGNED_CHAR    15
#define VTK_UNSIGNED_CHAR   3
#define VTK_SHORT           4
#define VTK_UNSIGNED_SHORT  5
#define VTK_INT             6
#define VTK_UNSIGNED_INT    7
#define VTK_LONG            8
#define VTK_UNSIGNED_LONG   9
#define VTK_FLOAT          10
#define VTK_DOUBLE         11 
#define VTK_ID_TYPE        12

// These types are not currently supported by GetDataType, but are 
// for completeness.
#define VTK_STRING         13
#define VTK_OPAQUE         14

// These types are enabled if VTK_TYPE_USE_LONG_LONG is defined.
#define VTK_LONG_LONG          16
#define VTK_UNSIGNED_LONG_LONG 17

// This type is enabled if VTK_TYPE_USE___INT64 is defined.
#define VTK___INT64            18

// This type is enabled if VTK_TYPE_USE___INT64 and
// VTK_TYPE_CONVERT_UI64_TO_DOUBLE are both defined.
#define VTK_UNSIGNED___INT64   19

// Some constant required for correct template performance
#define VTK_BIT_MIN                 0
#define VTK_BIT_MAX                 1
#if VTK_TYPE_CHAR_IS_SIGNED
# define VTK_CHAR_MIN VTK_SIGNED_CHAR_MIN
# define VTK_CHAR_MAX VTK_SIGNED_CHAR_MAX
#else
# define VTK_CHAR_MIN VTK_UNSIGNED_CHAR_MIN
# define VTK_CHAR_MAX VTK_UNSIGNED_CHAR_MAX
#endif
#define VTK_SIGNED_CHAR_MIN         static_cast<char>(0x80)
#define VTK_SIGNED_CHAR_MAX         static_cast<char>(0x7f)
#define VTK_UNSIGNED_CHAR_MIN       static_cast<unsigned char>(0u)
#define VTK_UNSIGNED_CHAR_MAX       static_cast<unsigned char>(0xffu)
#define VTK_SHORT_MIN               static_cast<short>(0x8000)
#define VTK_SHORT_MAX               static_cast<short>(0x7fff)
#define VTK_UNSIGNED_SHORT_MIN      static_cast<unsigned short>(0u)
#define VTK_UNSIGNED_SHORT_MAX      static_cast<unsigned short>(0xffffu)
#define VTK_INT_MIN                 static_cast<int>(~(~0u >> 1))
#define VTK_INT_MAX                 static_cast<int>(~0u >> 1)
#define VTK_UNSIGNED_INT_MIN        static_cast<unsigned int>(0)
#define VTK_UNSIGNED_INT_MAX        static_cast<unsigned int>(~0u)
#define VTK_LONG_MIN                static_cast<long>(~(~0ul >> 1))
#define VTK_LONG_MAX                static_cast<long>(~0ul >> 1)
#define VTK_UNSIGNED_LONG_MIN       static_cast<unsigned long>(0ul)
#define VTK_UNSIGNED_LONG_MAX       static_cast<unsigned long>(~0ul)
#define VTK_FLOAT_MIN               -1.0e+38f
#define VTK_FLOAT_MAX                1.0e+38f
#define VTK_DOUBLE_MIN              -1.0e+299
#define VTK_DOUBLE_MAX               1.0e+299
#if defined(VTK_SIZEOF_LONG_LONG)
# define VTK_LONG_LONG_MIN          static_cast<long long>(~(~0ull >> 1))
# define VTK_LONG_LONG_MAX          static_cast<long long>(~0ull >> 1)
# define VTK_UNSIGNED_LONG_LONG_MIN static_cast<unsigned long long>(0ull)
# define VTK_UNSIGNED_LONG_LONG_MAX static_cast<unsigned long long>(~0ull)
#endif
#if defined(VTK_SIZEOF___INT64)
# define VTK___INT64_MIN            static_cast<__int64>(~(~0ui64 >> 1))
# define VTK___INT64_MAX            static_cast<__int64>(~0ui64 >> 1)
# define VTK_UNSIGNED___INT64_MIN   static_cast<unsigned __int64>(0ui64)
# define VTK_UNSIGNED___INT64_MAX   static_cast<unsigned __int64>(~0ui64)
#endif

// These types are returned to distinguish data object types
#define VTK_POLY_DATA                       0
#define VTK_STRUCTURED_POINTS               1
#define VTK_STRUCTURED_GRID                 2
#define VTK_RECTILINEAR_GRID                3
#define VTK_UNSTRUCTURED_GRID               4
#define VTK_PIECEWISE_FUNCTION              5
#define VTK_IMAGE_DATA                      6
#define VTK_DATA_OBJECT                     7
#define VTK_DATA_SET                        8
#define VTK_POINT_SET                       9
#define VTK_UNIFORM_GRID                   10
#define VTK_COMPOSITE_DATA_SET             11
#define VTK_HIERARCHICAL_DATA_SET          12
#define VTK_MULTI_BLOCK_DATA_SET           13
#define VTK_HIERARCHICAL_BOX_DATA_SET      14
#define VTK_GENERIC_DATA_SET               15

// These types define error codes for vtk functions
#define VTK_OK                 1
#define VTK_ERROR              2

// These types define different text properties
#define VTK_ARIAL     0
#define VTK_COURIER   1
#define VTK_TIMES     2

#define VTK_TEXT_LEFT     0
#define VTK_TEXT_CENTERED 1
#define VTK_TEXT_RIGHT    2

#define VTK_TEXT_BOTTOM 0
#define VTK_TEXT_TOP    2

#define VTK_TEXT_GLOBAL_ANTIALIASING_SOME 0
#define VTK_TEXT_GLOBAL_ANTIALIASING_NONE 1
#define VTK_TEXT_GLOBAL_ANTIALIASING_ALL 2

#define VTK_LUMINANCE       1
#define VTK_LUMINANCE_ALPHA 2
#define VTK_RGB             3
#define VTK_RGBA            4

#define VTK_COLOR_MODE_DEFAULT 0
#define VTK_COLOR_MODE_MAP_SCALARS 1

// Constants for InterpolationType
#define VTK_NEAREST_INTERPOLATION       0
#define VTK_LINEAR_INTERPOLATION        1

// For volume rendering
#define VTK_MAX_VRCOMP                  4

// For multithreading

// The maximum number of threads allowed
#ifdef VTK_USE_SPROC
#define VTK_MAX_THREADS              32
#endif

#ifdef VTK_USE_PTHREADS
#define VTK_MAX_THREADS              32
#endif

#ifdef VTK_USE_WIN32_THREADS
#define VTK_MAX_THREADS              8
#endif

#ifndef VTK_USE_WIN32_THREADS
#ifndef VTK_USE_SPROC
#ifndef VTK_USE_PTHREADS
#define VTK_MAX_THREADS              1
#endif
#endif
#endif

// If VTK_USE_PTHREADS is defined, then the multithreaded
// function is of type void *, and returns NULL
// Otherwise the type is void which is correct for WIN32
// and SPROC
#ifdef VTK_USE_PTHREADS
#define VTK_THREAD_RETURN_VALUE  NULL
#define VTK_THREAD_RETURN_TYPE   void *
#endif

#ifdef VTK_USE_WIN32_THREADS
#define VTK_THREAD_RETURN_VALUE 0
#define VTK_THREAD_RETURN_TYPE vtkWindowsDWORD __stdcall
#endif

#if !defined(VTK_USE_PTHREADS) && !defined(VTK_USE_WIN32_THREADS)
#define VTK_THREAD_RETURN_VALUE
#define VTK_THREAD_RETURN_TYPE void
#endif

// For encoding

#define VTK_ENCODING_NONE         0 // to specify that no encoding should occur
#define VTK_ENCODING_US_ASCII     1
#define VTK_ENCODING_UNICODE      2
#define VTK_ENCODING_UTF_8        3
#define VTK_ENCODING_ISO_8859_1   4
#define VTK_ENCODING_ISO_8859_2   5
#define VTK_ENCODING_ISO_8859_3   6
#define VTK_ENCODING_ISO_8859_4   7
#define VTK_ENCODING_ISO_8859_5   8
#define VTK_ENCODING_ISO_8859_6   9
#define VTK_ENCODING_ISO_8859_7   10
#define VTK_ENCODING_ISO_8859_8   11
#define VTK_ENCODING_ISO_8859_9   12
#define VTK_ENCODING_ISO_8859_10  13
#define VTK_ENCODING_ISO_8859_11  14
#define VTK_ENCODING_ISO_8859_12  15
#define VTK_ENCODING_ISO_8859_13  16
#define VTK_ENCODING_ISO_8859_14  17
#define VTK_ENCODING_ISO_8859_15  18
#define VTK_ENCODING_ISO_8859_16  19
#define VTK_ENCODING_UNKNOWN      20  // leave this one at the end

//----------------------------------------------------------------------------
// Define named types and constants corresponding to specific integer
// and floating-point sizes and signedness.

// Select an 8-bit integer type.
#if VTK_SIZEOF_CHAR == 1
typedef unsigned char vtkTypeUInt8;
typedef signed char   vtkTypeInt8;
# define VTK_TYPE_UINT8 VTK_UNSIGNED_CHAR
# if VTK_TYPE_CHAR_IS_SIGNED
#  define VTK_TYPE_INT8 VTK_CHAR
# else
#  define VTK_TYPE_INT8 VTK_SIGNED_CHAR
# endif
#else
# error "No native data type can represent an 8-bit integer."
#endif

// Select a 16-bit integer type.
#if VTK_SIZEOF_SHORT == 2
typedef unsigned short vtkTypeUInt16;
typedef signed short   vtkTypeInt16;
# define VTK_TYPE_UINT16 VTK_UNSIGNED_SHORT
# define VTK_TYPE_INT16 VTK_SHORT
#elif VTK_SIZEOF_INT == 2
typedef unsigned int vtkTypeUInt16;
typedef signed int   vtkTypeInt16;
# define VTK_TYPE_UINT16 VTK_UNSIGNED_INT
# define VTK_TYPE_INT16 VTK_INT
#else
# error "No native data type can represent a 16-bit integer."
#endif

// Select a 32-bit integer type.
#if VTK_SIZEOF_INT == 4
typedef unsigned int vtkTypeUInt32;
typedef signed int   vtkTypeInt32;
# define VTK_TYPE_UINT32 VTK_UNSIGNED_INT
# define VTK_TYPE_INT32 VTK_INT
#elif VTK_SIZEOF_LONG == 4
typedef unsigned long vtkTypeUInt32;
typedef signed long   vtkTypeInt32;
# define VTK_TYPE_UINT32 VTK_UNSIGNED_LONG
# define VTK_TYPE_INT32 VTK_LONG
#else
# error "No native data type can represent a 32-bit integer."
#endif

// Select a 64-bit integer type.
#if VTK_SIZEOF_LONG == 8
typedef unsigned long vtkTypeUInt64;
typedef signed long   vtkTypeInt64;
# define VTK_TYPE_UINT64 VTK_UNSIGNED_LONG
# define VTK_TYPE_INT64 VTK_LONG
#elif defined(VTK_TYPE_USE_LONG_LONG) && VTK_SIZEOF_LONG_LONG == 8
typedef unsigned long long vtkTypeUInt64;
typedef signed long long   vtkTypeInt64;
# define VTK_TYPE_UINT64 VTK_UNSIGNED_LONG_LONG
# define VTK_TYPE_INT64 VTK_LONG_LONG
#elif defined(VTK_TYPE_USE___INT64) && VTK_SIZEOF___INT64 == 8
typedef unsigned __int64 vtkTypeUInt64;
typedef signed __int64   vtkTypeInt64;
# define VTK_TYPE_UINT64 VTK_UNSIGNED___INT64
# define VTK_TYPE_INT64 VTK___INT64
#else
# error "No native data type can represent a 64-bit integer."
#endif

// Select a 32-bit floating point type.
#if VTK_SIZEOF_FLOAT == 4
typedef float vtkTypeFloat32;
# define VTK_TYPE_FLOAT32 VTK_FLOAT
#else
# error "No native data type can represent a 32-bit floating point value."
#endif

// Select a 64-bit floating point type.
#if VTK_SIZEOF_DOUBLE == 8
typedef double vtkTypeFloat64;
# define VTK_TYPE_FLOAT64 VTK_DOUBLE
#else
# error "No native data type can represent a 64-bit floating point value."
#endif

#endif
