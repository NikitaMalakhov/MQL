#include "SQLParser.hpp"

ParserResult *SQLParser::parse()
{
    ParserResult *parserResult = new ParserResult();
    parserResult->create = NULL;
    parserResult->select = NULL;
    parserResult->drop = NULL;
    parserResult->update = NULL;
    parserResult->insert = NULL;
    parserResult->deletee = NULL;
    this->skipWhitespace();

    if (match("SELECT"))
    {
        SelectS *s = this->parseSelect();
        parserResult->type = Select;
        parserResult->select = s;
    }
    else if (match("CREATE"))
    {
        this->skipWhitespace();
        if (match("TABLE"))
        {
            CreateS *c = this->parseCreate();
            parserResult->create = c;
            parserResult->type = Create;
        }
    }
    else if (match("DROP"))
    {
        this->skipWhitespace();
        if (match("TABLE"))
        {
            DropS *d = this->parseDrop();
            parserResult->drop = d;
            parserResult->type = Drop;
        }
    }
    else if (match("UPDATE"))
    {
        UpdateS *u = this->parseUpdate();
        parserResult->type = Update;
        parserResult->update = u;
    }
    else if (match("INSERT"))
    {
        InsertS *i = this->parseInsert();
        parserResult->type = Insert;
        parserResult->insert = i;
    }
    else if (match("DELETE"))
    {
        DeleteS *d = this->parseDelete();
        parserResult->type = Delete;
        parserResult->deletee = d;
    }
    else
    {
        std::string errorString = "Syntax error at parse.\n" + this->query + "\n";
        for (int i = 0; i < this->currentPosition; i++)
            errorString += " ";
        errorString += "^";
        throw errorString;
    }
    return parserResult;
}

bool SQLParser::match(const std::string &exp)
{
    if (this->query.substr(this->currentPosition, exp.length()) == exp && (this->currentPosition + exp.length() <= this->query.length()))
    {
        this->currentPosition += exp.length();
        return true;
    }
    return false;
}

CreateS *SQLParser::parseCreate()
{
    try
    {
        this->skipWhitespace();
        std::string tableName = this->parseIdentifier();

        std::vector<struct FieldModel> fields;

        this->skipWhitespace();

        if (match("("))
        {
            while (!match(")"))
            {
                this->skipWhitespace();
                struct FieldModel fieldData;
                std::string fieldName = this->parseIdentifier();

                this->skipWhitespace();

                fieldData.name = fieldName;

                std::string typeName = this->parseIdentifier();

                fieldData.type = typeName;

                this->skipWhitespace();
                if (match("(")) // it means it is text
                {
                    int textLen = std::stoi(this->parseIdentifier());
                    fieldData.len = textLen;
                    if (!match(")"))
                    {
                        std::string errorString = "Syntax error at parseCreate. Expected ')'\n" + this->query + "\n";
                        for (int i = 0; i < this->currentPosition; i++)
                            errorString += " ";
                        errorString += "^";
                        throw errorString;
                    }
                }
                else
                {
                    this->skipWhitespace();
                    fieldData.len = sizeof(long);
                }

                fields.push_back(fieldData);

                if (this->query[this->currentPosition] == ',')
                    ++this->currentPosition;
            }
            this->skipWhitespace();
        }
        else
        {
            std::string errorString = "Syntax error at parseCreate. Expected '('\n" + this->query + "\n";
            for (int i = 0; i < this->currentPosition; i++)
                errorString += " ";
            errorString += "^";
            errorString += std::to_string(this->currentPosition);
            throw errorString;
        }
        CreateS *createS = new CreateS();
        createS->tableName = tableName;
        createS->fields = fields;

        return createS;
    }
    catch (const std::exception &ex)
    {
        std::cerr << ex.what() << '\n';
    }
}

