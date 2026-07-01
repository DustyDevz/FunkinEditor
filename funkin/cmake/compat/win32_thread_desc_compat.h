/*
* FunkinC++ Engine
 * Copyright (©) 2026 Dusty. All rights reserved.
 *
 * This project is licensed under the terms of the MIT license.
 * For a copy, see <https://opensource.org/licenses/MIT>.
*/

// Fixes a weird issue I was having
// SetThreadDescription was not declared in this scope; did you mean 'SetThreadInformation'?

#pragma once
#ifdef _WIN32
#include <windows.h>
#if !defined(_MSC_VER)
    #ifndef DILIGENT_HAS_SETTHREADDESCRIPTION_DECL
    #define DILIGENT_HAS_SETTHREADDESCRIPTION_DECL
    extern "C" WINBASEAPI HRESULT WINAPI SetThreadDescription(HANDLE hThread, PCWSTR lpThreadDescription);
#endif
#endif
#endif