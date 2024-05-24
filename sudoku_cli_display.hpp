#pragma once

#include "sudoku.hpp"

#include <unordered_map>

//------------------------------------------------------------------------------------------

class SudokuCliDisplay
{
public:
    int exec();

    void promptUserDifficulty();

    bool readFileToSudoku();

    void solveSudoku();

    enum class ExitChoice
    {
        Quit,
        Solve,
    };

    ExitChoice playSudoku();

    void clearScreen() const;

    void setSudokuCsvFolder(const std::string& folder) { sudokuCsvFolder = folder; }
    void setFileName(const std::string& file) { fileName = file; }
    void setFileType(const std::string& fileSuffix) { fileType = fileSuffix; }

private:
    enum class Difficulty
    {
        Easy,
        Medium,
        Hard,
        WorldsHardest,
    };

    bool promptUserYesNo(const std::string&) const;

    Sudoku sudoku;
    Difficulty difficulty;

    std::string sudokuCsvFolder = "sudoku_examples/";
    std::string fileName = "easy";
    std::string fileType = ".csv";

    static inline const std::unordered_map<std::string, Difficulty> inputToDifficulty =
    {
        {"x",    Difficulty::Easy},
        {"xx",   Difficulty::Medium},
        {"xxx",  Difficulty::Hard},
        {"xxxx", Difficulty::WorldsHardest}
    };

    static inline const std::unordered_map<Difficulty, std::string> difficultyToFileName =
    {
        {Difficulty::Easy,          "easy"},
        {Difficulty::Medium,        "medium"},
        {Difficulty::Hard,          "hard"},
        {Difficulty::WorldsHardest, "worlds_hardest"}
    };
};
