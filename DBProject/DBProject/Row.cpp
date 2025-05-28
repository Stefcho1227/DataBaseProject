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

void Row::writeToStream(std::ostream& os) const {
	for (size_t i = 0; i < cells.size(); i++) {
		if (i) {
			os << ", ";
		}
		cells[i]->writeToStream(os);
	}
	os << '\n';
}

void Row::print(const std::vector<int>& w) const {
	for (size_t i = 0; i < cells.size(); i++) {
		if (i) {
			std::cout << "|";
		}
		cells[i]->print(w[i]);
	}
	std::cout << '\n';
}

size_t Row::getSize() const {
	return cells.size();
}

void Row::addNullCell(/*const std::string& literal, */CellType expected) {
	cells.push_back(makeCell("NULL", expected));
}

Cell* Row::getCell(size_t index) const {
	return cells[index];
}

void Row::setCell(size_t index, const std::string& literal, CellType expected) {
	cells[index] = makeCell(literal, expected);
}

