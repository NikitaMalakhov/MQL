#include "POLIZ.hpp"

// Алгоритм преобразования инфиксной записи в постфиксную

// Перебираем выражение слева на право.
// 1. Если входящий элемент число, то добавляем его в очередь (QUEUE).
// 2. Если входящий элемент оператор (+, -, *, /) то проверяем:
//      Если стек (STACK) пуст или содержит левую скобку в вершине (TOP), то 
//          добавляем (PUSH) входящий оператор в стек (STACK).
//      Если входящий оператор имеет более высокий приоритет чем вершина (TOP), 
//          поместите (PUSH) его в стек (STACK).
//      Если входящий оператор имеет более низкий или равный приоритет, чем вершине (TOP), 
//          выгружаем POP в очередь (QUEUE), пока не увидите оператор 
//          с меньшим приоритетом или левую скобку на вершине (TOP), 
//          затем добавьте (PUSH) входящий оператор в стек (STACK).
// 3. Если входящий элемент является левой скобкой, поместите (PUSH) его в стек (STACK).
// 4. Если входящий элемент является правой скобкой, 
//      выгружаем стек (POP) и добавляем его элементы в очередь (QUEUE), 
//      пока не увидите левую круглую скобку. Удалите найденную скобку из стека (STACK).
// 5. В конце выражения выгрузите стек (POP) в очередь (QUEUE)


// // Функция для определения приоритета оператора
int POLIZ::priorityOfOperand(std::string operand){
    switch (operand[0])
    {
    case '+':
    case '-':
        return 1;
        break;

    case '*':
    case '/':
        return 2;

    case '%':
        return 3;
    default:
        return 0;
        break;
    }
}

// Функция преобразования инфиксной в постфиксную через указанный выше алгоритм 
std::vector<std::string> POLIZ::infixToPostfix(const std::vector<std::string>& infixExpression){
    std::vector<std::string> queueForPostfix;
    std::stack<std::string> stackForOperands;

    for (const std::string &stringInVector: infixExpression){
        if (std::isdigit(stringInVector[0])) {
            queueForPostfix.push_back(stringInVector);
        }else if (stringInVector[0] == '('){
            stackForOperands.push(stringInVector);
        }else if (stringInVector[0] == ')'){
            while (!stackForOperands.empty() && stackForOperands.top() != "("){ 
                // здесь мы ищем левую скобку, поэтому лексически должно быть правильно записано выражение,иначе error
                queueForPostfix.push_back(stackForOperands.top());
                stackForOperands.pop();
            }       
            stackForOperands.pop(); // удаляем '(' из стека
        }else{// это либо +,-,*,/,%
            while (!stackForOperands.empty() && priorityOfOperand(stringInVector) <= priorityOfOperand(stackForOperands.top())) {
                queueForPostfix.push_back(stackForOperands.top());
                stackForOperands.pop();
            }
            stackForOperands.push(stringInVector);
        }
    }
    // прошли всю строку, теперь весь стек закидываем в очередь до конца
    while (!stackForOperands.empty()){
        queueForPostfix.push_back(stackForOperands.top());
        stackForOperands.pop();
    }

    return queueForPostfix;
}

std::vector<std::string> POLIZ::tokenizeExpression(const std::string& expression) {
    std::vector<std::string> tokens;
    std::string currentToken;

    for (char c : expression) {
        if (std::isdigit(c)) {
            currentToken += c; // Add digit to current token
        } else if (!isspace(c)){
            // Add current token to vector if not empty
            if (!currentToken.empty()) {
                tokens.push_back(currentToken);
                currentToken.clear(); // Clear current token
            }

            // Add operator to vector as a separate token
            tokens.push_back(std::string(1, c));
        }
    }

    // Add the last token if it's not empty
    if (!currentToken.empty()) {
        tokens.push_back(currentToken);
    }

    return tokens;
}

// функция вычисления результата постфиксной записи
long POLIZ::calculatePostfix(const std::vector<std::string>& postfixExpression){
    std::stack<std::string> stackForNumbers;

    for (const std::string& element: postfixExpression){
        if (std::isdigit(element[0])){
            // добавляем число в стек
            stackForNumbers.push(element);
        }
        else{
            // из стека получаем два операнда. сначала второй, потом первый!
            std::string operand2 = stackForNumbers.top();
            stackForNumbers.pop();
            std::string operand1 = stackForNumbers.top();
            stackForNumbers.pop();

            long result;
            // выполняем нужную операцию и заносим результат в стек
            switch (element[0]){
                case '+':
                    result = std::stol(operand1) + std::stol(operand2);
                    stackForNumbers.push(std::to_string(result));
                    break;
                case '-':
                    result = std::stol(operand1) - std::stol(operand2);
                    stackForNumbers.push(std::to_string(result));
                    break;
                case '*':
                    result = std::stol(operand1) * std::stol(operand2);
                    stackForNumbers.push(std::to_string(result));
                    break;
                case '/':
                    result = std::stol(operand1) / std::stol(operand2);
                    stackForNumbers.push(std::to_string(result));
                    break;
                case '%':
                    result = std::stol(operand1) % std::stol(operand2);
                    stackForNumbers.push(std::to_string(result));
                    break;
            }
        }
    }
    //возвращаем последний оставшийся элемент в стеке - результат 
    return std::stol(stackForNumbers.top());
}


// int main() {
//     std::string expression;
//     getline(std::cin,expression);


//     std::vector<std::string> tokens = tokenizeExpression(expression);


//     // Output the tokens
//     for (const std::string& token : tokens) {
//         std::cout << token << " ";
//     }
//     std::cout << std::endl;

//     std::cout << std::endl;
//     std::cout << "postfix:"<<std::endl;
//     std::vector<std::string> postfixExpression = infixToPostfix(tokens);

//     // Output the tokens
//     for (const std::string& element : postfixExpression) {
//         std::cout <<  element << " ";
//     }
//     std::cout << std::endl;


//     long resultOfPOLIZ = calculatePostfix(postfixExpression);
//     std::cout << "Result = " << resultOfPOLIZ << std::endl;
//     return 0;
// }


