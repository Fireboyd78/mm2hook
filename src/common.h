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

// Lua 5.3.2 embedded
#include <lua/lua.hpp>

//  warning C4091: '__declspec(novtable)': ignored on left of 'x' when no variable is declared
#pragma warning (disable : 4091)