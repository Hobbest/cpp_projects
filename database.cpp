#include "database.h"

#include <map>
#include <string>
#include <set>
#include <vector>
#include <algorithm>
#include <sstream>
#include <functional>
#include <iostream>
#include <stdexcept>

void Database::Add(const Date& date, const std::string& event) {
    if (database.find(date) == database.end() || 
            database[date].find(event) == database[date].end()) 
    {
        database[date].insert(event);
        shedule[date].push_back(event);
    }
}

int Database::RemoveIf(const Predicator& pred) {
    int removed = 0;

    for (auto date_it = database.begin(); date_it != database.end(); ) {
        const Date date = date_it->first;

	for (auto event_it = date_it->second.begin(); event_it != date_it->second.end(); )
	{
	    const std::string event = *event_it;
	    auto next_event_it = std::next(event_it);
	    if (pred(date, event)) {
                date_it->second.erase(event_it);
	    }
	    event_it = next_event_it;
	}
	
	auto next_date_it = std::next(date_it);
	if (date_it->second.empty()) {
	    database.erase(date_it);
	} 
	date_it = next_date_it;

	auto new_pred = [date, pred] (const std::string& event) {
		return pred(date, event);
	};

	auto bound = std::stable_partition(
	        shedule[date].begin(), shedule[date].end(), new_pred);
	removed += std::distance(shedule[date].begin(), bound);
	shedule[date].erase(shedule[date].begin(), bound);
	if (shedule[date].empty()) {
	    shedule.erase(date);
	}
    }

    return removed;
}

std::vector<std::string> Database::FindIf(const Predicator & pred) const {
    std::vector< std::string > entries;
    for (const auto& [date, events]: shedule) {
        for (const std::string& event: events) {
            if (pred(date, event)) {
                 std::ostringstream os;
                 os << date << " " << event;
                 entries.push_back(os.str());
            }
        }
    }
    return entries;
}

void Database::Print(std::ostream& os) const {
    for (const auto& [date, events]: shedule) {
        for (const std::string& event : events) {
            os << date << " " << event << std::endl;
        }
    }
}

std::string Database::Last(const Date& date) const {
    auto u_bnd = shedule.upper_bound(date);
    if (u_bnd == shedule.begin()) {
	throw std::invalid_argument("");
    }
    --u_bnd;
    const Date last_date = u_bnd->first;
    const std::string last_event = u_bnd->second.back();
    std::ostringstream os;
    os << last_date << " " << last_event;
    return os.str();
}
