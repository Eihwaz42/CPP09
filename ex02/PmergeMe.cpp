#include "PmergeMe.hpp"

PmergeMe::PmergeMe()
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
    }
    return (*this);
}

PmergeMe::~PmergeMe()
{
}

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

    if (!(ss >> value) || value > INT_MAX)
        throw std::runtime_error("Error");

    return (static_cast<int>(value));
}

void PmergeMe::parseInput(int argc, char **argv)
{
    for (int i = 1; i < argc; i++)
    {
        int value = parseNumber(argv[i]);

        _vector.push_back(value);
        _deque.push_back(value);
    }
}

void PmergeMe::printBefore() const
{
    std::cout << "Before: ";

    for (size_t i = 0; i < _vector.size(); i++)
    {
        if (i != 0)
            std::cout << " ";
        std::cout << _vector[i];
    }

    std::cout << std::endl;
}

void PmergeMe::printAfter() const
{
    printContainer(_vector, "After:  ");
}

void PmergeMe::sortVector()
{
    std::vector<Item> items;

    for (size_t i = 0; i < _vector.size(); i++)
        items.push_back(Item(_vector[i], i));

    fordJohnsonVector(items);

    for (size_t i = 0; i < items.size(); i++)
        _vector[i] = items[i].value;
}

void PmergeMe::sort()
{
    sortVector();
}

Pair PmergeMe::findPair(const std::vector<Pair>& pairs, size_t id) const
{
    for (size_t i = 0; i < pairs.size(); i++)
    {
        if (pairs[i].larger.id == id)
            return (pairs[i]);
    }

    throw std::runtime_error("Error");
}

void PmergeMe::fordJohnsonVector(std::vector<Item>& items)
{
    if (items.size() <= 1)
        return;

    bool hasOdd = (items.size() % 2 != 0);
    Item odd;

    if (hasOdd)
        odd = items.back();

    std::vector<Pair> pairs;

    for (size_t i = 0; i + 1 < items.size(); i += 2)
    {
        if (items[i].value >= items[i + 1].value)
            pairs.push_back(Pair(items[i], items[i + 1]));
        else
            pairs.push_back(Pair(items[i + 1], items[i]));
    }

    std::vector<Item> winners;

    for (size_t i = 0; i < pairs.size(); i++)
        winners.push_back(pairs[i].larger);

    fordJohnsonVector(winners);

    std::vector<Pair> orderedPairs;

    for (size_t i = 0; i < winners.size(); i++)
        orderedPairs.push_back(findPair(pairs, winners[i].id));

    std::vector<Item> mainChain;
    mainChain.push_back(orderedPairs[0].smaller);

    for (size_t i = 0; i < winners.size(); i++)
        mainChain.push_back(winners[i]);

    std::vector<Pending> pending;

    for (size_t i = 1; i < orderedPairs.size(); i++)
    {
        pending.push_back(Pending(
            orderedPairs[i].smaller,
            orderedPairs[i].larger.id,
            true
        ));
    }

    if (hasOdd)
        pending.push_back(Pending(odd, 0, false));

    std::vector<size_t> order;
    order = generateJacobsthalOrder(pending.size());

    for (size_t i = 0; i < order.size(); i++)
    {
        Pending& current = pending[order[i]];

        size_t end;

        if (current.hasPartner)
            end = findPartner(mainChain, current.partnerId);
        else
            end = mainChain.size();

        size_t position = binarySearch(mainChain, current.item, end);

        mainChain.insert(mainChain.begin() + position, current.item);
    }

    items = mainChain;  
}

std::vector<size_t>
PmergeMe::generateJacobsthalOrder(size_t pendingCount) const
{
    std::vector<size_t> order;

    if (pendingCount == 0)
        return (order);

    size_t totalB = pendingCount + 1;
    size_t previous = 1;
    size_t jacobPrev = 1;
    size_t jacob = 3;

    while (previous < totalB)
    {
        size_t end = jacob;

        if (end > totalB)
            end = totalB;

        for (size_t b = end; b > previous; b--)
            order.push_back(b - 2);

        previous = end;

        size_t next = jacob + 2 * jacobPrev;
        jacobPrev = jacob;
        jacob = next;
    }

    return (order);
}

size_t PmergeMe::findPartner(const std::vector<Item>& chain,
    size_t id) const
{
    for (size_t i = 0; i < chain.size(); i++)
    {
        if (chain[i].id == id)
            return (i);
    }
    throw std::runtime_error("Error");
}

size_t PmergeMe::binarySearch(const std::vector<Item>& chain,
    const Item& item, size_t end) const
{
    size_t left = 0;
    size_t right = end;

    while (left < right)
    {
        size_t middle = left + (right - left) / 2;

        if (chain[middle].value < item.value)
            left = middle + 1;
        else
            right = middle;
    }
    return (left);
}
