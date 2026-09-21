#include "PmergeMe.hpp"

// Generate the insertion order of pending elements using Jacobsthal
// boundaries. b1 is already in the main chain, so pending[0] is b2.
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

// VECTOR HELPERS

Pair PmergeMe::findPair(const std::vector<Pair>& pairs,
    size_t id) const
{
    for (size_t i = 0; i < pairs.size(); i++)
    {
        if (pairs[i].larger.id == id)
            return (pairs[i]);
    }

    throw std::runtime_error("Error");
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


// VECTOR FORD-JOHNSON

void PmergeMe::fordJohnsonVector(std::vector<Item>& items)
{
    if (items.size() <= 1)
        return;

    // If the number of elements is odd, keep the last one aside
    bool hasOdd = (items.size() % 2 != 0);
    Item odd;

    if (hasOdd)
        odd = items.back();

    // Create pairs and store the larger element as the winner
    std::vector<Pair> pairs;

    for (size_t i = 0; i + 1 < items.size(); i += 2)
    {
        if (items[i].value >= items[i + 1].value)
            pairs.push_back(Pair(items[i], items[i + 1]));
        else
            pairs.push_back(Pair(items[i + 1], items[i]));
    }

    // Recursively sort the winners of each pair
    std::vector<Item> winners;

    for (size_t i = 0; i < pairs.size(); i++)
        winners.push_back(pairs[i].larger);

    fordJohnsonVector(winners);

    // Rebuild the pairs in winner order while preserving each association
    std::vector<Pair> orderedPairs;

    for (size_t i = 0; i < winners.size(); i++)
        orderedPairs.push_back(findPair(pairs, winners[i].id));

    // b1 is inserted first, followed by all sorted winners (a1, a2, ...)
    std::vector<Item> mainChain;

    mainChain.push_back(orderedPairs[0].smaller);

    for (size_t i = 0; i < winners.size(); i++)
        mainChain.push_back(winners[i]);

    // Store b2, b3, ... with the id of their associated winner
    std::vector<Pending> pending;

    for (size_t i = 1; i < orderedPairs.size(); i++)
    {
        pending.push_back(Pending(
            orderedPairs[i].smaller,
            orderedPairs[i].larger.id,
            true
        ));
    }

    // The odd element has no partner, so it can use the full search range
    if (hasOdd)
        pending.push_back(Pending(odd, 0, false));

    std::vector<size_t> order;
    order = generateJacobsthalOrder(pending.size());

    // Insert pending elements in Jacobsthal order
    // For paired elements, binary search stops before their known upper bound
    for (size_t i = 0; i < order.size(); i++)
    {
        Pending& current = pending[order[i]];
        size_t end;

        if (current.hasPartner)
            end = findPartner(mainChain, current.partnerId);
        else
            end = mainChain.size();

        size_t position = binarySearch(
            mainChain, current.item, end);

        mainChain.insert(
            mainChain.begin() + position, current.item);
    }

    // Replace the input sequence with the fully sorted main chain
    items = mainChain;
}

void PmergeMe::sortVector(int argc, char **argv)
{
    std::vector<Item> items;

    for (int i = 1; i < argc; i++)
    {
        int value = parseNumber(argv[i]);

        _vector.push_back(value);
        items.push_back(Item(value, i - 1));
    }

    fordJohnsonVector(items);

    for (size_t i = 0; i < items.size(); i++)
        _vector[i] = items[i].value;
}