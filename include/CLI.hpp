#pragma once
#include <string>

class CLI
{
public:
    static void clearScreen();

    static void printBanner(const std::string &title);

    static std::string getPassword(const std::string &prompt);

    static std::string getLine(const std::string &prompt);

    static int getMenuChoice(int maxOption);

    static void wait();
};