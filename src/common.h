//
// Common resources
//

#pragma once

#define _CRT_NON_CONFORMING_SWPRINTFS
#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdarg.h>
#include <windows.h>
#include <windowsx.h>

#include <ShlObj.h>

/*
    Yeah because it's not like GetClassName would ever be used anywhere...*stares at Microsoft*
*/
#ifdef GetClassName
#define WINUSER_GetClassName GetClassName
#undef GetClassName
#endif

#include "util.h"
#include "logfile.h"
#include "console.h"
#include "settings.h"

#ifdef _LUA_EMBED
// Lua 5.3.2 embedded
# include <lua/lua.hpp>
# ifndef NO_LUAINTF
#  include <LuaIntf/LuaIntf.h>
using namespace LuaIntf;
# endif // !NO_LUAINTF
#endif // !_LUA_EMBED

//  C4091: '__declspec(novtable)': ignored on left of 'x' when no variable is declared
#pragma warning (disable : 4091)
// disable warning/error about __identifier(<string>)
#pragma warning(disable:4483)

// Unsigned type definitions
typedef unsigned char byte;
typedef unsigned short ushort;
typedef unsigned int uint;
typedef unsigned long ulong;
typedef unsigned long long uint64;