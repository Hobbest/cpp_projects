#pragma once

#include "condition_parser.h"
#include "database.h"
#include "test_runner.h"

void TestParseCondition()
{
    {
	std::istringstream is("date != 2017-11-18");
	std::shared_ptr<Node> root = ParseCondition(is);
        Assert(root->Evaluate({ 2017, 1, 1 }, ""), "Parse condition 1");
        Assert(!root->Evaluate({ 2017, 11, 18 }, ""), "Parse condition 2");
    }
    {
	std::istringstream is(R"(event == "sport event")");
	std::shared_ptr<Node> root = ParseCondition(is);
        Assert(root->Evaluate({ 2017, 1, 1 }, "sport event"), "Parse condition 3");
        Assert(!root->Evaluate({ 2017, 1, 1 }, "holiday"), "Parse condition 4");
    }
    {
	std::istringstream is("date >= 2017-01-01 AND date < 2017-07-01");
	std::shared_ptr<Node> root = ParseCondition(is);
        Assert(root->Evaluate({ 2017, 1, 1 }, ""), "Parse condition 5");
        Assert(root->Evaluate({ 2017, 3, 1 }, ""), "Parse condition 6");
        Assert(root->Evaluate({ 2017, 6, 30 }, ""), "Parse condition 7");
        Assert(!root->Evaluate({ 2017, 7, 1 }, ""), "Parse condition 8");
        Assert(!root->Evaluate({ 2016, 12, 31 }, ""), "Parse condition 9");
    }
    {
	std::istringstream is(R"(event != "sport event" AND event != "Wednesday")");
	std::shared_ptr<Node> root = ParseCondition(is);
        Assert(root->Evaluate({ 2017, 1, 1 }, "holiday"), "Parse condition 10");
        Assert(!root->Evaluate({ 2017, 1, 1 }, "sport event"), "Parse condition 11");
        Assert(!root->Evaluate({ 2017, 1, 1 }, "Wednesday"), "Parse condition 12");
    }
    {
	std::istringstream is(R"(event == "holiday AND date == 2017-11-18")");
	std::shared_ptr<Node> root = ParseCondition(is);
        Assert(!root->Evaluate({ 2017, 11, 18 }, "holiday"), "Parse condition 13");
        Assert(!root->Evaluate({ 2017, 11, 18 }, "work day"), "Parse condition 14");
        Assert(root->Evaluate({ 1, 1, 1 }, "holiday AND date == 2017-11-18"), "Parse condition 15");
    }
    {
	std::istringstream is(R"(((event == "holiday" AND date == 2017-01-01)))");
	std::shared_ptr<Node> root = ParseCondition(is);
        Assert(root->Evaluate({ 2017, 1, 1 }, "holiday"), "Parse condition 16");
        Assert(!root->Evaluate({ 2017, 1, 2 }, "holiday"), "Parse condition 17");
    }
    {
	std::istringstream is(R"(date > 2017-01-01 AND (event == "holiday" OR date < 2017-07-01))");
	std::shared_ptr<Node> root = ParseCondition(is);
        Assert(!root->Evaluate({ 2016, 1, 1 }, "holiday"), "Parse condition 18");
        Assert(root->Evaluate({ 2017, 1, 2 }, "holiday"), "Parse condition 19");
        Assert(root->Evaluate({ 2017, 1, 2 }, "workday"), "Parse condition 20");
        Assert(!root->Evaluate({ 2018, 1, 2 }, "workday"), "Parse condition 21");
    }
    {
	std::istringstream is(R"(date > 2017-01-01 AND event == "holiday" OR date < 2017-07-01)");
	std::shared_ptr<Node> root = ParseCondition(is);
        Assert(root->Evaluate({ 2016, 1, 1 }, "event"), "Parse condition 22");
        Assert(root->Evaluate({ 2017, 1, 2 }, "holiday"), "Parse condition 23");
        Assert(root->Evaluate({ 2017, 1, 2 }, "workday"), "Parse condition 24");
        Assert(!root->Evaluate({ 2018, 1, 2 }, "workday"), "Parse condition 25");
    }
    {
	std::istringstream is(R"(((date == 2017-01-01 AND event == "holiday")))");
	std::shared_ptr<Node> root = ParseCondition(is);
        Assert(root->Evaluate({ 2017, 1, 1 }, "holiday"), "Parse condition 26");
        Assert(!root->Evaluate({ 2017, 1, 2 }, "holiday"), "Parse condition 27");
    }
    {
	std::istringstream is(R"(((event == "2017-01-01" OR date > 2016-01-01)))");
	std::shared_ptr<Node> root = ParseCondition(is);
        Assert(root->Evaluate({ 1, 1, 1 }, "2017-01-01"), "Parse condition 28");
        Assert(!root->Evaluate({ 2016, 1, 1 }, "event"), "Parse condition 29");
        Assert(root->Evaluate({ 2016, 1, 2 }, "event"), "Parse condition 30");
    }
}

