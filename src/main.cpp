#include <iostream>
#include "Repository.h"
#include "Staging.h"

void printUsage() {
    std::cout << "Usage: minigit <command> [arguments]\n";
    std::cout << "Commands:\n";
    std::cout << "  init\n";
    std::cout << "  add <file>\n";
    std::cout << "  status\n";
    std::cout << "  commit -m \"message\"\n";
    std::cout << "  log\n";
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        printUsage();
        return 1;
    }

    std::string command = argv[1];

    if (command == "init") {
        Repository::init();
    }
    else if (command == "add") {
        if (argc < 3) {
            std::cerr << "Error: add requires a filename\n";
            return 1;
        }
        Staging::add(argv[2]);
    }
    else if (command == "status") {
        Repository::status();
    }
    else if (command == "commit") {
        if (argc < 4 || std::string(argv[2]) != "-m") {
            std::cerr << "Error: usage is commit -m \"message\"\n";
            return 1;
        }
        Repository::commit(argv[3]);
    }
    else if (command == "log") {
        Repository::log();
    }
    else {
        std::cerr << "Unknown command: " << command << "\n";
        printUsage();
        return 1;
    }

    return 0;
}// test change
// second change
