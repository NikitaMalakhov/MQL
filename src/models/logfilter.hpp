#ifndef __LOGFILTER_HPP
#define __LOGFILTER_HPP

#include <string>


enum Operator
{
    EQUAL,
    GREATER,
    LESS,
    NOTEQUAL
};


struct LogFilter
{
    bool isString;
    std::string field;
    std::string op;
    std::string stringValue;
    long longValue;
};


#endif