#include <iostream>
#include <string>
#include <sstream>

#include "SudokuBoards.h"

bool StrIsNumber(const std::string& s)
{
	return !s.empty() && std::find_if(s.begin(), s.end(), [](unsigned char c) { return !std::isdigit(c); }) == s.end();
}

bool ReadInt(std::istream& in, size_t& value)
{
	std::string tmp;
	in >> tmp;

	if (StrIsNumber(tmp))
	{
		value = std::stoi(tmp);
		return true;
	}
	return false;
}

bool GetUserGuess(size_t& row, size_t& col, size_t& num)
{
	std::cout << "\n\nGive me the number you'd like to place in the format 'row column number': ";

	std::string tmp;
	std::getline(std::cin, tmp);

	std::stringstream ss(tmp);

	bool success = true;
	if (!ReadInt(ss, row))
	{
		std::cerr << "Bad row value. Try again.\n";
		success = false;
	}
	if (!ReadInt(ss, col))
	{
		std::cerr << "Bad column value. Try again.\n";
		success = false;
	}
	if (!ReadInt(ss, num))
	{
		std::cerr << "Bad number value. Try again.\n";
		success = false;
	}

	return success;
}

void RunGameLoop()
{
	Sudoku sudoku;
	sudoku.Load(cSUDOKU_BOARD_TEST);

	while (!sudoku.Solved())
	{
		system("CLS");
		sudoku.Write();

		size_t row, col, num;
		do
		{
			bool goodInput = false;
			do
			{
				goodInput = GetUserGuess(row, col, num);
			} while (!goodInput);

		} while (!sudoku.SetCell(row - 1, col - 1, num));

		system("CLS");

		std::cout << "Entered " << num << " at (" << row << ", " << col << ")\n\n";
		sudoku.Write();
	}

	system("CLS");
	std::cout << "! c o n g r a t u l a t i o n s !\n        ! y o u  w i n !\n";
	sudoku.Write();
}

int main()
{
	RunGameLoop();
}