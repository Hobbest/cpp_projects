#include "node.h"

#include <memory>
#include <string>

EmptyNode::EmptyNode() {}

bool EmptyNode::Evaluate(const Date& date, const std::string& event) const {
	return true;
}

DateComparisonNode::DateComparisonNode(const Comparison& cmp_, const Date& date_)
	: cmp(cmp_)
	, date(date_)
{
}

bool DateComparisonNode::Evaluate(const Date& date, const std::string& event) const {
	bool result{};
	switch(cmp) {
	case Comparison::Less:
		result = date < this->date;
		break;
	case Comparison::LessOrEqual:
		result = date <= this->date;
		break;
	case Comparison::Greater:
		result = date > this->date;
		break;
	case Comparison::GreaterOrEqual:
		result = date >= this->date;
		break;
	case Comparison::Equal:
		result = date == this->date;
		break;
	case Comparison::NotEqual:
		result = date != this->date;
                break;
	default:
		break;
	}
	return result;
}

EventComparisonNode::EventComparisonNode(const Comparison& cmp_, const std::string& event_)
	: cmp(cmp_)
	, event(event_)
{
}

bool EventComparisonNode::Evaluate(const Date& date, const std::string& event) const {
	bool result{};
	switch(cmp) {
	case Comparison::Less:
		result = event < this->event;
		break;
	case Comparison::LessOrEqual:
		result = event <= this->event;
		break;
	case Comparison::Greater:
		result = event > this->event;
		break;
	case Comparison::GreaterOrEqual:
		result = event >= this->event;
		break;
	case Comparison::Equal:
		result = event == this->event;
		break;
	case Comparison::NotEqual:
		result = event != this->event;
		break;
	default:
		break;
	}
	return result;
}

LogicalOperationNode::LogicalOperationNode(const LogicalOperation& log_op,
		const std::shared_ptr<Node> l_node,
		const std::shared_ptr<Node> r_node)
	: op(log_op)
	, left(l_node)
	, right(r_node)
{
}

bool LogicalOperationNode::Evaluate(const Date& date, const std::string& event) const {
	bool result{};
	switch(op) {
	case LogicalOperation::Or:
		result = left->Evaluate(date, event) | right->Evaluate(date, event);
		break;
	case LogicalOperation::And:
		result = left->Evaluate(date, event) & right->Evaluate(date, event);
		break;
	default:
		break;
	}
	return result;
}
