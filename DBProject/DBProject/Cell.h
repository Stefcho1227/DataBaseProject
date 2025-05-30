#pragma once
#include <iostream>
#include <fstream>
#include <string>
class Cell {
protected:
	bool isNull;
public:
	explicit Cell(bool isNull = false);
	virtual ~Cell() = default;
	virtual Cell* clone() const = 0;
	virtual std::string toString() const = 0;
	virtual void writeToStream(std::ostream& os) const;
	virtual void print(int width) const;
	virtual int getStringLength() const;
	virtual bool isNullValue() const;
};