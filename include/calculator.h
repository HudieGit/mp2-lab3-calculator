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

	string expr;
	string simpleExpr = "";
	
	unordered_map<string, double> variables; //��� ����������
	typeExpression type;


public:

	expression(string& e) {
		newExpr(e);
	}

	void newExpr() {
		std::getline(std::cin, expr);
		translator t;
		//��������� ������ � ��������� ������
		try {
			value = t.logicNode(expr, variables, simpleExpr, value);
		}
		catch (...) {
			type = typeExpression::ERROR;
			cout << "incorrect expression" << endl;
		}
	}

	void newExpr(string& e) {
		expr = e;
		translator t;
		//��������� ������ � ��������� ������
		try {
			value = t.logicNode(expr, variables, simpleExpr, value);
		}

		catch (...) {
			cout << "incorrect expression" << endl;
		}

	}

	void setter(double set) { //��� ��������
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
};

class screen {
private:

	//������� ������ ��� ���� ������(� �������, ��� ��� ��������)
	void clear_screen() {
		// ��� Windows
#ifdef _WIN32
		std::system("cls");
		// ��� Linux/macOS
#else
		std::system("clear");
#endif
	}

public:

	void counting() {
		std::string input;
		expression expr(input);
		do {


			expr.newExpr();
			//clear_screen();
			//������� ������


			//����� ���������� ����������
			if (expr.getter() != NOTHING) {
				std::cout << "result: " << expr.getter() << endl;
			}

			//����������� �������
			//std::cout << "\033[0;0H";

		} while (input != "stop");
	}

};

