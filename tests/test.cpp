#include <cstdlib>
#include <cstring>
#include "../src/lib/Table.hpp"
#include <iostream>

int main()
{
    THandle tableHandle;
    char name[16]; // 15 characters + null terminator
    struct TableStruct tableStruct;
    tableStruct.numOfFields = 3; // Количество полей в таблице

    // Определяем поля таблицы
    struct FieldDef fieldsDef[3] = {
        {"id", Long, sizeof(long)},
        {"name", Text, 10},
        {"age", Long, sizeof(long)}};
    tableStruct.fieldsDef = fieldsDef;

    // Создаем таблицу
    enum Errors result = createTable("students", &tableStruct);
    if (result != OK)
    {
        printf("Ошибка при создании таблицы.\n");
        return 1;
    }

    if (openTable("students", &tableHandle) != OK)
    {
        std::cerr << "Error opening table." << std::endl;
        return 1;
    }

    // Insert 20 rows
    for (int i = 0; i < 20; ++i)
    {
        // Create a new record
        if (createNew(tableHandle) != OK)
        {
            std::cerr << "Error creating new record." << std::endl;
            return 1;
        }

        // Set the id field
        if (putLongNew(tableHandle, "id", i) != OK)
        {
            std::cerr << "Error setting id field." << std::endl;
            return 1;
        }

        // Generate a name for the record
        sprintf(name, "Name %d", i);

        // Set the name field
        if (putTextNew(tableHandle, "name", name) != OK)
        {
            std::cerr << "Error setting name field." << std::endl;
            return 1;
        }

        // Set the age field
        if (putLongNew(tableHandle, "age", rand() % 100) != OK)
        {
            std::cerr << "Error setting age field." << std::endl;
            return 1;
        }

        // Insert the record at the end of the table
        if (insertzNew(tableHandle) != OK)
        {
            std::cerr << "Error inserting record." << std::endl;
            return 1;
        }
    }

    // Close the table
    if (closeTable(tableHandle) != OK)
    {
        std::cerr << "Error closing table." << std::endl;
        return 1;
    }

    return 0;
}
