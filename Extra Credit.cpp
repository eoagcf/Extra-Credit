#include <iostream>
#include <stack>
#include <cctype>
#include <cmath>
#include <string>
#include <stdexcept>
#include <unordered_map>

class Evaluator {
public:
    int eval(const std::string& expression) {
        std::string postfix = infixToPostfix(expression);
        return evaluatePostfix(postfix);
    }

private:
    // Operator precedence and associativity
    std::unordered_map<char, int> precedence = {
        {'!', 8}, {'+', 5}, {'-', 5}, {'*', 6}, {'/', 6}, {'%', 6},
        {'^', 7}, {'>', 4}, {'<', 4}, {'=', 3}, {'&', 2}, {'|', 1}
    };
    std::unordered_map<char, bool> rightAssociative = { {'^', true}, {'!', true} };

    // Convert infix to postfix
    std::string infixToPostfix(const std::string& expression) {
        std::string postfix;
        std::stack<char> operators;
        int n = expression.size();
        bool lastWasOperand = false; // To detect invalid sequences

        for (int i = 0; i < n; i++) {
            char ch = expression[i];
            if (std::isspace(ch)) continue; // Ignore whitespace

            if (std::isdigit(ch)) {
                postfix += ch; // Append operand
                lastWasOperand = true;
            }
            else if (isOperator(ch)) {
                if (!lastWasOperand && (ch != '-' && ch != '!')) {
                    throw std::runtime_error("Expression can't start with a binary operator @ char: " + std::to_string(i));
                }

                while (!operators.empty() && precedence[operators.top()] >= precedence[ch]) {
                    postfix += operators.top();
                    operators.pop();
                }
                operators.push(ch);
                lastWasOperand = false;
            }
            else if (ch == '(') {
                operators.push(ch);
                lastWasOperand = false;
            }
            else if (ch == ')') {
                while (!operators.empty() && operators.top() != '(') {
                    postfix += operators.top();
                    operators.pop();
                }
                if (operators.empty()) {
                    throw std::runtime_error("Mismatched parenthesis @ char: " + std::to_string(i));
                }
                operators.pop();
            }
            else {
                throw std::runtime_error("Invalid character in expression @ char: " + std::to_string(i));
            }
        }

        // Append remaining operators
        while (!operators.empty()) {
            if (operators.top() == '(') {
                throw std::runtime_error("Mismatched parenthesis in expression");
            }
            postfix += operators.top();
            operators.pop();
        }

        return postfix;
    }

    // Evaluate postfix expression
    int evaluatePostfix(const std::string& postfix) {
        std::stack<int> operands;

        for (char ch : postfix) {
            if (std::isdigit(ch)) {
                operands.push(ch - '0'); // Convert char to int
            }
            else if (isOperator(ch)) {
                if (operands.size() < 2 && ch != '!') {
                    throw std::runtime_error("Invalid operation in postfix expression");
                }
                int b = operands.top();
                operands.pop();
                int a = operands.top();
                operands.pop();

                switch (ch) {
                case '+': operands.push(a + b); break;
                case '-': operands.push(a - b); break;
                case '*': operands.push(a * b); break;
                case '/':
                    if (b == 0) throw std::runtime_error("Division by zero");
                    operands.push(a / b);
                    break;
                case '^': operands.push(std::pow(a, b)); break;
                case '>': operands.push(a > b); break;
                case '<': operands.push(a < b); break;
                case '=': operands.push(a == b); break;
                case '&': operands.push(a && b); break;
                case '|': operands.push(a || b); break;
                default: throw std::runtime_error("Unknown operator in postfix expression");
                }
            }
        }
        return operands.top();
    }

    // Check if character is an operator
    bool isOperator(char ch) {
        return precedence.find(ch) != precedence.end();
    }
};

int main() {
    Evaluator eval;
    try {
        std::cout << eval.eval("1+2*3") << std::endl;            // 7
        std::cout << eval.eval("2+2^2*3") << std::endl;         // 14
        std::cout << eval.eval("1==2") << std::endl;            // 0
        std::cout << eval.eval("1+3>2") << std::endl;           // 1
        std::cout << eval.eval("(4>=4)&&0") << std::endl;       // 0
        std::cout << eval.eval("(1+2)*3") << std::endl;         // 9
        std::cout << eval.eval("++++2-5*(3^2)") << std::endl;   // -41
    }
    catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }
    return 0;
}