void TestDatabase()
{
    std::istringstream empty_is("");
    auto empty_condition = ParseCondition(empty_is);
    auto empty_predicate = [empty_condition](const Date& date, const std::string& event) {
        return empty_condition->Evaluate(date, event);
    };

    // Add 2 - Del 1 - Add deleted again
    {
        Database db;
        Date d(2019, 1, 1);
        db.Add(d, "e1");
        db.Add(d, "e2");
        std::istringstream is(R"(event == "e1")");
        auto condition = ParseCondition(is);
        auto predicate = [condition](const Date& date, const std::string& event) {
            return condition->Evaluate(date, event);
        };
        AssertEqual(db.RemoveIf(predicate), 1, "Db Add2-Del-Add 1");
        db.Add(d, "e1");
        AssertEqual(db.FindIf(empty_predicate).size(), 2, "Db Add2-Del-Add 2");
    }

    // Add
    {
        Database db;
        Date d(2019, 1, 1);
        db.Add(d, "e1");
        db.Add(d, "e1");
        std::istringstream is("date == 2019-01-01");
        auto condition = ParseCondition(is);
        auto predicate = [condition](const Date& date, const std::string& event) {
            return condition->Evaluate(date, event);
        };
        AssertEqual(db.FindIf(predicate).size(), 1, "Db Add Duplicates 1");
    }

    // Last
    {
        Database db;
        Date d(2019, 1, 1);
        Date d1(2019, 1, 2);
        Date d2(2018, 12, 22);
        db.Add(d1, "e1");
        db.Add(d2, "e2");
        AssertEqual(db.Last(d), "2018-12-22 e2", "Db Last 1");
        Date d3(2018, 12, 24);
        db.Add(d3, "e3");
        AssertEqual(db.Last(d), "2018-12-24 e3", "Db Last 2");

        // Get last event for date before first event
        try {
            Date d4(2017, 2, 2);
            db.Last(d4);
            Assert(false, "Db Last 3");
        } catch (std::invalid_argument& e) {
            // Pass
        }

        // Delete event and get last
        std::istringstream is("date == 2018-12-24");
        auto condition = ParseCondition(is);
        auto predicate = [condition](const Date& date, const std::string& event) {
            return condition->Evaluate(date, event);
        };
        db.RemoveIf(predicate);
        AssertEqual(db.Last(d), "2018-12-22 e2", "Db Last 4");

        AssertEqual(db.Last(d1), "2019-01-02 e1", "Db Last 5");
        db.Add(d2, "e4");
        AssertEqual(db.Last(d2), "2018-12-22 e4", "Db Last 6");
    }

    // Del
    {
        Database db;
        db.Add({ 2019, 1, 1 }, "e1");
        db.Add({ 2019, 1, 1 }, "e2");
        db.Add({ 2018, 1, 7 }, "e3");
        db.Add({ 2018, 1, 7 }, "e4");
        std::istringstream is("date == 2018-01-07");
        auto condition = ParseCondition(is);
        auto predicate = [condition](const Date& date, const std::string& event) {
            return condition->Evaluate(date, event);
        };
        AssertEqual(db.RemoveIf(predicate), 2, "Db Del 1");
    }
    {
        Database db;
        db.Add({ 2019, 1, 1 }, "e1");
        db.Add({ 2019, 1, 1 }, "e2");
        db.Add({ 2018, 1, 7 }, "e3");
        db.Add({ 2018, 1, 7 }, "e4");
        std::istringstream is("date >= 2018-01-07 AND date <= 2020-01-01");
        auto condition = ParseCondition(is);
        auto predicate = [condition](const Date& date, const std::string& event) {
            return condition->Evaluate(date, event);
        };
        AssertEqual(db.RemoveIf(predicate), 4, "Db Del 2");
    }
    {
        Database db;
        db.Add({ 2019, 1, 1 }, "e1");
        db.Add({ 2019, 1, 1 }, "e2");
        db.Add({ 2018, 1, 7 }, "e3");
        db.Add({ 2018, 1, 7 }, "e4");
        AssertEqual(db.RemoveIf(empty_predicate), 4, "Db Del 3");
    }
    {
        Database db;
        db.Add({ 2019, 1, 1 }, "e1");
        db.Add({ 2019, 1, 1 }, "e2");
        db.Add({ 2018, 1, 7 }, "e3");
        db.Add({ 2018, 1, 7 }, "e4");
        std::istringstream is(R"(event == "e1")");
        auto condition = ParseCondition(is);
        auto predicate = [condition](const Date& date, const std::string& event) {
            return condition->Evaluate(date, event);
        };
        AssertEqual(db.RemoveIf(predicate), 1, "Db Del 4");
    }

    {
        Database db;
        db.Add({ 2019, 1, 1 }, "e1");
        db.Add({ 2019, 1, 1 }, "e2");
        db.Add({ 2018, 1, 7 }, "e3");
        db.Add({ 2018, 1, 7 }, "e4");
        std::istringstream is(R"(event == "e1" OR date == 2019-01-01)");
        auto condition = ParseCondition(is);
        auto predicate = [condition](const Date& date, const std::string& event) {
            return condition->Evaluate(date, event);
        };
        AssertEqual(db.RemoveIf(predicate), 2, "Db Del 5");
    }

    // Find
    {
        Database db;
        db.Add({ 2019, 1, 1 }, "e1");
        db.Add({ 2019, 1, 1 }, "e2");
        db.Add({ 2018, 1, 7 }, "e3");
        db.Add({ 2018, 1, 7 }, "e4");
        std::istringstream is("date == 2018-01-07");
        auto condition = ParseCondition(is);
        auto predicate = [condition](const Date& date, const std::string& event) {
            return condition->Evaluate(date, event);
        };
        AssertEqual(db.FindIf(predicate).size(), 2, "Db Find 1");
    }
    {
        Database db;
        db.Add({ 2019, 1, 1 }, "e1");
        db.Add({ 2019, 1, 1 }, "e2");
        db.Add({ 2018, 1, 7 }, "e3");
        db.Add({ 2018, 1, 7 }, "e4");
        std::istringstream is("date >= 2018-01-07 AND date <= 2020-01-01");
        auto condition = ParseCondition(is);
        auto predicate = [condition](const Date& date, const std::string& event) {
            return condition->Evaluate(date, event);
        };
        AssertEqual(db.FindIf(predicate).size(), 4, "Db Find 2");
    }
    {
        Database db;
        db.Add({ 2019, 1, 1 }, "e1");
        db.Add({ 2019, 1, 1 }, "e2");
        db.Add({ 2018, 1, 7 }, "e3");
        db.Add({ 2018, 1, 7 }, "e4");
        AssertEqual(db.FindIf(empty_predicate).size(), 4, "Db Find 3");
    }
    {
        Database db;
        db.Add({ 2019, 1, 1 }, "e1");
        db.Add({ 2019, 1, 1 }, "e2");
        db.Add({ 2018, 1, 7 }, "e3");
        db.Add({ 2018, 1, 7 }, "e4");
        std::istringstream is(R"(event == "e1")");
        auto condition = ParseCondition(is);
        auto predicate = [condition](const Date& date, const std::string& event) {
            return condition->Evaluate(date, event);
        };
        AssertEqual(db.FindIf(predicate).size(), 1, "Db Find 4");
    }

    {
        Database db;
        db.Add({ 2019, 1, 1 }, "e1");
        db.Add({ 2019, 1, 1 }, "e2");
        db.Add({ 2018, 1, 7 }, "e3");
        db.Add({ 2018, 1, 7 }, "e4");
        std::istringstream is(R"(event == "e1" OR date == 2019-01-01)");
        auto condition = ParseCondition(is);
        auto predicate = [condition](const Date& date, const std::string& event) {
            return condition->Evaluate(date, event);
        };
        AssertEqual(db.FindIf(predicate).size(), 2, "Db Find 5");
    }

    // Add - Del - Add - Del
    {
        Database db;
        db.Add({ 2019, 1, 1 }, "e1");
        db.Add({ 2019, 1, 1 }, "e2");
        AssertEqual(db.RemoveIf(empty_predicate), 2, "Db Add-Del-Add-Del 1");

        db.Add({ 2019, 1, 1 }, "e1");
        db.Add({ 2019, 1, 1 }, "e2");
        AssertEqual(db.RemoveIf(empty_predicate), 2, "Db Add-Del-Add-Del 1");
    }
}
