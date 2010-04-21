/*
 * Copyright (C) 2010 DiamondCore <http://diamondcore.eu/>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#ifndef DIAMOND_DEFINE_H
#define DIAMOND_DEFINE_H

#include <sys/types.h>

#include <ace/Basic_Types.h>
#include <ace/Default_Constants.h>
#include <ace/OS_NS_dlfcn.h>
#include <ace/ACE_export.h>

#include "Platform/CompilerDefs.h"

#define DIAMOND_LITTLEENDIAN 0
#define DIAMOND_BIGENDIAN    1

#if !defined(DIAMOND_ENDIAN)
#  if defined (ACE_BIG_ENDIAN)
#    define DIAMOND_ENDIAN DIAMOND_BIGENDIAN
#  else //ACE_BYTE_ORDER != ACE_BIG_ENDIAN
#    define DIAMOND_ENDIAN DIAMOND_LITTLEENDIAN
#  endif //ACE_BYTE_ORDER
#endif //DIAMOND_ENDIAN

typedef ACE_SHLIB_HANDLE DIAMOND_LIBRARY_HANDLE;

#define DIAMOND_SCRIPT_NAME "scripts"
#define DIAMOND_SCRIPT_SUFFIX ACE_DLL_SUFFIX
#define DIAMOND_SCRIPT_PREFIX ACE_DLL_PREFIX
#define DIAMOND_LOAD_LIBRARY(libname)    ACE_OS::dlopen(libname)
#define DIAMOND_CLOSE_LIBRARY(hlib)      ACE_OS::dlclose(hlib)
#define DIAMOND_GET_PROC_ADDR(hlib,name) ACE_OS::dlsym(hlib,name)

#define DIAMOND_PATH_MAX PATH_MAX                            // ace/os_include/os_limits.h -> ace/Basic_Types.h

#if PLATFORM == PLATFORM_WINDOWS
#  define DIAMOND_EXPORT __declspec(dllexport)
#  define DIAMOND_IMPORT __cdecl
#else //PLATFORM != PLATFORM_WINDOWS
#  define DIAMOND_EXPORT export
#  if defined(__APPLE_CC__) && defined(BIG_ENDIAN)
#    define DIAMOND_IMPORT __attribute__ ((longcall))
#  elif defined(__x86_64__)
#    define DIAMOND_IMPORT
#  else
#    define DIAMOND_IMPORT __attribute__ ((cdecl))
#  endif //__APPLE_CC__ && BIG_ENDIAN
#endif //PLATFORM

#if PLATFORM == PLATFORM_WINDOWS
#  ifdef DIAMOND_WIN32_DLL_IMPORT
#    define DIAMOND_DLL_DECL __declspec(dllimport)
#  else //!DIAMOND_WIN32_DLL_IMPORT
#    ifdef DIAMOND_WIND_DLL_EXPORT
#      define DIAMOND_DLL_DECL __declspec(dllexport)
#    else //!DIAMOND_WIND_DLL_EXPORT
#      define DIAMOND_DLL_DECL
#    endif //DIAMOND_WIND_DLL_EXPORT
#  endif //DIAMOND_WIN32_DLL_IMPORT
#else //PLATFORM != PLATFORM_WINDOWS
#  define DIAMOND_DLL_DECL
#endif //PLATFORM

#if PLATFORM == PLATFORM_WINDOWS
#  define DIAMOND_DLL_SPEC __declspec(dllexport)
#  ifndef DECLSPEC_NORETURN
#    define DECLSPEC_NORETURN __declspec(noreturn)
#  endif //DECLSPEC_NORETURN
#else //PLATFORM != PLATFORM_WINDOWS
#  define DIAMOND_DLL_SPEC
#  define DECLSPEC_NORETURN
#endif //PLATFORM

#if !defined(DEBUG)
#  define DIAMOND_INLINE inline
#else //DEBUG
#  if !defined(DIAMOND_DEBUG)
#    define DIAMOND_DEBUG
#  endif //DIAMOND_DEBUG
#  define DIAMOND_INLINE
#endif //!DEBUG

#if COMPILER == COMPILER_GNU
#  define ATTR_NORETURN __attribute__((noreturn))
#  define ATTR_PRINTF(F,V) __attribute__ ((format (printf, F, V)))
#else //COMPILER != COMPILER_GNU
#  define ATTR_NORETURN
#  define ATTR_PRINTF(F,V)
#endif //COMPILER == COMPILER_GNU

typedef ACE_INT64 int64;
typedef ACE_INT32 int32;
typedef ACE_INT16 int16;
typedef ACE_INT8 int8;
typedef ACE_UINT64 uint64;
typedef ACE_UINT32 uint32;
typedef ACE_UINT16 uint16;
typedef ACE_UINT8 uint8;

#if COMPILER != COMPILER_MICROSOFT
typedef uint16      WORD;
typedef uint32      DWORD;
#endif //COMPILER

typedef uint64 OBJECT_HANDLE;

#endif //DIAMOND_DEFINE_H
