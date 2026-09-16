#include "Repository.h"
#include "Staging.h"
#include "test_support.h"

void testRepository() {
    using namespace test_support;

    TemporaryDirectory directory;
    const std::string initOutput = captureStdout([] { Repository::init(); });
    require(fs::is_directory(".minigit/objects"), "init should create objects directory");
    require(fs::is_directory(".minigit/commits"), "init should create commits directory");
    require(fs::is_directory(".minigit/staging"), "init should create staging directory");
    require(readFile(".minigit/HEAD").empty(), "new repository HEAD should be empty");

    const std::string secondInit = captureStdout([] { Repository::init(); });
    require(secondInit.find("Reinitialized existing") != std::string::npos,
            "init should be idempotent");
    require(initOutput.find("Initialized empty MiniGit") != std::string::npos,
            "first init should report initialization");

    const std::string emptyCommit = captureStdout([] { Repository::commit("nothing"); });
    require(emptyCommit.find("Nothing to commit") != std::string::npos,
            "commit without staged files should do nothing");

    writeFile("tracked.txt", "one");
    writeFile("new.txt", "new");
    Staging::add("tracked.txt");
    const std::string initialStatus = captureStdout([] { Repository::status(); });
    require(initialStatus.find("Unchanged files:") != std::string::npos,
            "status should report unchanged staged files");

    writeFile("tracked.txt", "two");
    const std::string modifiedStatus = captureStdout([] { Repository::status(); });
    require(modifiedStatus.find("tracked.txt") != std::string::npos,
            "status should report modified files");
    require(modifiedStatus.find("new.txt") != std::string::npos,
            "status should report new files");

    fs::remove("tracked.txt");
    const std::string deletedStatus = captureStdout([] { Repository::status(); });
    require(deletedStatus.find("tracked.txt (deleted)") != std::string::npos,
            "status should report deleted staged files");

    writeFile("tracked.txt", "one");
    Staging::add("tracked.txt");
    const std::string commitOutput = captureStdout([] { Repository::commit("first commit"); });
    require(commitOutput.find("Committed as ") != std::string::npos,
            "commit should create a commit");
    require(readFile(".minigit/staging/index").empty(), "commit should clear the staging index");
    require(!readFile(".minigit/HEAD").empty(), "commit should update HEAD");

    const std::string logOutput = captureStdout([] { Repository::log(); });
    require(logOutput.find("message first commit") != std::string::npos,
            "log should show commit message");

    fs::remove_all(".minigit/commits");
    const std::string emptyLog = captureStdout([] { Repository::log(); });
    require(emptyLog.find("No commits yet") != std::string::npos,
            "log should handle an empty commit directory");
}