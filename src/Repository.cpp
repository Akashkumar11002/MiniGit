#include "Repository.h"
#include "ObjectStore.h"
#include <iostream>
#include <sstream>
#include <fstream>
#include <map>
#include <filesystem>
#include <vector>


namespace fs = std::filesystem;

const std::string Repository::MINIGIT_DIR = ".minigit";

void Repository::init() {
    if (fs::exists(MINIGIT_DIR)) {
        std::cout << "Reinitialized existing MiniGit repository in "
                  << fs::current_path().string() << "/" << MINIGIT_DIR << "\n";
        return;
    }

    fs::create_directory(MINIGIT_DIR);
    fs::create_directory(MINIGIT_DIR + "/objects");
    fs::create_directory(MINIGIT_DIR + "/commits");
    fs::create_directory(MINIGIT_DIR + "/staging");

    std::ofstream head(MINIGIT_DIR + "/HEAD");
    head << "";
    head.close();

    std::cout << "Initialized empty MiniGit repository in "
              << fs::current_path().string() << "/" << MINIGIT_DIR << "\n";
}

void Repository::status() {
    const std::string STAGING_INDEX = ".minigit/staging/index";

    std::map<std::string, std::string> staged;
    std::ifstream indexIn(STAGING_INDEX);
    std::string line;
    while (std::getline(indexIn, line)) {
        std::stringstream ss(line);
        std::string fname, fhash;
        ss >> fname >> fhash;
        if (!fname.empty()) staged[fname] = fhash;
    }
    indexIn.close();

    std::vector<std::string> modified;
    std::vector<std::string> unchanged;

    for (const auto& [fname, oldHash] : staged) {
        if (!fs::exists(fname)) {
            modified.push_back(fname + " (deleted)");
            continue;
        }
        std::ifstream in(fname);
        std::stringstream buffer;
        buffer << in.rdbuf();
        std::string currentHash = ObjectStore::hashContent(buffer.str());

        if (currentHash == oldHash) {
            unchanged.push_back(fname);
        } else {
            modified.push_back(fname);
        }
    }

    std::vector<std::string> newFiles;
    for (const auto& entry : fs::recursive_directory_iterator(".")) {
        if (entry.is_regular_file()) {
            std::string path = entry.path().lexically_normal().string();
            if (path.substr(0, 2) == "./") path = path.substr(2);

            if (path.find(".minigit/") == 0 || path.find("build/") == 0 || path.find(".git/") == 0)
                continue;

            if (staged.find(path) == staged.end()) {
                newFiles.push_back(path);
            }
        }
    }

    std::cout << "Modified files:\n";
    for (const auto& f : modified) std::cout << "  " << f << "\n";
    if (modified.empty()) std::cout << "  (none)\n";

    std::cout << "New files:\n";
    for (const auto& f : newFiles) std::cout << "  " << f << "\n";
    if (newFiles.empty()) std::cout << "  (none)\n";

    std::cout << "Unchanged files:\n";
    for (const auto& f : unchanged) std::cout << "  " << f << "\n";
    if (unchanged.empty()) std::cout << "  (none)\n";
}

void Repository::commit(const std::string& message) {
    std::cout << "commit called with message: " << message << " (not implemented yet)\n";
}

void Repository::log() {
    std::cout << "log called (not implemented yet)\n";
}