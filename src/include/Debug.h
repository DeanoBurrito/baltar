#pragma once

#include <stddef.h>

namespace Baltar
{
    enum class LogLevel : size_t
    {
        Fatal = 0,
        Error,
        Warning,
        Info,
        Debug
    };

    [[gnu::format(printf, 1, 3)]]
    void Log(const char* msg, LogLevel level, ...);

    //common error strings
    constexpr const char* StrUninitialized = "Uninitialized";
}

#define ASSERT(cond, msg) \
{ \
    if (!(cond)) \
        Log("ASSERT failed (%s:%u): %s", LogLevel::Fatal, __FILE__, __LINE__, (msg)); \
}

#define ASSERT_UNREACHABLE() \
{ \
    Log("ASSERT_UNREACHABLE reached (%s:%u).", LogLevel::Fatal, __FILE__, __LINE__); \
    __builtin_unreachable(); \
}

#define VERIFY(cond, retval, msg) \
{ \
    if (!(cond)) \
    { \
        Log("VERIFY failed (%s:%u): %s", LogLevel::Error, __FILE__, __LINE__, (msg)); \
        return retval; \
    } \
}
