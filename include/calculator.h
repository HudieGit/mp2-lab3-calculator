#pragma once
#include <cctype>
#include <sstream>
#include <iostream>
#include <cstdlib>
#include "structs.h"
#include "translator.h"

using namespace std;

class expression {
private:

	double value = NOTHING;

	bool isError = false;
	string expr;
	string simpleExpr = "";
	
	unordered_map<string, double> variables; //все переменные
	typeExpression type;


public:

	expression() {}

	expression(string& e) {
		newExpr(e);
	}

	void newExpr() {
		std::getline(std::cin, expr);
		translator t;
		//обработка строки и возможных ошибок
		try {
			value = t.logicNode(expr, variables, simpleExpr, value);
			isError = false;
		}
		catch (...) {
			isError = true;
			type = typeExpression::ERROR;
			cout << "incorrect expression" << endl;
		}
	}

	void newExpr(string& e) {
		expr = e;
		translator t;
		//обработка строки и возможных ошибок
		try {
			isError = false;
			value = t.logicNode(expr, variables, simpleExpr, value);
		}

		catch (...) {
			isError = true;
			cout << "incorrect expression" << endl;
		}

	}

	void setter(double set) { //под вопросом
		value = set;
	}

	double getter() {
		return value;
	}

	void outputVariables() {
		for (const auto& var : variables) {
			std::cout << var.first << std::endl;
		}
	}

	bool checkError() {
		return isError;
	}
};

class screen {
private:

	//очистка экрана дл€ всех систем(в надежде, что это работает)
	void clear_screen() {
		// ƒл€ Windows
#ifdef _WIN32
		std::system("cls");
		// ƒл€ Linux/macOS
#else
		std::system("clear");
#endif
	}

public:

	void counting() {

		std::string input;
		expression expr;
		do {
			std::getline(std::cin, input);
			expr.newExpr(input);
			if (input == "stop") { continue; }

			if (expr.getter() != NOTHING && !expr.checkError()) {
				std::cout << "result: " << expr.getter() << endl;
			}

			else {
			}
			//clear_screen();
			//очистка экрана


			//вывод последнего результата
			//перемещение курсора
			//std::cout << "\033[0;0H";


		} while (input != "stop");
	}

};

