#include "lib/Table.hpp"
#include "sql/SQL.hpp"
#include <iostream>
#include <string>
#include <vector>

int main()
{
    std::string query;
    SQL sql;
    while (true)
    {
        std::cout << "[>]:";
        std::getline(std::cin, query);
        sql.run(query);
    }
}