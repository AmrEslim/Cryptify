#include "CLI.hpp"
#include <iostream>
#include <limits>


void CLI::clearScreen(){
    system("cls");
}

void CLI::printBanner(const std::string& title) {
    clearScreen();
    std::cout << "========================================\n";
    std::cout << "    " << title << "\n";
    std::cout << "========================================\n\n";
}

std::string CLI::getLine(const std::string& prompt) {
    std::cout << prompt;
    std::string input;
    std::getline(std::cin >> std::ws, input);
    return input;
}

// TODO:
// this only works on windows and has to be implemented differently 

std::string CLI::getPassword(const std::string& prompt) {
    std::cout << prompt;
    std::string password;
    char ch;

    // _getch() reads a char without printing it
    while ((ch = _getwch()) != '\r') { // '\r' is Enter on Windows
        if (ch == '\b') { // Backspace
            if (!password.empty()) {
                std::cout << "\b \b"; // Erase character visually
                password.pop_back();
            }
        } else {
            password += ch;
            std::cout << '*'; // Mask with asterisk
        }
    }
    std::cout << "\n";
    return password;
}



int CLI::getMenuChoice(int maxOption) {
    int choice = 0;
    while (true) {
        std::cout << "\nSelect an option (1-" << maxOption << "): ";
        if (std::cin >> choice) {
            if (choice >= 1 && choice <= maxOption) {
                // Clear the newline left in buffer
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                return choice;
            }
        } else {
            // Fix the "infinite loop" bug if user types text
            std::cin.clear(); // Reset error flags
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // Discard bad input
        }
        std::cout << "Invalid input. Try again.\n";
    }
}

void CLI::wait() {
    std::cout << "\nPress Enter to continue...";
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    std::cin.get();
}