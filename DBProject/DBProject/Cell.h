#pragma once
#include <iostream>
#include <fstream>
#include <string>
//#include "CellProtocol.h"
class Cell {
protected:
	std::string text;
	bool isNull;
public:
	explicit Cell(const std::string& initial = "NULL");
	virtual ~Cell() = default;
	virtual Cell* clone() = 0;
	virtual void writeToStream(std::ostream& os) const;
	virtual void print(int width) const;
	virtual int getStringLength() const;
	const std::string& raw() const;

};