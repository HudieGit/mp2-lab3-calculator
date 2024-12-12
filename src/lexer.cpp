#include "Lexer.h"


using namespace std;


vector<token> Lexer::makeTokens(const std::string& inputStr) {
    input = inputStr;
    pos = 0;

    token token = nextToken();
    while (token.type != tokenType::END) {
        if (token.type == tokenType::UNKNOWN) throw std::invalid_argument("invaliad symbol");
        result.push_back(token);
        token = nextToken();
    }

    setVariableType();

    return result;
}

token Lexer::nextToken() {
    skipSpace();

    if (pos >= input.length()) return token(tokenType::END, "");

    char current = input.at(pos);

    // если символ буква, то пытаемся собрать переменную
    if (isalpha(current) || current == '_') {
        return tokenVariable();
    }

    // если символ цифра, то пытаемся собрать число
    else if (isdigit(current)) {
        return tokenNumber();
    }

    // если символ оператор, то оператор
    else if (current == '+' || current == '-' || current == '*' || current == '/' || current == '^') {
        pos++;
        return token(tokenType::OPERATOR, std::string(1, current));
    }

    // если символ скобка, 
    else if (current == '(' || current == ')') {
        pos++;
        return token(tokenType::PARENTHESIS, std::string(1, current));
    }

    else if (current == '=') {
        pos++;
        return token(tokenType::EQUSL, std::string(1, current));
    }

    // если ничего не нашли, то возвращаем неизвестный токен
    return token(tokenType::UNKNOWN, std::string(1, current));
}

// пропустить пробелы
void Lexer::skipSpace() {
    while (pos < input.length() && isspace(input.at(pos))) {
        pos++;
    }
}

//пытаемся в переменную
token Lexer::tokenVariable() {
    size_t startPos = pos;
    while (pos < input.length() && (isalnum(input.at(pos)) || input.at(pos) == '_')) {
        pos++;
    }
    return token(tokenType::VARIABLE, input.substr(startPos, pos - startPos));
}

//пытаемся в числа
token Lexer::tokenNumber() {
    size_t startPos = pos;
    if (input.at(pos) == '0') {
        pos++;

        if (pos < input.length() && input.at(pos) == '0') {
            throw std::logic_error("wrong a number");
        }

        if (pos < input.length() && input.at(pos) == '.') {
            pos += 1;
            while (pos < input.length() && isdigit(input.at(pos))) {
                pos++;
            }
        }
    }
    else if (pos < input.length() && input.at(pos) != '0') {
        while (pos < input.length() && isdigit(input.at(pos))) {
            pos++;
        }
        if (pos < input.length() && input.at(pos) == '.') {
            pos++;
            while (pos < input.length() && isdigit(input.at(pos))) {
                pos++;
            }
        }
    }

    else {
        throw std::logic_error("wrong a number");
    }

    return token(tokenType::NUMBER, input.substr(startPos, pos - startPos));
}

void Lexer::setVariableType() { //уточняем токены слов
    for (token& token : result) {
        if (token.type == tokenType::VARIABLE) {

            if (func.count(token.value)) {
                token.type = tokenType::FUNCTION;
            }

            else if (con.count(token.value)) {
                token.type = tokenType::CONST;
            }

        }

    }

}
