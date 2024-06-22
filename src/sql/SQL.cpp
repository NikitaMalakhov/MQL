#include "SQL.hpp"

SQL::SQL()
{
}

SQL::~SQL()
{
    delete this->parser;
    delete this->executor;
}

void SQL::run(std::string query)
{
    this->parser = new SQLParser(query);
    this->executor = new SQLExecutor();
    try
    {
        ParserResult *result = this->parser->parse();
        if (result->type == Create)
        {
            this->executor->execCreate(result->create);
        }
        else if (result->type == Select)
        {
            this->executor->execSelect(result->select);
        }
        else if (result->type == Insert)
        {
            this->executor->execInsert(result->insert);
        }
        else if (result->type == Update)
        {
            this->executor->execUpdate(result->update);
        }
        else if (result->type == Delete)
        {
            this->executor->execDelete(result->deletee);
        }
        else if (result->type == Drop)
        {
            this->executor->execDrop(result->drop);
        }
    }
    catch (const std::string &ex)
    {
        std::cerr << ex << '\n';
    }
}