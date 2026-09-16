#pragma once

#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>

namespace test_support {

namespace fs = std::filesystem;

class TemporaryDirectory {
public:
    TemporaryDirectory()
        : path(fs::temp_directory_path() / fs::path("minigit-test-" + std::to_string(++counter))) {
        fs::create_directories(path);
        fs::current_path(path);
    }

    ~TemporaryDirectory() {
        fs::current_path(originalPath);
        fs::remove_all(path);
    }

    TemporaryDirectory(const TemporaryDirectory&) = delete;
    TemporaryDirectory& operator=(const TemporaryDirectory&) = delete;

private:
    inline static unsigned int counter = 0;
    const fs::path originalPath = fs::current_path();
    const fs::path path;
};

inline void require(bool condition, const std::string& message) {
    if (!condition) {
        throw std::runtime_error(message);
    }
}

inline std::string readFile(const fs::path& path) {
    std::ifstream input(path, std::ios::binary);
    std::ostringstream content;
    content << input.rdbuf();
    return content.str();
}

template <typename Function>
std::string captureOutput(Function function, std::ostream& stream) {
    std::ostringstream captured;
    std::streambuf* original = stream.rdbuf(captured.rdbuf());
    function();
    stream.rdbuf(original);
    return captured.str();
}

template <typename Function>
std::string captureStdout(Function function) {
    return captureOutput(function, std::cout);
}

template <typename Function>
std::string captureStderr(Function function) {
    return captureOutput(function, std::cerr);
}

inline void writeFile(const fs::path& path, const std::string& content) {
    std::ofstream output(path, std::ios::binary);
    output << content;
}

}