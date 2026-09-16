#include <exception>
#include <iostream>

void testObjectStore();
void testStaging();
void testRepository();

int main() {
    try {
        testObjectStore();
        testStaging();
        testRepository();
        std::cout << "All tests passed\n";
        return 0;
    } catch (const std::exception& error) {
        std::cerr << "Test failure: " << error.what() << "\n";
        return 1;
    }
}