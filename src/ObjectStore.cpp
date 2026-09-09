#include "ObjectStore.h"
#include <fstream>
#include <sstream>
#include <iomanip>
#include <filesystem>

namespace fs = std::filesystem;

const std::string ObjectStore::OBJECTS_DIR = ".minigit/objects";

std::string ObjectStore::hashContent(const std::string& content) {
    uint64_t hash = 14695981039346656037ULL;
    const uint64_t prime = 1099511628211ULL;

    for (unsigned char c : content) {
        hash ^= c;
        hash *= prime;
    }

    std::stringstream ss;
    ss << std::hex << std::setw(16) << std::setfill('0') << hash;
    return ss.str();
}

std::string ObjectStore::storeObject(const std::string& content) {
    std::string hash = hashContent(content);
    std::string objectPath = OBJECTS_DIR + "/" + hash;

    if (!fs::exists(objectPath)) {
        std::ofstream out(objectPath);
        out << content;
    }

    return hash;
}