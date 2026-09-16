#ifndef RPN_HPP
#define RPN_HPP

#include <iostream>
#include <sstream>
#include <string>
#include <stack>
#include <stdexcept>

class RPN
{
    private:
        std::stack<int> _stack;

        bool isOperator(const std::string& token) const;
        void calculate(char op);

    public:
        RPN();
        RPN(const RPN& other);
        RPN& operator=(const RPN& other);
        ~RPN();

        int evaluate(const std::string& expression);
};

#endif