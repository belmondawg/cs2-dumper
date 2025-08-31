#include "include.hpp"

enum class LogLevel {
    Info,
    Debug,
    Warning,
    Error
};

class CLogger
{
public:
    void ClearConsole()
    {
        system("chcp 65001");
        system("cls");
    }

    void Log(LogLevel level, const char* szFmt, ...) {
        char szBuffer[1024];
        va_list args;
        va_start(args, szFmt);
        vsnprintf(szBuffer, sizeof(szBuffer), szFmt, args);
        va_end(args);

        const char* color = "";
        switch (level) {
            case LogLevel::Info:    color = "\x1b[0m";   break;
            case LogLevel::Debug:   color = "\x1b[90m";  break;
            case LogLevel::Warning: color = "\x1b[93m";  break;
            case LogLevel::Error:   color = "\x1b[91m";  break;
        }

        std::cout << color << "[mobbyn-dumper] " << szBuffer << "\x1b[0m\n";
    }
};

inline std::unique_ptr<CLogger> g_pLogger;