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

#ifdef _LUA_EMBED
// Lua 5.3.2 embedded
# include <lua/lua.hpp>
#ifndef NO_LUAINTF
# include <LuaIntf/LuaIntf.h>
using namespace LuaIntf;
#endif // !NO_LUAINTF
#endif // !_LUA_EMBED

//  C4091: '__declspec(novtable)': ignored on left of 'x' when no variable is declared
#pragma warning (disable : 4091)

// Unsigned type definitions
typedef std::uint8_t byte;
typedef std::uint16_t ushort;
typedef std::uint32_t uint;
typedef std::uint64_t uint64;