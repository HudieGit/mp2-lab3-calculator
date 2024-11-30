#include "structs.h"

using namespace std;



class Lexer {
public:
    vector<token> makeTokens(const std::string& inputStr) {
        input = inputStr;
        pos = 0;

        token token = nextToken();
        while (token.type != tokenType::END) {
            if (token.type == tokenType::UNKNOWN) throw 1;
            result.push_back(token);
            token = nextToken();
        }

        setVariableType();

        return result;
    }

private:
    std::unordered_set<std::string> con = { "pi", "e" };
    std::unordered_set<std::string> func = { "sin", "cos", "exp", "ln" };
    vector<token> result;
    std::string input;
    size_t pos;

    token nextToken() {
        skipSpace();

        if (pos >= input.length()) return token(tokenType::END, "");

        char current = input.at(pos);

        // если символ буква, то пытаемся собрать переменную
        if (isalpha(current)) {
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
    void skipSpace() {
        while (pos < input.length() && isspace(input.at(pos))) {
            pos++;
        }
    }

    //пытаемся в переменную
    token tokenVariable() {
        size_t startPos = pos;
        while (pos < input.length() && (isalnum(input.at(pos)) || input.at(pos) == '_')) {
            pos++;
        }
        return token(tokenType::VARIABLE, input.substr(startPos, pos - startPos));
    }

    //пытаемся в числа
    token tokenNumber() {
        size_t startPos = pos;

        if (pos + 1 < input.length() && input.at(pos) == '0' && input.at(pos + 1) == '.') {
            pos += 2;
            while (pos < input.length() && isdigit(input.at(pos))) {
                pos++;
            }
        }
        else {
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

        return token(tokenType::NUMBER, input.substr(startPos, pos - startPos));
    }
    
    void setVariableType() { //уточняем токены слов
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

};