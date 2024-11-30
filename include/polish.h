#pragma once

#include <stack>
#include <unordered_map>
#include <functional>
#include "structs.h"

using namespace std;

class polishConverter {
private:
    // ��������� ��������
    std::unordered_map<std::string, int> precedence = {
        {"+", 1}, {"-", 1}, {"*", 2}, {"/", 2}, {"^", 3}
    };

    // ����������������� ��������
    std::unordered_map<std::string, bool> leftAssociative = {
        {"+", true}, {"-", true}, {"*", true}, {"/", true}, {"^", false}
    };

public:
    // �������������� � �������� �������� �������
    vector<token> toPolish(const std::vector<token>& tokens) {
        vector<token> output;
        std::stack<token> operators; //���� ��� ����������

        for (const token& token : tokens) {
            switch (token.type) {
            case tokenType::NUMBER:
            case tokenType::VARIABLE:
                output.push_back(token);
                break;

            case tokenType::FUNCTION:
                operators.push(token);
                break;

            case tokenType::OPERATOR: {
                while (!operators.empty() && operators.top().type != tokenType::PARENTHESIS && ((leftAssociative[token.value] && precedence[token.value] <= precedence[operators.top().value]) ||
                        (!leftAssociative[token.value] && precedence[token.value] < precedence[operators.top().value]))) {

                    output.push_back(operators.top());
                    operators.pop();
                }
                operators.push(token);
                break;
            }

            case tokenType::PARENTHESIS:
                if (token.value == "(") {
                    operators.push(token);
                }
                else if (token.value == ")") {
                    while (!operators.empty() && operators.top().value != "(") {
                        output.push_back(operators.top());
                        operators.pop();
                    }
                    if (operators.empty() || operators.top().value != "(") {
                        throw std::runtime_error("Mismatched parentheses");
                    }
                    operators.pop(); // ������� "("

                    // ���� ����� ����� �� ������� ����� �������, ����������� �
                    if (!operators.empty() && operators.top().type == tokenType::FUNCTION) {
                        output.push_back(operators.top());
                        operators.pop();
                    }
                }
                break;

            default:
                throw std::runtime_error("Unknown token type");
            }
        }

        // ����������� ���������� ���������
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
                // ���� ����� � �����, ����������� � ������ � ����
                stack.push(stod(token.value));
            }
            else if (token.type == tokenType::VARIABLE){
                auto it = variables.find(token.value);
                stack.push(it->second);
            }
            else if (token.type == tokenType::OPERATOR) {
                // ���� ����� � ��������, ��������� ��� ����� �� ����� � ��������� ��������
                if (stack.size() < 2) throw invalid_argument("������������ ��������� ��� ��������");

                double b = stack.top(); stack.pop();
                double a = stack.top(); stack.pop();

                double result = applyOperator(token.value, a, b);
                stack.push(result);
            }
            else if (token.type == tokenType::FUNCTION) {
                // ���� ����� � �������, ��������� ���� ����� �� ����� � ��������� �������
                if (stack.empty()) throw invalid_argument("������������ ��������� ��� �������");

                double arg = stack.top(); stack.pop();
                double result = applyFunction(token.value, arg);
                stack.push(result);
            }
            else {
                throw invalid_argument("����������� �����: " + token.value);
            }
        }

        // � ����� ������ �������� ����� ���� �������� � ��������� ����������
        if (stack.size() != 1) throw logic_error("������������ ���������");

        return stack.top();
    }

private:
    // ��������� ��������
    double applyOperator(const string& op, double a, double b) {
        if (op == "+") return a + b;
        if (op == "-") return a - b;
        if (op == "*") return a * b;
        if (op == "/") {
            if (b == 0) throw invalid_argument("������� �� ����");
            return a / b;
        }
        if (op == "^") return pow(a, b);

        throw invalid_argument("����������� ��������: " + op);
    }

    // ��������� �������
    double applyFunction(const std::string& func, double arg) {
        static const std::unordered_map<std::string, std::function<double(double)>> functions = {
            {"sin", [](double x) { return sin(x); }},
            {"cos", [](double x) { return cos(x); }},
            {"tan", [](double x) { return tan(x); }},
            {"log", [](double x) {
                if (x <= 0) throw std::invalid_argument("�������� �������������� ����� ��� ����");
                return log(x);
            }},
            {"sqrt", [](double x) {
                if (x < 0) throw std::invalid_argument("���������� ������ �� �������������� �����");
                return sqrt(x);
            }},
        };

        auto it = functions.find(func);
        if (it != functions.end()) {
            return it->second(arg);
        }

        throw std::invalid_argument("����������� �������: " + func);
    }
};