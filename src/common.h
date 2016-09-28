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

#include "util.h"
#include "logfile.h"

#ifdef _LUA_EMBED
  // Lua 5.3.2 embedded
  #include <lua/lua.hpp>
  #include <LuaIntf/LuaIntf.h>
#endif // !_LUA_EMBED

//  C4091: '__declspec(novtable)': ignored on left of 'x' when no variable is declared
#pragma warning (disable : 4091)

/*
    Enable some warnings off by default

    C4710: "'function' : function not inlined"
    C4711: "function 'function' selected for inline expansion"
    C4714: "function 'function' marked as __forceinline not inlined"
*/
#pragma warning (1 : 4711)
#pragma warning (3 : 4710 4714)