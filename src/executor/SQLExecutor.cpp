#include "SQLExecutor.hpp"
#include "../models/values_insert.hpp"
#include <unordered_map>
#include <iostream>
#include <iomanip>
#include "../parser/SQLParser.hpp"
#include <fstream>
#include <cstdio>
#include <cstdlib>


template <typename T, typename... Ts>
std::ostream &operator<<(std::ostream &os, const std::variant<T, Ts...> &v)
{
    std::visit([&os](auto &&arg)
               { os << arg; },
               v);
    return os;
}

SQLExecutor::SQLExecutor()
{
}

SQLExecutor::~SQLExecutor()
{
}

bool SQLExecutor::execDelete(DeleteS *deleteStruct)
{
    THandle tableHandle;
    unsigned int numFields;
    enum Errors res = openTable(const_cast<char *>(deleteStruct->tableName.c_str()), &tableHandle);

    std::unordered_map<std::string, enum FieldType> map;
    if (getFieldsNum(tableHandle, &numFields) != OK)
        return 1;

    std::vector<std::string> allFields(numFields);
    for (unsigned i = 0; i < numFields; ++i)
    {
        char *fieldName;
        if (getFieldName(tableHandle, i, &fieldName) != OK)
        {
            return 1;
        }
        allFields[i] = fieldName;
    }

    for (const auto &f : allFields)
    {
        enum FieldType type;
        res = getFieldType(tableHandle, const_cast<char *>(f.c_str()), &type);
        map.insert(std::make_pair(f, type));
    }

    moveFirst(tableHandle);

    while (!afterLast(tableHandle))
    {
        std::vector<std::variant<long, std::string>> v;
        for (auto &field : allFields)
        {
            switch (map[field])
            {
            case Long:
                long l;
                res = getLong(tableHandle, const_cast<char *>(field.c_str()), &l);
                v.push_back(l);
                break;
            case Text:
                char *text;
                res = getText(tableHandle, const_cast<char *>(field.c_str()), &text);
                std::string t(text);
                v.push_back(t);
                break;
            }
        }
        if (deleteStruct->filters->eval(allFields, v))
        {
            std::cout << 1 << std::endl;
            deleteRec(tableHandle);
        }
        res = moveNext(tableHandle);
    }
    closeTable(tableHandle);
}

bool SQLExecutor::execUpdate(UpdateS *updateStruct)
{
    THandle tableHandle;
    unsigned int numFields;
    enum Errors res = openTable(const_cast<char *>(updateStruct->tableName.c_str()), &tableHandle);

    std::unordered_map<std::string, enum FieldType> map;
    if (getFieldsNum(tableHandle, &numFields) != OK)
        return 1;

    std::vector<std::string> allFields(numFields);
    for (unsigned i = 0; i < numFields; ++i)
    {
        char *fieldName;
        if (getFieldName(tableHandle, i, &fieldName) != OK)
        {
            return 1;
        }
        allFields[i] = fieldName;
    }

    for (const auto &f : allFields)
    {
        enum FieldType type;
        res = getFieldType(tableHandle, const_cast<char *>(f.c_str()), &type);
        map.insert(std::make_pair(f, type));
    }

    moveFirst(tableHandle);

    while (!afterLast(tableHandle))
    {
        std::vector<std::variant<long, std::string>> v;
        for (auto &field : allFields)
        {
            switch (map[field])
            {
            case Long:
                long l;
                res = getLong(tableHandle, const_cast<char *>(field.c_str()), &l);
                v.push_back(l);
                break;
            case Text:
                char *text;
                res = getText(tableHandle, const_cast<char *>(field.c_str()), &text);
                std::string t(text);
                v.push_back(t);
                break;
            }
        }
        if (updateStruct->filters->eval(allFields, v))
        {
            for (const auto &update : updateStruct->updates)
            {
                std::string r = update.after_equal;
                for (const auto &field : allFields)
                {
                    const std::string s = field;
                    std::string t;
                    if (map[field] == Text)
                    {
                        char *text;
                        res = getText(tableHandle, const_cast<char *>(field.c_str()), &text);
                        std::string a(text);
                        t = a;
                    }
                    else
                    {
                        long l;
                        res = getLong(tableHandle, const_cast<char *>(field.c_str()), &l);
                        t = std::to_string(l);
                    }

                    std::string::size_type n = 0;
                    while ((n = r.find(s, n)) != std::string::npos)
                    {
                        r.replace(n, s.size(), t);
                        n += t.size();
                    }
                }

                std::vector<std::string> vec = POLIZ::infixToPostfix(POLIZ::tokenizeExpression(r));

                long result = POLIZ::calculatePostfix(vec);
                startEdit(tableHandle);
                putLong(tableHandle, const_cast<char *>(update.before_equal.c_str()), result);
                finishEdit(tableHandle);
            }
        }
        res = moveNext(tableHandle);
    }
    closeTable(tableHandle);
}

bool SQLExecutor::execCreate(CreateS *createStruct)
{
    struct TableStruct tableStruct;
    tableStruct.numOfFields = createStruct->fields.size();

    struct FieldDef *fields = new struct FieldDef[tableStruct.numOfFields];
    for (int i = 0; i < tableStruct.numOfFields; i++)
    {
        strcpy(fields[i].name, createStruct->fields[i].name.c_str());
        fields[i].len = createStruct->fields[i].len;
        fields[i].type = (createStruct->fields[i].type == "TEXT") ? Text : Long;
    }
    tableStruct.fieldsDef = fields;
    enum Errors res = createTable(const_cast<char *>(createStruct->tableName.c_str()), &tableStruct);
    return res == OK;
}

