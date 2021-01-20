#pragma once

#include "date.h"
#include "token.h"

#include <memory>
#include <string>

class Node {
public:
    virtual bool Evaluate(const Date& date, const std::string& event) const = 0;
};

class EmptyNode: public Node {
public:
	EmptyNode();
	bool Evaluate(const Date& date, const std::string& event) const override;
};

enum class Comparison {
	Less,
	LessOrEqual,
	Greater,
	GreaterOrEqual,
	Equal,
	NotEqual,
};

class DateComparisonNode: public Node {
public:
	const Comparison cmp;
	const Date date;
public:
	DateComparisonNode(const Comparison& cmp, const Date& date);
	bool Evaluate(const Date& date, const std::string& event) const override;
};

class EventComparisonNode: public Node {
public:
    const Comparison cmp;
    const std::string event;
public:
    EventComparisonNode(const Comparison& cmp, const std::string& event);
    bool Evaluate(const Date& date, const std::string& event) const override;
};

enum class LogicalOperation {
	And,
	Or,
};

class LogicalOperationNode: public Node {
public:
	const LogicalOperation op;
	const std::shared_ptr<Node> left, right;
public:
	LogicalOperationNode(const LogicalOperation& log_op,
			const std::shared_ptr<Node> l_node,
			const std::shared_ptr<Node> r_node);
    bool Evaluate(const Date& date, const std::string& event) const override;
};
