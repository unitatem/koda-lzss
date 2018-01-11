#ifndef DICTIONARY_HPP
#define DICTIONARY_HPP

#include <vector>

class Dictionary {
public:
    Dictionary(int size, int max_match_length, unsigned char firstElement);

    void insertFromBack(unsigned char element, int distToBack);

    void shiftLeft(int step);

    std::pair<int, int> findMatch(const std::vector<unsigned char> &data, int begin, int end);

    void print();

    unsigned char getCharAtGivenIdx(int i);

private:
    std::vector<unsigned char> dictionary;
    int maxMatchLength;
};

#endif