bool SQLExecutor::execInsert(InsertS *insertStruct)
{
    THandle tableHandle;
    enum Errors res = openTable(const_cast<char *>(insertStruct->tableName.c_str()), &tableHandle);

    enum FieldType fieldTypes[insertStruct->values.size()];

    unsigned int numFields;
    if (getFieldsNum(tableHandle, &numFields) != OK)
        return 1;
    std::vector<std::string> fieldNames(numFields);

    for (unsigned i = 0; i < numFields; ++i)
    {
        char *fieldName;
        if (getFieldName(tableHandle, i, &fieldName) != OK)
        {
            return 1;
        }
        fieldNames[i] = fieldName;
    }
    std::vector<std::string> &fields = fieldNames;

    std::unordered_map<std::string, enum FieldType> map;

    for (const auto &f : fields)
    {
        enum FieldType type;
        res = getFieldType(tableHandle, const_cast<char *>(f.c_str()), &type);
        map.insert(std::make_pair(f, type));
    }
    createNew(tableHandle);
    for (int i = 0; i < fields.size(); i++)
    {
        std::string field = fields[i];
        struct ValuesInsert insertValue = insertStruct->values[i];

        if (insertValue.isString)
        {
            putTextNew(tableHandle, const_cast<char *>(field.c_str()), const_cast<char *>(insertValue.stringValue.c_str()));
        }
        else
        {
            putLongNew(tableHandle, const_cast<char *>(field.c_str()), insertValue.longValue);
        }
    }
    insertNew(tableHandle);
    closeTable(tableHandle);
}

bool SQLExecutor::execDrop(DropS *dropStruct)
{
    std::remove(const_cast<char *> (dropStruct->tableName.c_str()));
}

bool SQLExecutor::execSelect(SelectS *selectStruct)
{
    THandle tableHandle;

    enum Errors res = openTable(const_cast<char *>(selectStruct->tableName.c_str()), &tableHandle);
    enum FieldType fieldTypes[selectStruct->fields.size()];

    std::vector<std::string> &fields = selectStruct->fields;

    std::unordered_map<std::string, enum FieldType> map;

    unsigned int numFields;
    if (getFieldsNum(tableHandle, &numFields) != OK)
        return 1;
    std::vector<std::string> allFields(numFields);
    for (unsigned i = 0; i < numFields; ++i)
    {
        char *fieldName;
        if (getFieldName(tableHandle, i, &fieldName) != OK)
        {
            return 1;
        }
        allFields[i] = fieldName;
    }

    if (selectStruct->allColumns)
    {
        unsigned int numFields;
        if (getFieldsNum(tableHandle, &numFields) != OK)
            return 1;
        std::vector<std::string> fieldNames(numFields);
        for (unsigned i = 0; i < numFields; ++i)
        {
            char *fieldName;
            if (getFieldName(tableHandle, i, &fieldName) != OK)
            {
                return 1;
            }
            fieldNames[i] = fieldName;
        }
        fields = fieldNames;
    }

    for (const auto &f : allFields)
    {
        enum FieldType type;
        res = getFieldType(tableHandle, const_cast<char *>(f.c_str()), &type);
        map.insert(std::make_pair(f, type));
    }

    std::vector<std::vector<std::variant<long, std::string>>> result;

    moveFirst(tableHandle);
    std::vector<int> widths;
    std::cout << std::endl;
    std::cout << "|";
    for (const auto &s : fields)
    {
        std::cout << std::setw(15) << std::left << s << "|";
    }
    std::cout << std::endl;
    for (int i = 0; i < fields.size(); i++)
    {
        std::cout << " ";
        std::cout << "---------------";
    }
    std::cout << std::endl;
    while (!afterLast(tableHandle))
    {
        std::vector<std::variant<long, std::string>> v;
        for (auto &field : allFields)
        {
            switch (map[field])
            {
            case Long:
                long l;
                res = getLong(tableHandle, const_cast<char *>(field.c_str()), &l);
                v.push_back(l);
                break;
            case Text:
                char *text;
                res = getText(tableHandle, const_cast<char *>(field.c_str()), &text);
                std::string t(text);
                v.push_back(t);
                break;
            }
        }
        if (selectStruct->filters != nullptr)
        {
            if (selectStruct->filters->eval(allFields, v))
            {
                std::cout << "|";
                for (int i = 0; i < allFields.size(); i++)
                {
                    std::variant<long, std::string> d = v[i];
                    if (std::find(fields.begin(), fields.end(), allFields[i]) != fields.end())
                        std::cout << std::setw(15) << std::left << d << "|";
                }
                std::cout << std::endl;
                result.push_back(v);
            }
            res = moveNext(tableHandle);
        }
        else
        {
            std::cout << "|";

            for (int i = 0; i < allFields.size(); i++)
            {
                std::variant<long, std::string> d = v[i];
                if (std::find(fields.begin(), fields.end(), allFields[i]) != fields.end())
                    std::cout << std::setw(15) << std::left << d << "|";
            }
            std::cout << std::endl;
            res = moveNext(tableHandle);
            if (res != OK)
            {
                printf("Ошибка при перемещении указателя на следующую запись.\n");
                closeTable(tableHandle);
                return 1;
            }
        }
    }
    std::cout << std::endl;

    return true;
}
