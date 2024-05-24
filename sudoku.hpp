#pragma once

#include <fstream>
#include <iostream>
#include <optional>
#include <vector>

//------------------------------------------------------------------------------------------

class Sudoku
{
public:
    Sudoku() : grid(SudokuWidth, std::vector<Cell>(SudokuWidth)) {}

    bool readFromCsv(std::ifstream &, char);

    bool isValid() const;
    bool isFull() const;
    bool isDone() const;

    // Returns true if successfully solved sudoku
    bool solve();

    void setSolverDisplay(bool status) { displaySolver = status; }
    void setSpacePadding(int);

    // Returns true if successfully set number
    bool setNumber(int rowIndex, int colIndex, int value);

    void clearAnswers();

    friend std::ostream& operator<<(std::ostream&, Sudoku&);

private:
    enum class CellState
    {
        Fixed,
        ToFill,
    };

    struct Cell
    {
        int value = 0;
        CellState state = CellState::ToFill;
    };

    struct Coord
    {
        Coord(int xx, int yy) : x(xx), y(yy) {}
        bool nextCell();
        int x = 0;
        int y = 0;
    };

    std::string formatRow(int rowIndex) const;
    std::string formatRowSeparator(char) const;
    std::string formatCoordRow() const;

    bool isSequenceValid(const std::vector<int>&) const;
    bool isRowValid(int rowIndex) const;
    bool isColumnValid(int colIndex) const;
    // Checks box with top left coordinates [rowIndex, colIndex]
    bool isBoxValid(int rowIndex, int colIndex) const;
    bool solveRecursive(Coord);

    // Increments cell from left to right, returning true if at the end of the sudoku
    Coord nextValidCell(Coord) const;

    bool setNumber(Coord, int value);
    int getNumber(int rowIndex, int ColIndex) const;
    int getNumber(Coord) const;
    CellState getCellStatus(int rowIndex, int ColIndex) const;

    std::vector<std::vector<Cell>> grid;

    bool displaySolver = false;
    static constexpr auto coordSudokuSeparator = ' ';
    static constexpr char verticalLine = '|';
    std::string spacer = " ";

    static constexpr int SudokuWidth = 9;
    static constexpr int BoxWidth = 3;
    static constexpr int PaddingLowerLimit = 0;
    static constexpr int PaddingUpperLimit = 2;
    static constexpr int digitsPerCell = 1;
    static constexpr int NoValue = 0;
    static constexpr int MinValue = 1;
    static constexpr int MaxValue = 9;
    static constexpr int MinCoord = 0;
    static constexpr int MaxCoord = SudokuWidth - 1;
};