SelectS *SQLParser::parseSelect()
{
    try
    {
        // SELECT id, name, age FROM table
        // SELECT *
        std::vector<std::string> fields;
        std::string tableName;
        std::vector<struct LogExpressionNode> filters;
        bool allColumns = false;
        while (true)
        {
            this->skipWhitespace();
            std::string fieldName = this->parseIdentifier();
            if (fieldName == "*")
            {
                allColumns = true;
                break;
            }
            fields.push_back(fieldName);

            this->skipWhitespace();
            if (query[currentPosition] == ',')
                ++currentPosition;
            else
                break;
        }

        this->skipWhitespace();
        Expression *whereClause;
        if (match("FROM"))
        {
            this->skipWhitespace();
            tableName = this->parseIdentifier();
            this->skipWhitespace();

            if (match("WHERE"))
            {
                this->skipWhitespace();

                whereClause = this->parseExpression();
            }
            else
            {
                whereClause = nullptr;
            }
        }
        else
        {
            std::string errorString = "Syntax error at parseSelect. Expected 'FROM'.\n" + this->query + "\n";
            for (int i = 0; i < this->currentPosition; i++)
                errorString += " ";
            errorString += "^";
            throw errorString;
        }
        SelectS *select = new SelectS();
        select->allColumns = allColumns;
        select->fields = fields;
        select->filters = whereClause;
        select->tableName = tableName;
        return select;
    }
    catch (const std::exception &ex)
    {
        std::cerr << ex.what() << '\n';
    }
}

Expression *SQLParser::parseExpression()
{
    this->skipWhitespace();
    if (match("ALL"))
    {
        Expression *p = new AllExpression();
        return p;
    }
    else
    {
        return this->parseLogicalExpression();
    }
    return nullptr;
}

Expression *SQLParser::parseLogicalExpression()
{
    Expression *expr = parseLogicalTerm();
    this->skipWhitespace();
    while (match("OR"))
    {
        this->skipWhitespace();
        Expression *right = parseLogicalTerm();
        expr = new LRExpression(expr, right, "OR", false);
        this->skipWhitespace();
    }

    return expr;
}

Expression *SQLParser::parseLogicalTerm()
{
    Expression *expr = parseLogicalMultiplier();
    this->skipWhitespace();
    while (match("AND"))
    {
        this->skipWhitespace();
        Expression *right = parseLogicalMultiplier();
        expr = new LRExpression(expr, right, "AND", false);
        this->skipWhitespace();
    }
    return expr;
}

Expression *SQLParser::parseInExpression()
{
}

Expression *SQLParser::parseLogicalMultiplier()
{
    if (match("NOT"))
    {
        this->skipWhitespace();

        return parseComparisonExpression(true);
    }
    else if (match("("))
    {
        this->skipWhitespace();
        Expression *expr = parseLogicalExpression();
        this->skipWhitespace();
        match(")");
        return expr;
    }
    else if (match("("))
    {
        this->skipWhitespace();
        Expression *expr = parseComparisonExpression(false);
        this->skipWhitespace();
        match(")");
        return expr;
    }
    else
    {
        this->skipWhitespace();
        Expression *expr = parseComparisonExpression(false);
        return expr;
    }
}

// while (match("AND"))
// {
//     this->skipWhitespace();

//     Expression * right = parseComparisonExpression();
//     expr = std::make_unique<LRExpression>(std::move(expr), std::move(right), "AND");
//     this->skipWhitespace();
// }

std::string SQLParser::parseIdentifier()
{
    size_t start = currentPosition;
    while (isalnum(this->query[this->currentPosition]) || this->query[this->currentPosition] == '_' || this->query[this->currentPosition] == '*' || this->query[this->currentPosition] == '-' || this->query[this->currentPosition] == '%')
    {
        this->currentPosition++;
    }
    return this->query.substr(start, this->currentPosition - start);
}

void SQLParser::skipWhitespace()
{
    while (this->currentPosition < this->query.length() && std::isspace(this->query[this->currentPosition]))
        ++this->currentPosition;
}

