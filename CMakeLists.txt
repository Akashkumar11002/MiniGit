cmake_minimum_required(VERSION 3.16)

project(MiniGit VERSION 1.0 LANGUAGES CXX)

include(CTest)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(minigit
    src/main.cpp
    src/Repository.cpp
    src/ObjectStore.cpp
    src/Staging.cpp
)

if(BUILD_TESTING)
    add_executable(minigit_tests
        tests/test_main.cpp
        tests/test_object_store.cpp
        tests/test_staging.cpp
        tests/test_repository.cpp
        src/Repository.cpp
        src/ObjectStore.cpp
        src/Staging.cpp
    )
    target_include_directories(minigit_tests PRIVATE src tests)
    add_test(NAME minigit_tests COMMAND minigit_tests)
endif()