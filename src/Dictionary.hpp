#ifndef DICTIONARY_HPP
#define DICTIONARY_HPP

#include <vector>

class Dictionary {
public:
    Dictionary(int size, unsigned char firstElement);

    void insertBack(unsigned char element);

    void shiftOneLeft();

    std::pair<int, int> findMatch(const std::vector<unsigned char> &data, int begin, int end);

    void print();

    char getCharAtGivenIdx(int i);

private:
    std::vector<unsigned char> dictionary;
};

#endif