bool is_digits(const std::string &str)
{
    return std::all_of(str.begin(), str.end(), ::isdigit); // C++11
}

Expression *SQLParser::parseComparisonExpression(bool n = false)
{
    Expression *expr;
    this->skipWhitespace();
    std::string columnName = this->parseIdentifier();

    this->skipWhitespace();
    std::string op;

    if (match(">="))
        op = ">=";
    else if (match("<="))
        op = "<=";
    else if (match("="))
        op = "=";
    else if (match("<"))
        op = "<";
    else if (match(">"))
        op = ">";
    else if (match("NOT"))
        n = !n;

    this->skipWhitespace();

    if (match("LIKE"))
    {
        this->skipWhitespace();
        match("'");
        std::string pattern = parseIdentifier();
        match("'");

        Expression *p = new LikeExpression(columnName, pattern, n);
        return p;
    }
    else if (match("IN"))
    {
        this->skipWhitespace();
        std::set<std::variant<long, std::string>> s;

        if (match("("))
        {
            this->skipWhitespace();
            while (!match(")"))
            {
                this->skipWhitespace();
                if (match("'"))
                {
                    std::string p = this->parseIdentifier();
                    match("'");
                    std::variant<long, std::string> v = p;
                    s.insert(v);
                }
                else
                {
                    std::string number = this->parseIdentifier();
                    long l = std::stol(number);
                    std::variant<long, std::string> v = l;
                    s.insert(v);
                }
                match(",");
                this->skipWhitespace();
            }
        }

        Expression *e = new InExpression(columnName, s, n);
        return e;
    }

    this->skipWhitespace();

    if (match("'"))
    {
        std::variant<long, std::string> v = this->parseIdentifier();
        Expression *p = new ComparisonExpression(columnName, op, v, n);
        match("'");
        return p;
    }
    else
    {
        std::variant<long, std::string> v = std::stol(this->parseIdentifier());
        Expression *p = new ComparisonExpression(columnName, op, v, n);
        return p;
    }
}

DropS *SQLParser::parseDrop()
{
    try
    {
        this->skipWhitespace();
        std::string tableName;
        tableName = this->parseIdentifier();
        // std::cout << tableName << std::endl;
        // else
        // {
        //     std::string errorString = "Syntax error at parseDrop. Expected 'DROP'\n" + this->query + "\n";
        //     for (int i = 0; i < this->currentPosition; i++)
        //         errorString += " ";
        //     errorString += "^";
        //     throw errorString;
        // }
        DropS *drop = new DropS();
        drop->tableName = tableName;
        return drop;
    }
    catch (const std::exception &ex)
    {
        std::cerr << ex.what() << '\n';
    }
}

