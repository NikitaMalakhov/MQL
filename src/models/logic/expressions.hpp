#ifndef __EXPRESSIONS_HPP
#define __EXPRESSIONS_HPP
#include <string>
#include <vector>
#include <set>
#include <iostream>

class Expression
{
public:
    virtual bool eval(std::vector<std::string> &col, std::vector<std::variant<long, std::string>> &row) const = 0;
    virtual ~Expression() {}
};

class ComparisonExpression : public Expression
{
public:
    std::string columnName;
    std::string op;
    std::variant<long, std::string> value;

    bool n;
    ComparisonExpression(std::string columnName, std::string op, std::variant<long, std::string> &val, bool n) : columnName(columnName), op(op), value(val), n(n)
    {
    }
    bool eval(std::vector<std::string> &col, std::vector<std::variant<long, std::string>> &row) const override;
};

class LRExpression : public Expression
{

public:
    Expression *left;
    Expression *right;

    std::string op;
    bool n;

    LRExpression(Expression *l, Expression *r, std::string op, bool n) : left(l), right(r), op(op), n(n)
    {
    }
    bool eval(std::vector<std::string> &col, std::vector<std::variant<long, std::string>> &row) const override;
};

class LikeExpression : public Expression
{

public:
    std::string columnName;
    std::string pattern;

    bool n;
    LikeExpression(std::string columnName, std::string &pat, bool n) : columnName(columnName), pattern(pat), n(n)
    {
    }
    bool eval(std::vector<std::string> &col, std::vector<std::variant<long, std::string>> &row) const override;
    bool like(std::string &value, const std::string &pattern) const;
    bool matchBracket(char &c, const std::string &pattern, size_t &pIdx) const;
};

class InExpression : public Expression
{
public:
    std::string columnName;
    std::set<std::variant<long, std::string>> values;

    bool n;
    InExpression(std::string colName, std::set<std::variant<long, std::string>> &vals, bool n) : columnName(colName), values(vals), n(n) {}
    bool eval(std::vector<std::string> &col, std::vector<std::variant<long, std::string>> &row) const override;
};

class AllExpression : public Expression
{
public:
    AllExpression() {}
    bool eval(std::vector<std::string> &col, std::vector<std::variant<long, std::string>> &row) const override { return true; }
};
#endif