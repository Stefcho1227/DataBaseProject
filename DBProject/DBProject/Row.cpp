#include "Row.h"

Cell* Row::makeCell(const std::string& literal, CellType expectedType) {
	switch (expectedType) {
		case CellType::Integer:
			return new IntegerCell(literal);
		case CellType::Double:
			return new DoubleCell(literal);
		case CellType::String:
			return new StringCell(literal);
		case CellType::Date:
			return new DateCell(literal);
		default:
			throw std::invalid_argument("Unkown type");
	}
}

Row::Row(const std::vector<std::string>& literals, const std::vector<CellType>& types) {
	if (literals.size() != types.size()) {
		throw std::invalid_argument("Row: size mismatch");
	}
	cells.reserve(literals.size());
	for (size_t i = 0; i < literals.size(); i++) {
		cells.push_back(makeCell(literals[i], types[i]));
	}
}

Row::Row(const Row& other) {
	cells.reserve(other.size());
	try {
		for (size_t i = 0; i < other.cells.size(); i++) {
			cells.push_back(other.cells[i]->clone());
		}
	}
	catch (...) {
		for (size_t i = 0; i < cells.size(); i++) {
			delete cells[i];
		}
		cells.clear();
		throw;
	}
}

Row& Row::operator=(const Row& other) {
	if (this == &other) {
		return *this;
	}
	Row temp(other);
	std::swap(cells, temp.cells);
	return *this;
}

Row::~Row() {
	for (size_t i = 0; i < cells.size(); i++){
		delete cells[i];
	}
	cells.clear();
}

void Row::writeToStream(std::ostream& os) const {
	for (size_t i = 0; i < cells.size(); i++) {
		if (i > 0) {
			os << ", ";
		}
		cells[i]->writeToStream(os);
	}
	os << '\n';
}

void Row::print(const std::vector<int>& columnWidths) const {
	for (size_t i = 0; i < cells.size(); i++) {
		if (i > 0) {
			std::cout << "|";
		}
		cells[i]->print(columnWidths[i]);
	}
	std::cout << '\n';
}

void Row::addNullCell(CellType expected) {
	cells.push_back(makeCell("NULL", expected));
}

Cell* Row::getCell(int index) const {
	if (index >= cells.size()) {
		throw std::invalid_argument("invalid index");
	}
	return cells[index];
}

size_t Row::size() const {
	return cells.size();
}

void Row::setCell(int index, const std::string& literal, CellType expected) {
	if (index >= cells.size()) {
		throw std::invalid_argument("invalid index");
	}
	Cell* newCell = makeCell(literal, expected);
	delete cells[index];
	cells[index] = newCell;
}

