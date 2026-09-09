#pragma once
#include <string>

class Staging {
public:
    static void add(const std::string& filename);

private:
    static const std::string STAGING_INDEX;
};