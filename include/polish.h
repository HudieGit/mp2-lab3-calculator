#pragma once

#include <stack>
#include <unordered_map>
#include <functional>
#include "structs.h"

using namespace std;

class polishConverter {
private:
    // Приоритет операций
    std::unordered_map<std::string, int> precedence = {
        {"+", 1}, {"-", 1}, {"*", 2}, {"/", 2}, {"^", 3}, {"u-", 4}, {"u+", 4}
    };

    // Левоассоциативные операции
    std::unordered_map<std::string, bool> leftAssociative = {
        {"+", true}, {"-", true}, {"*", true}, {"/", true}, {"^", false}
    };

public:
    // Преобразование в обратную польскую нотацию
    vector<token> toPolish(const std::vector<token>& tokens) {
        vector<token> output;
        std::stack<token> operators; //стек для операторов
        bool expectUnary = true;

        for (const token& token : tokens) {
            switch (token.type) {
            case tokenType::NUMBER:
            case tokenType::VARIABLE:
                output.push_back(token);
                expectUnary = false;
                break;

            case tokenType::FUNCTION:
                operators.push(token);
                expectUnary = true;
                break;

            case tokenType::OPERATOR: {
                std::string op = token.value;

                // Если ожидается унарный оператор
                if (expectUnary) {
                    if (op == "-") op = "u-";
                    else if (op == "+") op = "u+";
                }

                while (!operators.empty() && operators.top().type != tokenType::PARENTHESIS && ((leftAssociative[op] && precedence[op] <= precedence[operators.top().value]) ||
                        (!leftAssociative[op] && precedence[op] < precedence[operators.top().value]))) {

                    output.push_back(operators.top());
                    operators.pop();
                }

                operators.push(struct::token(tokenType::OPERATOR, op));
                expectUnary = true;
                break;
            }

            case tokenType::PARENTHESIS:
                if (token.value == "(") {
                    operators.push(token);
                    expectUnary = true;
                }
                else if (token.value == ")") {
                    while (!operators.empty() && operators.top().value != "(") {
                        output.push_back(operators.top());
                        operators.pop();
                    }
                    if (operators.empty()) {
                        throw std::runtime_error("Mismatched parentheses");
                    }
                    operators.pop(); // Удаляем "("

                    // Если после этого на вершине стека функция, выталкиваем её
                    if (!operators.empty() && operators.top().type == tokenType::FUNCTION) {
                        output.push_back(operators.top());
                        operators.pop();
                    }
                }
                expectUnary = false;
                break;

            default:
                throw std::runtime_error("Unknown token type");
            }
        }

        // Выталкиваем оставшиеся операторы
        while (!operators.empty()) {
            if (operators.top().type == tokenType::PARENTHESIS) {
                throw std::runtime_error("Mismatched parentheses");
            }
            output.push_back(operators.top());
            operators.pop();
        }

        return output;
    }
};


class RPNCalculator {
public:
    double evaluate(const vector<token>& tokens, const unordered_map<string, double>& variables) {
        std::stack<double> stack;

        for (const token& token : tokens) {
            if (token.type == tokenType::NUMBER) {
                // Если токен — число, преобразуем и кладем в стек
                stack.push(stod(token.value));
            }
            else if (token.type == tokenType::VARIABLE){
                auto it = variables.find(token.value);
                stack.push(it->second);
            }
            else if (token.type == tokenType::OPERATOR) {
                // Если токен — оператор, извлекаем два числа из стека и выполняем операцию

                if ((token.value == "u-" || token.value == "u+") && stack.size() > 0) {
                    
                    double a = stack.top(); stack.pop();

                    double result = applyUnaryOperator(token.value, a);
                    stack.push(result);
                }

                else {
                    if (stack.size() < 2) throw invalid_argument("Недостаточно операндов для операции");

                    double b = stack.top(); stack.pop();
                    double a = stack.top(); stack.pop();

                    double result = applyOperator(token.value, a, b);
                    stack.push(result);
                }  
             }
            else if (token.type == tokenType::FUNCTION) {
                // Если токен — функция, извлекаем одно число из стека и применяем функцию
                if (stack.empty()) throw invalid_argument("Недостаточно операндов для функции");

                double arg = stack.top(); stack.pop();
                double result = applyFunction(token.value, arg);
                stack.push(result);
            }
            else {
                throw invalid_argument("Неизвестный токен: " + token.value);
            }
        }

        // В стеке должно остаться ровно одно значение — результат вычисления
        if (stack.size() != 1) throw logic_error("Некорректное выражение");

        return stack.top();
    }

private:
    // Применить оператор
    double applyOperator(const string& op, double a, double b) {
        if (op == "+") return a + b;
        if (op == "-") return a - b;
        if (op == "*") return a * b;
        if (op == "/") {
            if (b == 0) throw invalid_argument("Деление на ноль");
            return a / b;
        }
        if (op == "^") return pow(a, b);

        throw invalid_argument("Неизвестный оператор: " + op);
    }

    double applyUnaryOperator(const string& op, double a){
        if (op == "u-") return -a;
        if (op == "u+") return a;
        throw invalid_argument("неизвестный оператор" + op);
    }

    // Применить функцию
    //i don't now how it works but it works
    double applyFunction(const std::string& func, double arg) {
        static const std::unordered_map<std::string, std::function<double(double)>> functions = {
            {"sin", [](double x) { return sin(x); }},
            {"cos", [](double x) { return cos(x); }},
            {"exp", [](double x) { return exp(x); }},
            {"tan", [](double x) { return tan(x); }},
            {"log", [](double x) {
                if (x <= 0) throw std::invalid_argument("Логарифм отрицательного числа или нуля");
                return log(x);
            }},
            {"sqrt", [](double x) {
                if (x < 0) throw std::invalid_argument("Квадратный корень из отрицательного числа");
                return sqrt(x);
            }},
        };

        auto it = functions.find(func);
        if (it != functions.end()) {
            return it->second(arg);
        }

        throw std::invalid_argument("Неизвестная функция: " + func);
    }
};