#ifndef LZSS_HPP
#define LZSS_HPP

#include <tuple>
#include <vector>

#define DICTIONARY_SIZE_BITS   10
#define DICTIONARY_SIZE        (1 << DICTIONARY_SIZE_BITS)

#define DICTIONARY_MATCH_LENGTH_BITS    3
#define DICTIONARY_MATCH_LENGTH         (1 << DICTIONARY_MATCH_LENGTH_BITS)

#define WINDOW_SIZE            DICTIONARY_SIZE

struct EncodedDataParams {
    std::vector<unsigned char> data;
    unsigned int bitSize;
    double avgByteLength;
};

class LZSS {
public:
    EncodedDataParams encode(const std::vector<unsigned char> &input) const;

    std::vector<unsigned char> decode(const std::vector<unsigned char> &compressed, int size) const;
};

#endif
