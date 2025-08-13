#pragma once
#include <string>
#include <iostream>

#ifdef _WIN64
#include <windows.h>
#include <io.h>
#include <fcntl.h>
#endif

struct IInput 
{
    virtual ~IInput() = default;
    virtual std::string readLine() = 0;
};

struct IOutput
{
    virtual ~IOutput() = default;
    virtual void write(const std::string& s) = 0;
    virtual void writeln(const std::string& s) = 0;
    virtual void clear() = 0;
};

struct ConsoleIO : IInput, IOutput {
    ConsoleIO()
    {
#ifdef _WIN64
        // Turn on UTF-8
        SetConsoleOutputCP(CP_UTF8);
        SetConsoleCP(CP_UTF8);

        // Allow color and learing in ANSI
        HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
        if (hOut != INVALID_HANDLE_VALUE) {
            DWORD mode = 0;
            if (GetConsoleMode(hOut, &mode)) {
                mode |= ENABLE_PROCESSED_OUTPUT | ENABLE_VIRTUAL_TERMINAL_PROCESSING;
                SetConsoleMode(hOut, mode);
            }
        }
        // optional: fast iostream
        std::ios::sync_with_stdio(false);
#endif
    }

    std::string readLine() override
    {
        std::string s;
        std::getline(std::cin, s);
        return s;
    }

    void write(const std::string& s) override { std::cout << s; }
    void writeln(const std::string& s) override { std::cout << s << "\n"; }

    void clear() override
    {
        // ANSI:erase and set cursor to start
        // \x1b[2J - clear, \x1b[H - to pos (1,1)
        std::cout << "\x1b[2J\x1b[H";
        std::cout.flush();
    }
};

struct log : IOutput
{
    void write(const std::string& s) override { std::cout << "\n [log]: " << s; }
};