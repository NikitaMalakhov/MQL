#ifndef __LOGNODE_HPP
#define __LOGNODE_HPP
#include <string>
#include <vector>
#include "logfilter.hpp"


struct LogExpressionNode {
    std::vector<LogFilter> logFilters;

    std::vector<LogExpressionNode> children;
    std::string logicalOperator;
    bool isNegated;
};

#endif