UpdateS *SQLParser::parseUpdate()
{
    // UPDATE <имя таблицы> SET <имя поля> = <выражение> <WHERE-клауза>
    // <выражение> ::= <Long-выражение> | <Text-выражение>
    //
    // <Long-выражение> ::= <Long-слагаемое> { <+|-> <Long-слагаемое> }
    // <Text-выражение> ::= <имя поля типа TEXT> | <строка>

    // <Long-слагаемое> ::= <Long-множитель> { <*|/|%> <Long-множитель> }
    // <Long-множитель> ::= <Long-величина> | ( <Long-выражение> )
    // <Long-величина> ::= <имя поля типа LONG> | <длинное целое>

    // Examples:
    // 1 Age = Age  +-/*%                       ok      done with poliz
    // 2 Age = 8*(3-2)+1                        ok      done with poliz
    // 3 Age = 2 - Age*10 done
    // 4 Age = 100                              ok      done with poliz
    // 5 AGE = PhoneNumber       ok                     done

    // 6 Name = 'string'   ok                           done
    // 7 Name = Surname          ok                     done
    this->skipWhitespace();
    std::string tableName = this->parseIdentifier();

    // std::cout << tableName << std::endl;
    std::vector<struct UpdateData> updates; // SET ... WHERE
    Expression *whereClause;
    this->skipWhitespace();

    if (match("SET"))
    {

        this->skipWhitespace();
        // create vector with struct updatedData: column1 = value WHERE-statement
        while (true)
        {
            this->skipWhitespace();
            UpdateData valueStructure;
            std::string first_field = this->parseIdentifier(); // first_field - <имя поля>
            // std::cout << first_field << std::endl;

            valueStructure.before_equal = first_field;

            this->skipWhitespace();

            // std::string stringValue;
            // long longValue;
            // bool isString = false;
            if (match("="))
            {
                this->skipWhitespace();
                std::string after_equal_buffer;
                while (this->currentPosition < this->query.length())
                {
                    after_equal_buffer += this->query[currentPosition];
                    this->currentPosition++;
                    if (match("WHERE"))
                    {
                        std::string wherestr = "WHERE";
                        this->currentPosition -= wherestr.length();
                        break;
                    }
                }
                if (this->currentPosition >= this->query.length())
                {
                    std::string errorString = "Syntax error at parseUpdate. Expected 'WHERE'\nIn: " + this->query + "\n";
                    throw errorString;
                }
                valueStructure.after_equal = after_equal_buffer;

                // problem: Age = 2 + Age*2 + 23 WheRE ...

                // if (match("'"))
                // {
                //     // this is example №6: Name = 'string'
                //     stringValue = this->parseIdentifier();
                //     valueStructure.stringValue = stringValue;
                //     isString = true;
                //     match("'");
                // }
                // else
                // {

                //     if (isalpha(this->query[this->currentPosition]))
                //     { // если это буква
                //         // example №5 or №7: Name = Surname or Age = PhoneNumber
                //         std::string s = this->parseIdentifier(); // this is second identifier,откуда нужно получить long значение или text значение
                //         valueStructure.name_second_field = s;
                //         this->skipWhitespace();

                //         // example №1: Age = Age +-*/% POLIZexpresion
                //         if (match("+"))
                //         {
                //             this->skipWhitespace();
                //             std::string restOfString;
                //             while (this->query[currentPosition] != '\n' && !isalpha(this->query[currentPosition])){
                //                 restOfString += this->query[currentPosition];
                //                 this->currentPosition++;
                //             }
                //             std::vector<std::string> tokens = POLIZ::tokenizeExpression(restOfString);
                //             std::vector<std::string> postfixExpression = POLIZ::infixToPostfix(tokens);
                //             long resultOfPOLIZ = POLIZ::calculatePostfix(postfixExpression);
                //             std::cout << "resultOfPOLIZ = " << resultOfPOLIZ << std::endl << std::endl;
                //             valueStructure.longValue = resultOfPOLIZ;
                //             valueStructure.operand = "+";
                //         }
                //         else if (match("-"))
                //         {
                //             this->skipWhitespace();
                //             std::string restOfString;
                //             while (this->query[currentPosition] != '\n' && !isalpha(this->query[currentPosition])){
                //                 restOfString += this->query[currentPosition];
                //                 this->currentPosition++;
                //             }
                //             std::vector<std::string> tokens = POLIZ::tokenizeExpression(restOfString);
                //             std::vector<std::string> postfixExpression = POLIZ::infixToPostfix(tokens);
                //             long resultOfPOLIZ = POLIZ::calculatePostfix(postfixExpression);
                //             std::cout << "resultOfPOLIZ = " << resultOfPOLIZ << std::endl << std::endl;
                //             valueStructure.longValue = resultOfPOLIZ;
                //             valueStructure.operand = "-";
                //         }
                //         else if (match("*"))
                //         {
                //             this->skipWhitespace();
                //             std::string restOfString;
                //             while (this->query[currentPosition] != '\n' && !isalpha(this->query[currentPosition])){
                //                 restOfString += this->query[currentPosition];
                //                 this->currentPosition++;
                //             }
                //             std::vector<std::string> tokens = POLIZ::tokenizeExpression(restOfString);
                //             std::vector<std::string> postfixExpression = POLIZ::infixToPostfix(tokens);
                //             long resultOfPOLIZ = POLIZ::calculatePostfix(postfixExpression);
                //             std::cout << "resultOfPOLIZ = " << resultOfPOLIZ << std::endl << std::endl;
                //             valueStructure.longValue = resultOfPOLIZ;
                //             valueStructure.operand = "*";
                //         }
                //         else if (match("/"))
                //         {
                //             this->skipWhitespace();
                //             std::string restOfString;
                //             while (this->query[currentPosition] != '\n' && !isalpha(this->query[currentPosition])){
                //                 restOfString += this->query[currentPosition];
                //                 this->currentPosition++;
                //             }
                //             std::vector<std::string> tokens = POLIZ::tokenizeExpression(restOfString);
                //             std::vector<std::string> postfixExpression = POLIZ::infixToPostfix(tokens);
                //             long resultOfPOLIZ = POLIZ::calculatePostfix(postfixExpression);
                //             std::cout << "resultOfPOLIZ = " << resultOfPOLIZ << std::endl << std::endl;
                //             valueStructure.longValue = resultOfPOLIZ;
                //             valueStructure.operand = "/";
                //         }
                //         else if (match("%"))
                //         {
                //             this->skipWhitespace();
                //             std::string restOfString;
                //             while (this->query[currentPosition] != '\n' && !isalpha(this->query[currentPosition])){
                //                 restOfString += this->query[currentPosition];
                //                 this->currentPosition++;
                //             }
                //             std::vector<std::string> tokens = POLIZ::tokenizeExpression(restOfString);
                //             std::vector<std::string> postfixExpression = POLIZ::infixToPostfix(tokens);
                //             long resultOfPOLIZ = POLIZ::calculatePostfix(postfixExpression);
                //             std::cout << "resultOfPOLIZ = " << resultOfPOLIZ << std::endl << std::endl;
                //             valueStructure.longValue = resultOfPOLIZ;
                //             valueStructure.operand = "%";
                //         }
                //     }
                //     else
                //     {
                //         // examples  №2 and №4: Age = 8*(3-2)+1  , Age = 100
                //         this->skipWhitespace();
                //         //std::cout << this->query.substr(currentPosition, this->query.length() - currentPosition)  << std::endl;
                //         // std::cout << "OK 565" << std::endl;
                //          //this->query.substr(currentPosition, this->query.length() - currentPosition);
                //         std::string restOfString;
                //         while (this->query[currentPosition] != '\n' && !isalpha(this->query[currentPosition])){
                //             restOfString += this->query[currentPosition];
                //             this->currentPosition++;
                //         }
                //         //restOfString += " end";
                //         // std::cout << restOfString  << std::endl;

                //         std::vector<std::string> tokens = POLIZ::tokenizeExpression(restOfString);
                //         // // Output the tokens
                //         // for (const std::string& token : tokens) {
                //         //     std::cout << token << " ";
                //         // }
                //         // std::cout << std::endl;
                //         std::vector<std::string> postfixExpression = POLIZ::infixToPostfix(tokens);
                //         long resultOfPOLIZ = POLIZ::calculatePostfix(postfixExpression);
                //         std::cout << "resultOfPOLIZ = " << resultOfPOLIZ << std::endl << std::endl;
                //         valueStructure.longValue = resultOfPOLIZ;
                //         isString = false;
                //     }
                // }
            }
            else
            {
                std::string errorString = "Syntax error at parseUpdate. Expected '='\n" + this->query + "\n";
                for (int i = 0; i < this->currentPosition; i++)
                    errorString += " ";
                errorString += "^";
                throw errorString;
            }

            // valueStructure.isString = isString;
            updates.push_back(valueStructure);
            this->skipWhitespace();
            // это если у нас много условий будет, хотя всего одно должно быть
            if (query[currentPosition] == ',')
                ++currentPosition;
            else
                break;
        }

        this->skipWhitespace();
        if (match("WHERE"))
        {
            this->skipWhitespace();
            whereClause = this->parseExpression();
        }
        else
        {
            std::string errorString = "Syntax error at parseUpdate. Expected 'WHERE'\n" + this->query + "\n";
            for (int i = 0; i < this->currentPosition; i++)
                errorString += " ";
            errorString += "^";
            throw errorString;
        }
    }
    else
    {
        std::string errorString = "Syntax error at parseUpdate. Expected 'SET'\n" + this->query + "\n";
        for (int i = 0; i < this->currentPosition; i++)
            errorString += " ";
        errorString += "^";
        throw errorString;
    }
    UpdateS *update = new UpdateS();
    update->tableName = tableName;
    update->updates = updates;
    update->filters = whereClause;
    return update;
}

