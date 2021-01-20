#include "date.h"

#include <exception>
#include <iomanip>
#include <string>

Date::Date(int y, int m, int d) {
    if (m < 1 || m > 12) {
        throw std::invalid_argument("Month value is invalid: " + std::to_string(m));
    } else if (d < 1 || d > 31) {
        throw std::invalid_argument("Day value is invalid: " + std::to_string(d));
    }
    year = y;
    month = m;
    day = d;
}

int Date::Year() const {
    return year;
}

int Date::Month() const {
    return month;
}

int Date::Day() const {
    return day;
}

bool operator<(const Date& lhs, const Date& rhs) {
    if (lhs.Year() == rhs.Year()) {
        if (lhs.Month() == rhs.Month()) {
            return lhs.Day() < rhs.Day();
        }
        return lhs.Month() < rhs.Month();
    }
    return lhs.Year() < rhs.Year();
}

bool operator>=(const Date& lhs, const Date& rhs) {
    return (lhs < rhs) == false;
}

bool operator>(const Date& lhs, const Date& rhs) {
    if (lhs.Year() == rhs.Year()) {
        if (lhs.Month() == rhs.Month()) {
            return lhs.Day() > rhs.Day();
        }
        return lhs.Month() > rhs.Month();
    }
    return lhs.Year() > rhs.Year();
}

bool operator<=(const Date& lhs, const Date& rhs) {
    return (lhs > rhs) == false;
}

bool operator==(const Date& lhs, const Date& rhs) {
    return (lhs.Year() == rhs.Year())
            & (lhs.Month() == rhs.Month())
            & (lhs.Day() == rhs.Day());
}

bool operator!=(const Date& lhs, const Date& rhs) {
    return (lhs == rhs) == false;
}

std::ostream& operator<<(std::ostream& os, const Date& date) {
    os << std::setw(4) << std::setfill('0') << date.Year() << '-'
       << std::setw(2) << std::setfill('0') << date.Month() << '-'
       << std::setw(2) << std::setfill('0') << date.Day();
    return os;
}

Date ParseDate(std::istream& is) {
	std::string s;
	is >> s;
	std::istringstream iss(s);
	int y, m, d;
	char delim1, delim2;
	iss >> y >> delim1 >> m >> delim2 >> d;
	if (is && delim1 == '-' && delim2 == '-') {
	    return Date(y, m, d);
	} else {
	    throw std::invalid_argument("Wrong date format: " + s);
	}
}
