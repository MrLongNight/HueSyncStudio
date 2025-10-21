#include <iostream>
#include "core/Logger.h"

int main() {
    Logger::init();
    Logger::get()->info("Welcome to HueSyncStudio!");
    std::cout << "Hello, HueSyncStudio!" << std::endl;
    return 0;
}
