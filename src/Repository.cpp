#include "Repository.h"
#include <iostream>
#include <fstream>
#include <filesystem>

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
    std::cout << "status called (not implemented yet)\n";
}

void Repository::commit(const std::string& message) {
    std::cout << "commit called with message: " << message << " (not implemented yet)\n";
}

void Repository::log() {
    std::cout << "log called (not implemented yet)\n";
}