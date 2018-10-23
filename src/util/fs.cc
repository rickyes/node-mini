#include <fstream>
#include <iostream>
#include "fs.h"

std::string fs::readFile(const char* filename) {
    std::string contents;
    std::ifstream inf(filename);
    if (!inf.is_open()) {
        std::cout << "Error opeing file"; 
    }
    char buffer_memory[4096];
    while (!inf.eof()) {
        inf.getline(buffer_memory, 100);
        contents.append(buffer_memory);
        std::cout << buffer_memory << std::endl;
    }
    return contents;
}

void fs::sayHello() {
    printf("hello");
}