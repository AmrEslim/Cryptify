#include <iostream>

extern "C" {
    int rust_hello();
    int rust_greet(const char* name);

}

int main() {
    std::cout << "c++ calling rust functions:\n";
    // Call the Rust function
    rust_hello();

    //call rust tester function
    rust_greet("C++ is great!!!");

    return 0;

}