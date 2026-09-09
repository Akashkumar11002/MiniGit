#pragma once
#include <string>

class ObjectStore {
public:
    static std::string hashContent(const std::string& content);
    static std::string storeObject(const std::string& content);

private:
    static const std::string OBJECTS_DIR;
};