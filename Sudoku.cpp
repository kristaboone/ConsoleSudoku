#include <cassert>
#include <iostream>
#include <iomanip>
#include <map>

#include <Windows.h>

#include "Sudoku.h"

// ====================================================================
Sudoku::Sudoku()
	: m_board(cNUM_ROWS_COLS, SudokuBoardRow(cNUM_ROWS_COLS))
	, m_editable(cNUM_ROWS_COLS, std::bitset<cNUM_ROWS_COLS>()) // defaults to 0s
	, m_boardChecker(cNUM_SETS_TO_CHECK, std::bitset<cNUM_ROWS_COLS>()) // defaults to 0s
	, m_gameState(GameState::eUNSOLVED)
{
}

void Sudoku::Load(const SudokuBoard& startingBoard)
{
	m_gameState = GameState::eLOADING;
	for (size_t r = 0; r < cNUM_ROWS_COLS; ++r)
	{
		for (size_t c = 0; c < cNUM_ROWS_COLS; ++c)
		{
			m_editable.at(r).set(c); // make sure it's editable before we load in
			SetCell(r, c, startingBoard[r][c]);
		}
	}
	m_gameState = GameState::eUNSOLVED;
}

void Sudoku::Clear()
{
	m_board.clear();
	m_editable.clear();
	m_boardChecker.clear();
}

void Sudoku::TrackProgress(const size_t row, const size_t col, const size_t num)
{
	// track progress
	if (num > 0)
	{
		m_boardChecker[GetCheckerRowIndex(row)].set(num - 1);
		m_boardChecker[GetCheckerColumnIndex(col)].set(num - 1);
		m_boardChecker[GetCheckerCellIndex(row, col)].set(num - 1);
	}
	else if (num == 0)
	{
		const size_t currentNumAtSpot = m_board[row][col];
		if (currentNumAtSpot > 0)
		{
			m_boardChecker[GetCheckerRowIndex(row)].reset(currentNumAtSpot - 1);
			m_boardChecker[GetCheckerColumnIndex(col)].reset(currentNumAtSpot - 1);
			m_boardChecker[GetCheckerCellIndex(row, col)].reset(currentNumAtSpot - 1);
		}
	}
	else
	{
		assert(true); // should never reach here, ValidateCellEntry should return false if num < 0
	}

	// see if we've solved anything yet
	bool solved = true;
	for (const auto& setThatNeedsValidated : m_boardChecker)
	{
		solved &= setThatNeedsValidated.all();

		if (!solved)
		{
			break; // early exit
		}
	}

	if (solved)
	{
		m_gameState = GameState::eSOLVED;
	}
}

bool Sudoku::SetCell(const size_t row, const size_t col, const size_t num)
{
	// exit early if there is a problem with the cell values
	if (!ValidateCellEntry(row, col, num))
	{
		return false;
	}

	// error, we can't edit this cell
	if (!CanEditCell(row, col))
	{
		std::cerr << "Number at row " << row << ", column " << col << " is not editable!" << std::endl;
		return false;
	}

	// track that we just set this number
	TrackProgress(row, col, num);

	// set the number at the cell
	m_board[row][col] = num;

	// if we're loading starting data, this cell should be uneditable in the future
	if ((m_gameState == GameState::eLOADING) && (num != 0))
	{
		SetCanEditCell(row, col, false);
	}

	return true;
}

bool Sudoku::Solved() const
{
	return m_gameState == GameState::eSOLVED;
}

void Sudoku::TryWriteSep(std::ostream& out, const size_t rowOrColNum, const std::string& sep, const bool useNewline) const
{
	if (rowOrColNum == 0)
	{
		return;
	}
	else if ((rowOrColNum + 1) == cNUM_ROWS_COLS)
	{
		return;
	}
	else if ((rowOrColNum + 1) % cNUM_ROWS_COLS_IN_CELL == 0)
	{
		if (useNewline)
		{
			out << "\n  ";
		}
		out << sep << " ";
	}
}

void Sudoku::WriteNumber(size_t row, size_t col) const
{
	if (m_board[row][col] == 0)
	{
		std::cout << "  "; // use an empty space for numbers that aren't filled (0)
		return;
	}

	// This is really ugly and will only work on windows, but that'll be fine for this
	if (CanEditCell(row, col))
	{
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 11);
	}
	else
	{
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);
	}

	std::cout << m_board[row][col] << " ";

	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);
}

void Sudoku::Write() const
{
	static const std::string cROW_SEP = "---------------------";
	static const std::string cCOL_SEP = "|";

	std::cout << "\n\n";

	for (size_t r = 0; r < cNUM_ROWS_COLS; ++r)
	{
		std::cout << "  ";
		for (size_t c = 0; c < cNUM_ROWS_COLS; ++c)
		{
			WriteNumber(r, c);
			TryWriteSep(std::cout, c, cCOL_SEP, false);
		}

		TryWriteSep(std::cout, r, cROW_SEP, true);
		std::cout << "\n";
	}
}

bool Sudoku::CanEditCell(const size_t row, const size_t col) const
{
	assert(CheckCellValue(row));
	assert(CheckCellValue(col));
	return m_editable[row][col];
}

void Sudoku::SetCanEditCell(const size_t row, const size_t col, const bool editable)
{
	assert(CheckCellValue(row));
	assert(CheckCellValue(col));
	m_editable[row][col] = editable;
}

bool Sudoku::CheckCellValue(const size_t cellValue) const
{
	return CheckNumInBounds((size_t)0, cNUM_ROWS_COLS, cellValue);
}

bool Sudoku::ValidateCellEntry(const size_t row, const size_t col, const size_t num) const
{
	bool err = false;
	if (!CheckCellValue(row))
	{
		std::cerr << "Bad row value " << row << std::endl;
		err = true;
	}
	if (!CheckCellValue(col))
	{
		std::cerr << "Bad column value " << col << std::endl;
		err = true;
	}
	if (!CheckCellValue(num))
	{
		std::cerr << "Bad number " << num << std::endl;
		err = true;
	}
	return !err;
}

size_t Sudoku::GetCheckerRowIndex(const size_t row) const
{
	// checker bitarrays are stored rows | cols | cells
	return row;
}

size_t Sudoku::GetCheckerColumnIndex(const size_t col) const
{
	// checker bitarrays are stored rows | cols | cells
	return cNUM_ROWS_COLS + col;
}

size_t Sudoku::GetCheckerCellIndex(const size_t row, const size_t col) const
{
	// checker bitarrays are stored rows | cols | cells
	const size_t rowCell = std::floor(row / cNUM_ROWS_COLS_IN_CELL);
	assert(CheckNumInBounds(static_cast<size_t>(0), cNUM_ROWS_COLS_IN_CELL, rowCell));

	const size_t colCell = std::floor(col / cNUM_ROWS_COLS_IN_CELL);
	assert(CheckNumInBounds(static_cast<size_t>(0), cNUM_ROWS_COLS_IN_CELL, colCell));

	return (cNUM_ROWS_COLS * 2) + (rowCell * cNUM_ROWS_COLS_IN_CELL) + colCell;
}
