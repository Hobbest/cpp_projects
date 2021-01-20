#pragma once

#include <iostream>
#include <sstream>

class Date {
	int year, month, day;
public:
	Date() = delete;
	Date(int y, int m, int d);
	int Year() const;
	int Month() const;
	int Day() const;
};

bool operator<(const Date& lhs, const Date& rhs);
bool operator>=(const Date& lhs, const Date& rhs);
bool operator>(const Date& lhs, const Date& rhs);
bool operator<=(const Date& lhs, const Date& rhs);
bool operator==(const Date& lhs, const Date& rhs);
bool operator!=(const Date& lhs, const Date& rhs);

std::ostream& operator<<(std::ostream& os, const Date& date);

Date ParseDate(std::istream& is);
