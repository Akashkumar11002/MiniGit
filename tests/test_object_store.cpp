#include "ObjectStore.h"
#include "Repository.h"
#include "test_support.h"

void testObjectStore() {
    using namespace test_support;

        require(ObjectStore::hashContent("") == "cbf29ce484222325", "empty content hash changed");
    require(ObjectStore::hashContent("same") == ObjectStore::hashContent("same"),
            "hashing must be deterministic");
    require(ObjectStore::hashContent("same") != ObjectStore::hashContent("different"),
            "different content should produce different hashes");

    TemporaryDirectory directory;
    Repository::init();
    const std::string binaryContent("a\0b\xff", 4);
    const std::string hash = ObjectStore::storeObject(binaryContent);
    const fs::path objectPath = fs::path(".minigit/objects") / hash;
    require(fs::exists(objectPath), "stored object should exist");
    require(readFile(objectPath) == binaryContent, "stored object content changed");
    require(ObjectStore::storeObject(binaryContent) == hash, "storing same object changed hash");
}