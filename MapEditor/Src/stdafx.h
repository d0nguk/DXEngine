#pragma once

#include <Windows.h>
#include <crtdbg.h>

#ifdef _DEBUG
#define LEAKTEST TRUE
#define new new(_NORMAL_BLOCK, __FILE__, __LINE__)
#endif
