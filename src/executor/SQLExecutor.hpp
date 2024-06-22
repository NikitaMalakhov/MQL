#ifndef __SQLEXECUTOR_HPP
#define __SQLEXECUTOR_HPP

#include "../models/ret_models.hpp"
#include "../lib/Table.hpp"
#include "poliz/POLIZ.hpp"

class SQLExecutor
{
public:
    bool execCreate(CreateS *createStruct);
    bool execSelect(SelectS *selectStruct);
    bool execInsert(InsertS *insertStruct);
    bool execUpdate(UpdateS *updateStruct);
    bool execDelete(DeleteS *deleteStruct);
    bool execDrop(DropS *dropStruct);
    SQLExecutor();  
    
    ~SQLExecutor();
};

#endif