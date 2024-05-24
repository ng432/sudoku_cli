#include <fstream>
#include <iostream>
#include <sstream>

#include "sudoku.hpp"
#include "sudoku_cli_display.hpp"

//------------------------------------------------------------------------------------------

int SudokuCliDisplay::exec()
{
    clearScreen();

    promptUserDifficulty();

    if (!readFileToSudoku())
    {
        return 1;
    }

    ExitChoice choice = playSudoku();

    if (choice == ExitChoice::Solve)
    {
        solveSudoku();
    }

    return 0;
}

//------------------------------------------------------------------------------------------

void SudokuCliDisplay::clearScreen() const
{
    #ifdef _WIN32
        system("cls");
    #else
        system("clear");
    #endif
}

//------------------------------------------------------------------------------------------

void SudokuCliDisplay::promptUserDifficulty()
{
    while (true)
    {
        std::cout << "What difficulty sudoku would you like to solve? \n"
                  << "x for easy, xx for medium, xxx for hard, xxxx for the world's hardest. \n";
        std::string sudokuChoice;
        std::getline(std::cin, sudokuChoice);

        const auto choiceIt = inputToDifficulty.find(sudokuChoice);
        if (choiceIt != inputToDifficulty.end())
        {
            difficulty = choiceIt->second;
            fileName = difficultyToFileName.at(difficulty);
            return;
        }
        else
        {
            clearScreen();
            std::cout << "Invalid choice. \n";
        }
    }
}

//------------------------------------------------------------------------------------------

bool SudokuCliDisplay::readFileToSudoku()
{
    std::ifstream file{ sudokuCsvFolder + fileName + fileType };

    if (!file.is_open())
    {
        std::cerr << "Error opening file. \n";
        return false;
    }
    if (!sudoku.readFromCsv(file, ','))
    {
        std::cerr << "Error reading file. \n";
        file.close();
        return false;
    }

    file.close();
    return true;
}

//------------------------------------------------------------------------------------------

void SudokuCliDisplay::solveSudoku()
{
    clearScreen();

    std::string solverVisibilityPrompt;
    solverVisibilityPrompt += "Would you like to watch the sudoku be solved? \n";
    solverVisibilityPrompt += "This drastically slows down the algorithm, but it looks cool. \n";
    solverVisibilityPrompt += "Warning: on Windows terminals this results in very jittery output. \n";

    sudoku.setSolverDisplay(promptUserYesNo(solverVisibilityPrompt));

    sudoku.solve();
    clearScreen();
    std::cout << sudoku << "\n";
    std::cout << "Here's the solved sudoku!" << "\n";
}

//------------------------------------------------------------------------------------------

SudokuCliDisplay::ExitChoice SudokuCliDisplay::playSudoku()
{
    std::string input;

    while (true)
    {
        std::cout << "\n" << sudoku << "\n";
        std::cout << "Enter a move (x,y,val), 'quit', or 'solve': \n";
        std::getline(std::cin, input);

        std::istringstream inputstream{ input };

        if (input.empty())
        {
            std::cout << "No input detected, please try again. \n";
        }

        if (input == "quit")
        {
            return ExitChoice::Quit;
        }

        if (input == "solve")
        {
            return ExitChoice::Solve;
        }

        int x{};
        int y{};
        int val{};
        char delim1{};
        char delim2{};

        inputstream >> x >> delim1 >> y >> delim2 >> val;

        if (!inputstream || (delim1 != ',') || (delim2 != ',')
            || !sudoku.setNumber(y, x, val))
        {
            std::cout << "Invalid input or move. Please try again. \n";
        }
        else
        {
            std::cout << "Move accepted. \n";
        }

        if (sudoku.isDone())
        {
            std::cout << "Congratulations! You finished the sudoku. \n";
            return ExitChoice::Quit;
        }
        else if (sudoku.isFull())
        {
            std::string fullButNotValidPrompt = "You have filled the sudoku, but it is not valid. Would you like to restart? \n";

            if (promptUserYesNo(fullButNotValidPrompt))
            {
                sudoku.clearAnswers();
            }
            else
            {
                return ExitChoice::Quit;
            }
        }
    }
}

//------------------------------------------------------------------------------------------

bool SudokuCliDisplay::promptUserYesNo(const std::string& message) const
{
    std::string input;

    while (true)
    {
        std::cout << message << " Enter either 'yes' or 'no': \n";
        std::getline(std::cin, input);

        if (input == "yes")
        {
            return true;
        }
        if (input == "no")
        {
            return false;
        }

        std::cout << "Invalid input. ";
    }
}
