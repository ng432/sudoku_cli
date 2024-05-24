#include "sudoku.hpp"

#include <fstream>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

//------------------------------------------------------------------------------------------

void clearScreen()
{
    #ifdef _WIN32
        system("cls");
    #else
        system("clear");
    #endif
}

//------------------------------------------------------------------------------------------

bool Sudoku::readFromCsv(std::ifstream & file, const char delim)
{
    std::string line;

    for (int rowsRead = 0; rowsRead < SudokuWidth; ++rowsRead)
    {
        if (!getline(file, line))
        {
            std::cerr << "Error opening file" << std::endl;
            return false;
        }

        std::istringstream lineStream(line);
        std::string numAsString;
        auto columnsRead = 0;

        while (std::getline(lineStream, numAsString, delim) && columnsRead < SudokuWidth)
        {
            auto num = NoValue;
            try
            {
                num = std::stoi(numAsString);
            }
            catch (...)
            {
                std::cerr << "Failed to convert cell into integer." << std::endl;
                return false;
            }

            if ((num < NoValue) || (MaxValue < num))
            {
                std::cerr << "Number is out of range." << std::endl;
                return false;
            }

            const auto state = (num == NoValue) ? CellState::ToFill : CellState::Fixed;

            grid[rowsRead][columnsRead++] = Cell{ num, state };
        }

        if ((columnsRead != SudokuWidth) || !lineStream.eof())
        {
            std::cerr << "Error opening file; not exactly 9 columns read." << std::endl;
            return false;
        }
    }

    if (!file.eof())
    {
        std::cerr << "Error opening file; more than 9 rows in CSV." << std::endl;
        return false;
    }

    return true;
}

//------------------------------------------------------------------------------------------

bool Sudoku::isValid() const
{
    for (int i = 0; i < SudokuWidth; i++)
    {
        const int boxColIndex = (i * BoxWidth) % SudokuWidth;
        const int boxRowIndex = (i / BoxWidth) * BoxWidth;

        if (!isRowValid(i) || !isColumnValid(i) || !isBoxValid(boxRowIndex, boxColIndex))
        {
            return false;
        }
    }
    return true;
}

//------------------------------------------------------------------------------------------

bool Sudoku::isFull() const
{
    for (int rowIndex = 0; rowIndex < SudokuWidth; ++rowIndex)
    {
        for (int colIndex = 0; colIndex < SudokuWidth; ++colIndex)
        {
            if (getNumber(rowIndex, colIndex) == NoValue)
            {
                return false;
            }
        }
    }
    return true;
}

//------------------------------------------------------------------------------------------

bool Sudoku::isDone() const
{
    return (isFull() && isValid());
}

//------------------------------------------------------------------------------------------

bool Sudoku::solve()
{
    clearAnswers();
    return solveRecursive(Sudoku::Coord(0, 0));
}

//------------------------------------------------------------------------------------------

void Sudoku::setSpacePadding(int sp)
{
    if ((sp < PaddingLowerLimit) || (PaddingUpperLimit < sp))
    {
        std::cout << "Space padding must be between " << PaddingLowerLimit << " and " << PaddingUpperLimit << ", inclusive. \n";
        return;
    }
    spacer = std::string(sp, ' ');
}

//------------------------------------------------------------------------------------------

bool Sudoku::setNumber(int rowIndex, int colIndex, int value)
{
    if ((value < NoValue) || (MaxValue < value) ||
        (rowIndex < MinCoord) || (MaxCoord < rowIndex)  ||
        (colIndex < MinCoord) || (MaxCoord < colIndex) ||
        (getCellStatus(rowIndex, colIndex) == CellState::Fixed))
    {
        return false;
    }

    grid[rowIndex][colIndex].value = value;
    return true;

}

//------------------------------------------------------------------------------------------

void Sudoku::clearAnswers()
{
    for (auto& row : grid)
    {
        for (auto& cell : row)
        {
            if (cell.state == CellState::ToFill)
            {
                cell.value = NoValue;
            }
        }
    }
}

//------------------------------------------------------------------------------------------

std::ostream& operator<<(std::ostream& os, Sudoku& sud)
{
    std::string rowSeparator;
    std::string rowBetweenCoordSudoku;
    std::string rowOfNumbers;

    rowSeparator = sud.formatRowSeparator('-');
    rowBetweenCoordSudoku = sud.formatRowSeparator(sud.coordSudokuSeparator);

    os << sud.formatCoordRow() << "\n";

    // Displaying vertical spacer between coords and sudoku
    for (int j = 0; j < sud.spacer.length(); ++j)
    {
        os << sud.formatRowSeparator(' ') << "\n";
    }

    // Displaying sudoku
    for (int i = 0; i < sud.SudokuWidth; ++i)
    {
        rowOfNumbers = sud.formatRow(i);

        // Have a double separator for boxes
        if ((i % sud.BoxWidth) == 0)
        {
            os << rowSeparator << "\n";
        }

        os << rowOfNumbers << "\n" << rowSeparator << "\n";
    }
    os << "\n";
    return os;
}

//------------------------------------------------------------------------------------------

bool Sudoku::Coord::nextCell()
{
    if ((x == MaxCoord) && (y == MaxCoord))
    {
        return true;
    }

    if (++x > MaxCoord)
    {
        x = MinCoord;
        ++y;
    }

    return false;
}

//------------------------------------------------------------------------------------------

