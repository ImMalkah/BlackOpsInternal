#pragma once
#include <Windows.h>
#include <cstdio>
#include <cstdarg>

namespace Log
{
    inline void Debug(const char* fmt, ...)
    {
        char buffer[1024];

        va_list args;
        va_start(args, fmt);
        vsnprintf(buffer, sizeof(buffer), fmt, args);
        va_end(args);

        OutputDebugStringA("[BlackOpsInternal] ");
        OutputDebugStringA(buffer);
        OutputDebugStringA("\n");

        std::printf("[BlackOpsInternal] %s\n", buffer);
    }
}
