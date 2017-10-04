#pragma once

#include <cstdio>

namespace dgb {
    class Debug {
    public:
        static void Log (const char* format, ...) {
            printf("[Log] ");
            va_list args;
            va_start (args, format);
            vprintf (format, args);
            va_end (args);
            printf("\n");
        }
        
        static void LogWarning (const char* format, ...) {
            printf("[Warning] ");
            va_list args;
            va_start (args, format);
            vprintf (format, args);
            va_end (args);
            printf("\n");
        }
        
        static void LogError (const char* format, ...) {
            printf("[Error] ");
            va_list args;
            va_start (args, format);
            vprintf (format, args);
            va_end (args);
            printf("\n");
        }
    };
}
