


#pragma once
#include <string>

class Repository {
public:
    static void init();
    static void status();
    static void commit(const std::string& message);
    static void log();

private:
    static const std::string MINIGIT_DIR;
};