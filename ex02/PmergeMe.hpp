#ifndef PMERGEME_HPP
#define PMERGEME_HPP

#include <iostream>
#include <vector>
#include <deque>
#include <string>
#include <sstream>
#include <stdexcept>
#include <climits>
#include <cstdlib>

struct Item
{
    int value;
    size_t id;

    Item() : value(0), id(0) {}
    Item(int v, size_t i) : value(v), id(i) {}
};

struct Pair
{
    Item larger;
    Item smaller;

    Pair(const Item& l, const Item& s)
        : larger(l), smaller(s) {}
};

struct Pending
{
    Item item;
    size_t partnerId;
    bool hasPartner;

    Pending(const Item& i, size_t id, bool has)
        : item(i), partnerId(id), hasPartner(has) {}
};

class PmergeMe
{
    private:
        std::vector<int> _vector;
        std::deque<int> _deque;

        int parseNumber(const std::string& str) const;
        void fordJohnsonVector(std::vector<Item>& items);
        void sortVector();
        Pair findPair(const std::vector<Pair>& pairs, size_t id) const;
        std::vector<size_t> generateJacobsthalOrder(size_t pendingCount) const;
        size_t findPartner(const std::vector<Item>& chain, size_t id) const;
        size_t binarySearch(const std::vector<Item>& chain, const Item& item, size_t end) const;

        template <typename T>
        void printContainer(const T& container,
            const std::string& label) const
        {
            std::cout << label;

            for (typename T::const_iterator it = container.begin();
                it != container.end(); ++it)
            {
                if (it != container.begin())
                    std::cout << " ";
                std::cout << *it;
            }
            std::cout << std::endl;
        }

    public:
        PmergeMe();
        PmergeMe(const PmergeMe& other);
        PmergeMe& operator=(const PmergeMe& other);
        ~PmergeMe();

        void parseInput(int argc, char **argv);
        void printBefore() const;
        void printAfter() const;
        void sort();
};

#endif