InsertS *SQLParser::parseInsert()
{
    // INSERT INTO table_name(value1,value2,... ,valueN);
    this->skipWhitespace();
    std::string tableName;
    std::vector<struct ValuesInsert> allValues;
    if (match("INTO"))
    {
        this->skipWhitespace();
        tableName = this->parseIdentifier();
        this->skipWhitespace();
        if (match("("))
        {
            while (true)
            {
                this->skipWhitespace();
                ValuesInsert value;
                std::string stringValue;
                long longValue;
                bool isString = false;
                if (match("'"))
                {
                    stringValue = this->parseIdentifier();
                    isString = true;
                    match("'");
                }
                else if (isdigit(this->query[this->currentPosition]))
                {
                    std::string s = this->parseIdentifier();
                    longValue = std::stol(s);
                    isString = false;
                }
                else
                {
                    std::string errorString = "Syntax error at parseInsert. Wrong stringValue syntax '('\n" + this->query + "\n";
                    for (int i = 0; i < this->currentPosition; i++)
                        errorString += " ";
                    errorString += "^";
                    throw errorString;
                }
                value.isString = isString;
                value.longValue = longValue;
                value.stringValue = stringValue;
                allValues.push_back(value);

                this->skipWhitespace();
                if (query[currentPosition] == ',')
                    ++currentPosition;
                else
                    break;
            }
        }
        else
        {
            std::string errorString = "Syntax error at parseInsert. Expected '('\n" + this->query + "\n";
            for (int i = 0; i < this->currentPosition; i++)
                errorString += " ";
            errorString += "^";
            throw errorString;
        }
    }
    else
    {
        std::string errorString = "Syntax error at parseInsert. Expected 'INTO'\n" + this->query + "\n";
        for (int i = 0; i < this->currentPosition; i++)
            errorString += " ";
        errorString += "^";
        throw errorString;
    }
    InsertS *insert = new InsertS();
    insert->tableName = tableName;
    insert->values = allValues;
    return insert;
}

DeleteS *SQLParser::parseDelete()
{
    // DELETE FROM tableName WHERE ...
    this->skipWhitespace();
    Expression *filters = nullptr;
    std::string tableName;
    if (match("FROM"))
    {
        this->skipWhitespace();
        tableName = this->parseIdentifier();
        this->skipWhitespace();
        if (match("WHERE"))
        {
            this->skipWhitespace();
            filters = this->parseExpression();
        }
        else
        {
            std::string errorString = "Syntax error at parseDelete. Expected 'WHERE'\n" + this->query + "\n";
            for (int i = 0; i < this->currentPosition; i++)
                errorString += " ";
            errorString += "^";
            throw errorString;
        }
    }
    else
    {
        std::string errorString = "Syntax error at parseDelete. Expected 'FROM'\n" + this->query + "\n";
        for (int i = 0; i < this->currentPosition; i++)
            errorString += " ";
        errorString += "^";
        throw errorString;
    }
    DeleteS *deletee = new DeleteS();
    deletee->filters = filters;
    deletee->tableName = tableName;
    return deletee;
}
