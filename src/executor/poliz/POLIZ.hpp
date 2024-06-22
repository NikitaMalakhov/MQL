#ifndef __POLIZ_HPP

#define __POLIZ_HPP

#include <iostream>
#include <vector>
#include <string>
#include <cctype>
#include <stack>


class POLIZ{
public:
    static int priorityOfOperand(std::string operand);
    static std::vector<std::string> infixToPostfix(const std::vector<std::string>& infixExpression);
    static std::vector<std::string> tokenizeExpression(const std::string& expression);
    static long calculatePostfix(const std::vector<std::string>& postfixExpression);


    POLIZ(){};
    ~POLIZ(){}
};

#endif

