#include <Debug.h>
#include <cstdarg>
#include <cstring>
#include <iostream>
#include <3rdparty/Nanoprintf.h>

namespace Baltar
{
    void Log(const char* msg, LogLevel level, ...)
    {
        va_list argsList;
        va_start(argsList, level);
        const size_t buffLen = npf_vsnprintf(nullptr, 0, msg, argsList) + 1;
        va_end(argsList);

        //TODO: better solution than VLA?
        char buffer[buffLen];
        std::memset(buffer, 0, buffLen);

        va_start(argsList, level);
        npf_vsnprintf(buffer, buffLen, msg, argsList);
        va_end(argsList);

        std::cout << buffer << "\r\n";
        //TOOD: handle fatal level
        if (level == LogLevel::Fatal)
            std::abort();
    }
}
