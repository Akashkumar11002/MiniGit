#include "Staging.h"
#include "ObjectStore.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <filesystem>
#include <map>

namespace fs = std::filesystem;

const std::string Staging::STAGING_INDEX = ".minigit/staging/index";

void Staging::add(const std::string& filename) {
    if (!fs::exists(filename)) {
        std::cerr << "Error: file '" << filename << "' does not exist\n";
        return;
    }

    std::ifstream in(filename);
    std::stringstream buffer;
    buffer << in.rdbuf();
    std::string content = buffer.str();

    std::string hash = ObjectStore::storeObject(content);

    std::map<std::string, std::string> entries;
    std::ifstream indexIn(STAGING_INDEX);
    std::string line;
    while (std::getline(indexIn, line)) {
        std::stringstream ss(line);
        std::string fname, fhash;
        ss >> fname >> fhash;
        if (!fname.empty()) entries[fname] = fhash;
    }
    indexIn.close();

    entries[filename] = hash;

    std::ofstream indexOut(STAGING_INDEX, std::ios::trunc);
    for (const auto& [fname, fhash] : entries) {
        indexOut << fname << " " << fhash << "\n";
    }

    std::cout << "Staged " << filename << " (" << hash << ")\n";
}