#include "PmergeMe.hpp"

// CONSTRUCTORS / DESTRUCTOR / ASSIGNMENT

PmergeMe::PmergeMe() : _vectorTime(0), _dequeTime(0)
{
}

PmergeMe::PmergeMe(const PmergeMe& other)
{
    *this = other;
}

PmergeMe& PmergeMe::operator=(const PmergeMe& other)
{
    if (this != &other)
    {
        _vector = other._vector;
        _deque = other._deque;
        _vectorTime = other._vectorTime;
        _dequeTime = other._dequeTime;
    }
    return (*this);
}

PmergeMe::~PmergeMe()
{
}


// PARSING

int PmergeMe::parseNumber(const std::string& str) const
{
    if (str.empty())
        throw std::runtime_error("Error");

    for (size_t i = 0; i < str.length(); i++)
    {
        if (str[i] < '0' || str[i] > '9')
            throw std::runtime_error("Error");
    }

    std::stringstream ss(str);
    long value;

    if (!(ss >> value) || value <= 0 || value > INT_MAX)
        throw std::runtime_error("Error");

    return (static_cast<int>(value));
}

void PmergeMe::parseInput(int argc, char **argv)
{
    for (int i = 1; i < argc; i++)
        parseNumber(argv[i]);
}

// TIMING

double PmergeMe::getTime() const
{
    struct timeval time;

    gettimeofday(&time, NULL);
    return (time.tv_sec * 1000000.0 + time.tv_usec);
}

void PmergeMe::sort(int argc, char **argv)
{
    double start;

    start = getTime();
    sortVector(argc, argv);
    _vectorTime = getTime() - start;

    start = getTime();
    sortDeque(argc, argv);
    _dequeTime = getTime() - start;

    // Verify that both implementations produced the same sorted sequence
    if (_vector.size() != _deque.size())
        throw std::runtime_error("Error");

    for (size_t i = 0; i < _vector.size(); i++)
    {
        if (_vector[i] != _deque[i])
            throw std::runtime_error("Error");
    }
}


// DISPLAY

void PmergeMe::printBefore(int argc, char **argv) const
{
    std::cout << "Before: ";

    for (int i = 1; i < argc; i++)
    {
        if (i != 1)
            std::cout << " ";
        std::cout << argv[i];
    }

    std::cout << std::endl;
}

void PmergeMe::printAfter() const
{
    printContainer(_vector, "After:  ");
}

void PmergeMe::printTimes() const
{
    std::cout << std::fixed << std::setprecision(0);

    std::cout << "Time to process a range of "
        << _vector.size()
        << " elements with std::vector : "
        << _vectorTime << " us" << std::endl;

    std::cout << "Time to process a range of "
        << _deque.size()
        << " elements with std::deque  : "
        << _dequeTime << " us" << std::endl;
}