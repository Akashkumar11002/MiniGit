#include "ObjectStore.h"
#include "Repository.h"
#include "Staging.h"
#include "test_support.h"

void testStaging() {
    using namespace test_support;

    TemporaryDirectory directory;
    Repository::init();

    const std::string missingOutput = captureStderr([] { Staging::add("missing.txt"); });
    require(missingOutput.find("does not exist") != std::string::npos,
            "adding a missing file should report an error");
    require(!fs::exists(".minigit/staging/index"), "missing file should not create an index");

    writeFile("empty.txt", "");
    const std::string emptyOutput = captureStdout([] { Staging::add("empty.txt"); });
    require(emptyOutput.find("Staged empty.txt") != std::string::npos,
            "empty file should be staged");

    writeFile("tracked.txt", "before");
    Staging::add("tracked.txt");
    const std::string firstIndex = readFile(".minigit/staging/index");
    require(firstIndex.find("tracked.txt " + ObjectStore::hashContent("before")) != std::string::npos,
            "staged file should have its original hash");

    writeFile("tracked.txt", "after");
    Staging::add("tracked.txt");
    const std::string updatedIndex = readFile(".minigit/staging/index");
    require(updatedIndex.find("tracked.txt " + ObjectStore::hashContent("after")) != std::string::npos,
            "restaging should replace the previous hash");
    require(updatedIndex.find("tracked.txt ") == updatedIndex.rfind("tracked.txt "),
            "restaging should not duplicate an index entry");
}