std::string Sudoku::formatRow(int rowIndex) const
{
    std::ostringstream formattedRow;

    // To display coords
    formattedRow << rowIndex << spacer << coordSudokuSeparator;

    for (int colIndex = 0; colIndex < SudokuWidth; ++colIndex)
    {
        std::string colour;

        if ((colIndex % BoxWidth) == 0)
        {
            // Have a double separator for boxes
            formattedRow << verticalLine;
        }

        if (getCellStatus(rowIndex, colIndex) == CellState::Fixed)
        {
            // Displaying starting digits as bold
            colour = "\033[1m";
        }
        else if (getNumber(rowIndex, colIndex) == NoValue)
        {
            // Displaying 0's as red
            colour = "\033[31m";
        }
        else
        {
            // Displaying added digits as green
            colour = "\033[32m";
        }

        formattedRow << spacer << colour << getNumber(rowIndex, colIndex) << "\033[0m" << spacer << verticalLine;
    }
    return formattedRow.str();
}


//------------------------------------------------------------------------------------------

std::string Sudoku::formatRowSeparator(char repeat) const
{
    const int width = (1 + digitsPerCell + (2 * spacer.size())) * SudokuWidth + (SudokuWidth / BoxWidth);

    std::ostringstream formattedRow;

    formattedRow << " " << spacer << coordSudokuSeparator;

    for (int i = 0; i < width; ++i)
    {
        formattedRow << repeat;
    }

    return formattedRow.str();
}

//------------------------------------------------------------------------------------------

std::string Sudoku::formatCoordRow() const
{
    std::ostringstream formattedCoordRow;

    formattedCoordRow << " " << spacer << coordSudokuSeparator;

    for (int colIndex = 0; colIndex < SudokuWidth; ++colIndex)
    {
        if ((colIndex % BoxWidth) == 0)
        {
            // Have a double separator for boxes
            formattedCoordRow << ' ';
        }
        formattedCoordRow << spacer << colIndex << spacer << ' ';
    }

    return formattedCoordRow.str();
}

//------------------------------------------------------------------------------------------

bool Sudoku::isSequenceValid(const std::vector<int>& sequence) const
{
    //TODO: add check to make sure sequence is 9 long
    std::vector<int> checkedNums(SudokuWidth);

    for (int i = 0; i < SudokuWidth; ++i)
    {
        const int currentCell = sequence[i];

        if ((currentCell != NoValue) && (checkedNums[currentCell - 1] == 1))
        {
            return false;
        }
        else if (currentCell != NoValue)
        {
            checkedNums[currentCell - 1]++;
        }
    }
    return true;
}

//------------------------------------------------------------------------------------------

bool Sudoku::isRowValid(int rowIndex) const
{
    std::vector<int> sequence(SudokuWidth);

    for (int colIndex = 0; colIndex < SudokuWidth; ++colIndex)
    {
        sequence[colIndex] = getNumber(rowIndex, colIndex);
    }
    return isSequenceValid(sequence);
}

//------------------------------------------------------------------------------------------

bool Sudoku::isColumnValid(int colIndex) const
{
    std::vector<int> sequence(SudokuWidth);

    for (int rowIndex = 0; rowIndex < SudokuWidth; ++rowIndex)
    {
        sequence[rowIndex] = getNumber(rowIndex, colIndex);
    }
    return isSequenceValid(sequence);
}

//------------------------------------------------------------------------------------------

bool Sudoku::isBoxValid(int rowIndex, int colIndex) const
{
    std::vector<int> sequence(SudokuWidth);

    for (int cellsChecked = 0; cellsChecked < SudokuWidth; ++cellsChecked)
    {
        const int withinBoxColIndex = cellsChecked % BoxWidth;
        const int withinBoxRowIndex = cellsChecked / BoxWidth;

        sequence[cellsChecked] = getNumber(rowIndex + withinBoxRowIndex, colIndex + withinBoxColIndex);
    }
    return isSequenceValid(sequence);
}

//------------------------------------------------------------------------------------------

bool Sudoku::solveRecursive(Coord currentCoord)
{
    currentCoord = nextValidCell(currentCoord);

    for (int cellValue = MinValue; cellValue <= MaxValue; ++cellValue)
    {
        setNumber(currentCoord, cellValue);

        if (displaySolver)
        {
            clearScreen();
            std::cout << *this << "\n";
        }

        if (isDone() || (isValid() && solveRecursive(currentCoord)))
        {
            return true;
        }
    }

    setNumber(currentCoord, NoValue);
    return false;
}

//------------------------------------------------------------------------------------------

Sudoku::Coord Sudoku::nextValidCell(Coord currentCell) const
{
    while (getNumber(currentCell) != NoValue)
    {
        currentCell.nextCell();
    }
    return currentCell;
}

//------------------------------------------------------------------------------------------

bool Sudoku::setNumber(Coord coord, int value)
{
    return setNumber(coord.y, coord.x, value);
}

//------------------------------------------------------------------------------------------

int Sudoku::getNumber(int rowIndex, int colIndex) const
{
    return grid[rowIndex][colIndex].value;
}

//------------------------------------------------------------------------------------------

int Sudoku::getNumber(Coord coord) const
{
    return getNumber(coord.y, coord.x);
}

//------------------------------------------------------------------------------------------

Sudoku::CellState Sudoku::getCellStatus(int rowIndex, int colIndex) const
{
    return grid[rowIndex][colIndex].state;
}
