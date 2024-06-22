#ifndef __SQL_HPP

#define __SQL_HPP

#include "../executor/SQLExecutor.hpp"
#include "../parser/SQLParser.hpp"
#include <string>
#include <vector>



class SQL
{
private:
    SQLExecutor *executor;
    SQLParser *parser;

public:
    SQL();
    void run(std::string query);
    ~SQL();
};

#endif