#include <iostream>
#include <cstdlib>
#include <ctime>

int main() 
{
    std::srand(static_cast<unsigned int>(std::time(nullptr)));
    int secretNumber = std::rand() % 100 + 1;
    int guess = 0;
    int tries = 0;

    std::cout << "Welcome to the Number Guessing Game!\n";
    std::cout << "I have picked a number between 1 and 100.\n\n";

    while (guess != secretNumber) 
    {
        std::cout << "Enter your guess: ";
        if (!(std::cin >> guess)) 
        {
            std::cout << "Invalid input. Please enter a number.\n";
            std::cin.clear();
            std::cin.ignore(10000, '\n');
            continue;
        }

        tries++;

        if (guess > secretNumber) 
        {
            std::cout << "Too high! Try again.\n\n";
        } 
        else if (guess < secretNumber) 
        {
            std::cout << "Too low! Try again.\n\n";
        } 
        else 
        {
            std::cout << "\nCongratulations! You guessed the number in " << tries << " tries.\n";
        }
    }

    std::cout << "Press Enter to exit...";
    std::cin.ignore(10000, '\n');
    std::cin.get();

    return 0;
}
