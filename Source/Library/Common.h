﻿#pragma once

#ifndef  UNICODE
#define UNICODE
#endif // ! UNICODE

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif // ! WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <wincodec.h>
#include <wrl.h>

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "d3dcompiler.lib")

#include <d3d11_4.h>
#include <d3dcompiler.h>
#include <directxcolors.h>

#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>

#include <cassert>
#include <filesystem>
#include <memory>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "Resource.h"
struct DirectionsInput {
	BOOL bFront;
	BOOL bLeft;
	BOOL bBack;
	BOOL bRight;
	BOOL bUp;
	BOOL bDown;
};

struct MouseRelativeMovement
{
	LONG X;
	LONG Y;
};

constexpr LPCWSTR PSZ_COURSE_TITLE = L"Game Graphics Programming";

using namespace Microsoft::WRL;
using namespace DirectX;