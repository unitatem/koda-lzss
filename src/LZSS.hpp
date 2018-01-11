#ifndef LZSS_HPP
#define LZSS_HPP

#include <tuple>
#include <vector>

#define DICTIONARY_SIZE_BITS   5
#define DICTIONARY_SIZE        (1 << DICTIONARY_SIZE_BITS)

#define DICTIONARY_MATCH_LENGTH_BITS    3
#define DICTIONARY_MATCH_LENGTH         (1 << DICTIONARY_MATCH_LENGTH_BITS)

#define WINDOW_SIZE            DICTIONARY_SIZE

class LZSS {
public:
    std::tuple<std::vector<unsigned char>, int> encode(const std::vector<unsigned char> &input) const;

    std::vector<unsigned char> decode(const std::vector<unsigned char> &compressed, int size) const;
};

#endif
