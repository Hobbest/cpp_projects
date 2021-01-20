#pragma once

#include "date.h"

#include <map>
#include <set>
#include <string>
#include <iostream>
#include <vector>
#include <functional>

using Predicator = std::function<bool(const Date&, const std::string&)>;

class Database {
	std::map< Date, std::set<std::string> > database;
	std::map< Date, std::vector<std::string> > shedule;
public:
    void Add(const Date& date, const std::string& event);

    int RemoveIf(const Predicator& pred);

    std::vector< std::string > FindIf(const Predicator& pred) const;

    void Print(std::ostream& os) const;

    std::string Last(const Date& date) const;
};
