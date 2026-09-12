#include "Repository.h"
#include "ObjectStore.h"
#include <iostream>
#include <sstream>
#include <fstream>
#include <map>
#include <filesystem>
#include <vector>
#include <ctime>
#include <algorithm>



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
    const std::string STAGING_INDEX = ".minigit/staging/index";
    const std::string COMMITS_DIR = ".minigit/commits";
    const std::string HEAD_FILE = ".minigit/HEAD";

    std::ifstream indexIn(STAGING_INDEX);
    std::stringstream stagedContent;
    stagedContent << indexIn.rdbuf();
    indexIn.close();

    if (stagedContent.str().empty()) {
        std::cout << "Nothing to commit. Use 'minigit add <file>' first.\n";
        return;
    }

    std::time_t now = std::time(nullptr);
    std::string timestampStr = std::ctime(&now);
    if (!timestampStr.empty() && timestampStr.back() == '\n')
        timestampStr.pop_back();

    std::string commitInput = stagedContent.str() + timestampStr + message;
    std::string commitId = ObjectStore::hashContent(commitInput);

    std::ofstream commitOut(COMMITS_DIR + "/" + commitId);
    commitOut << "commit " << commitId << "\n";
    commitOut << "message " << message << "\n";
    commitOut << "timestamp " << timestampStr << "\n";
    commitOut << "files\n";
    commitOut << stagedContent.str();
    commitOut.close();

    std::ofstream headOut(HEAD_FILE, std::ios::trunc);
    headOut << commitId << "\n";
    headOut.close();

    std::ofstream clearIndex(STAGING_INDEX, std::ios::trunc);
    clearIndex.close();

    std::cout << "Committed as " << commitId << "\n";
    std::cout << message << "\n";
}

void Repository::log() {
    const std::string COMMITS_DIR = ".minigit/commits";

    if (!fs::exists(COMMITS_DIR) || fs::is_empty(COMMITS_DIR)) {
        std::cout << "No commits yet.\n";
        return;
    }

    // Collect all commit files along with their last-modified time
    std::vector<std::pair<fs::file_time_type, std::string>> commitFiles;
    for (const auto& entry : fs::directory_iterator(COMMITS_DIR)) {
        if (entry.is_regular_file()) {
            commitFiles.push_back({entry.last_write_time(), entry.path().string()});
        }
    }

    // Sort newest first
    std::sort(commitFiles.begin(), commitFiles.end(),
              [](const auto& a, const auto& b) { return a.first > b.first; });

    // Print each commit's details
    for (const auto& [time, path] : commitFiles) {
        std::ifstream in(path);
        std::string line;
        std::cout << "----------------------------------\n";
        while (std::getline(in, line)) {
            if (line.rfind("commit ", 0) == 0 ||
                line.rfind("message ", 0) == 0 ||
                line.rfind("timestamp ", 0) == 0) {
                std::cout << line << "\n";
            }
        }
    }
    std::cout << "----------------------------------\n";
}