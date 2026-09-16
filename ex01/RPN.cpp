#include "RPN.hpp"

RPN::RPN()
{
}

RPN::RPN(const RPN& other)
{
    *this = other;
}

RPN& RPN::operator=(const RPN& other)
{
    if (this != &other)
        _stack = other._stack;
    return (*this);
}

RPN::~RPN()
{
}

bool RPN::isOperator(const std::string& token) const
{
    if (token.length() != 1)
        return (false);

    return (token[0] == '+'
        || token[0] == '-'
        || token[0] == '*'
        || token[0] == '/');
}

void RPN::calculate(char op)
{
    if (_stack.size() < 2)
        throw std::runtime_error("Invalid expression");

    int b = _stack.top();
    _stack.pop();

    int a = _stack.top();
    _stack.pop();

    if (op == '+')
        _stack.push(a + b);
    else if (op == '-')
        _stack.push(a - b);
    else if (op == '*')
        _stack.push(a * b);
    else if (op == '/')
    {
        if (b == 0)
            throw std::runtime_error("Division by zero");
        _stack.push(a / b);
    }
}

int RPN::evaluate(const std::string& expression)
{
    std::stringstream ss(expression);
    std::string token;

    // Reads each token and either pushes an operand or applies an operator
    while (ss >> token)
    {
        if (isOperator(token))
        {
            calculate(token[0]);
        }
        else
        {
            // Operands must be single digits from 0 to 9
            if (token.length() != 1 || !std::isdigit(token[0]))
                throw std::runtime_error("Invalid token");

            // Converts the digit character to an integer and pushes it onto the stack
            _stack.push(token[0] - '0');
        }
    }

    // A valid RPN expression must leave exactly one result on the stack
    if (_stack.size() != 1)
        throw std::runtime_error("Invalid expression");

    return (_stack.top());
}