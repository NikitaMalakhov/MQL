#include "expressions.hpp"
#include <iostream>

bool ComparisonExpression::eval(std::vector<std::string> &col, std::vector<std::variant<long, std::string>> &row) const
{

    for (int i = 0; i < col.size(); i++)
    {
        if (col[i] == this->columnName)
        {
            bool res;
            if (op == ">")
                res = (row[i] > value);
            else if (op == "<")
                res = (row[i] < value);
            else if (op == "=")
                res = (row[i] == value);
            else if (op == "!=")
                res = (row[i] != value);
            else if (op == ">=")
                res = (row[i] >= value);
            else if (op == "<=")
                res = (row[i] <= value);
            return this->n ? !res : res;
        }
    }
    throw "Column" + this->columnName + " not found in table";
}

bool LRExpression::eval(std::vector<std::string> &col, std::vector<std::variant<long, std::string>> &row) const
{

    if (op == "AND")
    {
        bool res = (left->eval(col, row) && right->eval(col, row));
        return this->n ? !res : res;
    }
    else if (op == "OR")
    {
        bool res = left->eval(col, row) || right->eval(col, row);
        return this->n ? !res : res;
    }
}

bool LikeExpression::eval(std::vector<std::string> &col, std::vector<std::variant<long, std::string>> &row) const
{
    for (int i = 0; i < col.size(); i++)
    {
        if (col[i] == this->columnName)
        {
            return this->like(std::get<std::string>(row[i]), this->pattern);
        }
    }
    throw "Column" + this->columnName + " not found in table";
}

bool LikeExpression::matchBracket(char &c, const std::string &pattern, size_t &pIdx) const
{
    bool negation = false;
    bool match = false;
    size_t startIdx = pIdx;

    if (pattern[pIdx] == '^')
    {
        negation = true;
        ++pIdx;
    }

    while (pIdx < pattern.size() && pattern[pIdx] != ']')
    {
        if (pIdx + 1 < pattern.size() && pattern[pIdx + 1] == '-')
        {
            char startRange = pattern[pIdx];
            char endRange = pattern[pIdx + 2];
            if (startRange <= c && c <= endRange)
            {
                match = true;
            }
            pIdx += 3;
        }
        else
        {
            if (pattern[pIdx] == c)
            {
                match = true;
            }
            ++pIdx;
        }
    }

    // Пропустить закрывающую скобку
    if (pattern[pIdx] == ']')
    {
        ++pIdx;
    }

    return negation ? !match : match;
}

// Основная функция для проверки соответствия строки шаблону
bool LikeExpression::like(std::string &value, const std::string &pattern) const
{
    size_t vLen = value.length();
    size_t pLen = pattern.length();
    size_t vIdx = 0, pIdx = 0;
    size_t starIdx = -1, matchIdx = 0;

    while (vIdx < vLen)
    {
        if (pIdx < pLen && (pattern[pIdx] == value[vIdx] || pattern[pIdx] == '_'))
        {
            ++vIdx;
            ++pIdx;
        }
        else if (pIdx < pLen && pattern[pIdx] == '%')
        {
            starIdx = pIdx;
            matchIdx = vIdx;
            ++pIdx;
        }
        else if (pIdx < pLen && pattern[pIdx] == '[')
        {
            ++pIdx;
            if (matchBracket(value[vIdx], pattern, pIdx))
            {
                ++vIdx;
            }
            else if (starIdx != -1)
            {
                pIdx = starIdx + 1;
                ++matchIdx;
                vIdx = matchIdx;
            }
            else
            {
                return false;
            }
        }
        else if (starIdx != -1)
        {
            pIdx = starIdx + 1;
            ++matchIdx;
            vIdx = matchIdx;
        }
        else
        {
            return false;
        }
    }

    while (pIdx < pLen && pattern[pIdx] == '%')
    {
        ++pIdx;
    }

    return pIdx == pLen;
}

bool InExpression::eval(std::vector<std::string> &col, std::vector<std::variant<long, std::string>> &row) const
{
    for (int i = 0; i < col.size(); i++)
    {
        if (col[i] == this->columnName)
        {
            bool res;
            res = (this->values.find(row[i]) != this->values.end());
            return this->n ? !res : res;
        }
    }
    throw "Column" + this->columnName + " not found in table";
}