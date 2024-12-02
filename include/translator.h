#pragma once

#include <iostream>
#include <unordered_map>
#include "structs.h"
#include "parser.h"
#include "lexer.h"
#include "polish.h"
#include "stack.h"
using namespace std;

//��������� ��� �������� ����������

class translator {
private:
    Lexer l;
    Parser p;
    polishConverter polish;
    typeExpression typeExp;
    RPNCalculator cal;
    double result = NOTHING;
    //�������� ���������� ����������� ����������



    void setTypeExp(const vector<token>& tokens, const unordered_map<string, double>& variables, string& simpleStr, const double& lastResult) {

        //������� ���������� �����
        size_t countEquals = 0;
        for (const auto& token : tokens) {
            if (token.value == "=") ++countEquals;
        }

        if (p.checkSim(tokens) && p.checkVarInEXPR(tokens, variables) && lastResult != NOTHING) {
            typeExp = typeExpression::SIMPLE;
        }

        //���������� ��������
        else if (countEquals == 1 && tokens.size() == 1 && simpleStr.size() != 0) {
            typeExp = typeExpression::EQUAL;
        }

        //������� ����������
        else if (countEquals == 1 && tokens[0].type == tokenType::VARIABLE && tokens[1].value == "=") {
            vector<token> subTokens(tokens.begin() + 2, tokens.end());
            if (p.parserExp(subTokens) && p.checkVarInEXPR(subTokens, variables)) {
                typeExp = typeExpression::SET;
            }
        }

        //���������� ���������
        else if (countEquals == 0 && p.parserExp(tokens) && p.checkVarInEXPR(tokens, variables)) {
            typeExp = typeExpression::EXPRESSION;
        }

        else {
            typeExp = typeExpression::ERROR;
        }
    }

public:

    double logicNode(const string& str, unordered_map<string, double>& variables, string& simpleStr, double& lastResult) {

        if (str.empty()) throw 1;
        vector<token> tokens = l.makeTokens(str); //������� ������
        setTypeExp(tokens, variables, simpleStr, lastResult); //������ ��� ���������


        if (typeExp == typeExpression::SIMPLE) {
            simpleStr = str;
            token newToken(tokenType::NUMBER, to_string(lastResult));
            tokens.insert(tokens.begin(), newToken);
            result = cal.evaluate(polish.toPolish(tokens), variables);
        }

        else if (typeExp == typeExpression::EQUAL) {
            Lexer le;
            vector<token> tokens = le.makeTokens(simpleStr);
            token newToken(tokenType::NUMBER, to_string(lastResult));
            tokens.insert(tokens.begin(), newToken);
            result = cal.evaluate(polish.toPolish(tokens), variables);
        }

        else if (typeExp == typeExpression::SET) {

            if (tokens[0].type == tokenType::CONST) {
                throw 1;
            }

            else if (tokens[0].type == tokenType::FUNCTION) {
                throw 1;
            }


            vector<token> subTokens(tokens.begin() + 2, tokens.end());
            variables[tokens[0].value] = cal.evaluate(polish.toPolish(subTokens), variables);
            result = NOTHING;

        }

        else if (typeExp == typeExpression::EXPRESSION) {

            result = cal.evaluate(polish.toPolish(tokens), variables);

        }

        else if (typeExp == typeExpression::ERROR) {
        }

        return result; 
    }

};
