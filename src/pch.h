#pragma once
#ifndef PCH_H
#define PCH_H

#if (defined(_M_IX86) || defined(__i386__) || __WORDSIZE == 32) || (defined(MEM_FORCE_86) && !defined(MEM_FORCE_64))
#define MEM_86
#elif (defined(_M_X64) || defined(__LP64__) || defined(_LP64) || __WORDSIZE == 64) || defined(MEM_FORCE_64)
#define MEM_64
#endif

#include <iostream>
#include <intrin.h>
#include <Windows.h>
#include <d3d11.h>
#include <D3DX11.h>
#include <minhook/Minhook.h>
#include <imgui/imgui.h>
#include <imgui/imgui_impl_win32.h>
#include <imgui/imgui_impl_dx11.h>



#endif