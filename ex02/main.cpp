#include "PmergeMe.hpp"

int main(int argc, char **argv)
{
    if (argc < 2)
    {
        std::cerr << "Error" << std::endl;
        return (1);
    }

    try
    {
        PmergeMe sorter;

        sorter.parseInput(argc, argv);
        sorter.printBefore();
        sorter.sort();
        sorter.printAfter();
    }
    catch (const std::exception&)
    {
        std::cerr << "Error" << std::endl;
        return (1);
    }


    return (0);
}