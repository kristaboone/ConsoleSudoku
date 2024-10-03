#pragma once
#include <bitset>
#include <ostream>
#include <vector>

using SudokuBoardRow = std::vector<size_t>;
using SudokuBoard = std::vector<SudokuBoardRow>;

enum class GameState
{
	eLOADING,
	eUNSOLVED,
	eSOLVED
};

class Sudoku
{
public:
	Sudoku();
	void Load(const SudokuBoard& startingBoard);
	void Clear();

	bool SetCell(const size_t row, const size_t col, const size_t num);
	bool Solved() const;

	void Write() const;

private:
	SudokuBoard m_board;
	GameState m_gameState;

	static const size_t cNUM_ROWS_COLS{ 9 };
	static const size_t cNUM_ROWS_COLS_IN_CELL{3};
	static const size_t cNUM_SETS_TO_CHECK{27};

	using BitArray = std::vector<bool>;
	std::vector<std::bitset<cNUM_ROWS_COLS>> m_editable;
	std::vector<std::bitset<cNUM_ROWS_COLS>> m_boardChecker;

	void TrackProgress(const size_t row, const size_t col, const size_t num);

	void TryWriteSep(std::ostream& out, const size_t rowOrColNum, const std::string& sep, const bool useNewline) const;
	void WriteNumber(size_t row, size_t col) const;

	bool CanEditCell(const size_t row, const size_t col) const;
	void SetCanEditCell(const size_t row, const size_t col, const bool editable);

	template<typename t>
	bool CheckNumInBounds(const t min, const t max, const t num) const
	{
		return num >= min && num <= max;
	}
	bool CheckCellValue(const size_t cellValue) const;
	bool ValidateCellEntry(const size_t row, const size_t col, const size_t num) const;

	size_t GetCheckerRowIndex(const size_t row) const;
	size_t GetCheckerColumnIndex(const size_t col) const;
	size_t GetCheckerCellIndex(const size_t row, const size_t col) const;

};
