#include "Dictionary.hpp"
#include "Logger.hpp"

#include <algorithm>

Dictionary::Dictionary(int size, int max_match_length, unsigned char firstElement)
        : dictionary(size, firstElement), maxMatchLength(max_match_length) {}

void Dictionary::insertFromBack(unsigned char element, int distToBack) {
    dictionary[dictionary.size() - distToBack] = element;
}

void Dictionary::shiftLeft(int step) {
    const auto end = dictionary.size() - step;
    const auto data = dictionary.data();
    for (auto i = 0u; i < end; ++i)
        *(data + i) = *(data + i + step);
}

unsigned char Dictionary::getCharAtGivenIdx(int i){
    return dictionary[i];
}

// TODO: profile, if too slow refactor with use of moving hash
std::pair<int, int> Dictionary::findMatch(const std::vector<unsigned char> &data,
                                          int begin, int end) {
    auto beginIterator = std::begin(data) + begin;
    auto tryMoreIterator = std::begin(data) + begin + 1;
    const auto endIterator = std::begin(data) + end;

    auto idxOfBest = 0;
    auto maxLength = 0;
    bool repeat;
    do {
        auto it = std::search(dictionary.begin(), dictionary.end(), beginIterator, tryMoreIterator);
        repeat = it != dictionary.end();
        if (repeat) {
            idxOfBest = static_cast<int>(it - dictionary.begin());
            maxLength = static_cast<int>(tryMoreIterator - beginIterator);
            ++tryMoreIterator;
        }
    } while (repeat && tryMoreIterator < endIterator && maxLength <= maxMatchLength);

    return {idxOfBest, maxLength};
}

void Dictionary::print() {
    printVector("dict", dictionary);
}
