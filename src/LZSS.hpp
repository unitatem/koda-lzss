#ifndef LZSS_HPP
#define LZSS_HPP

#include <vector>

#define DICTIONARY_SIZE        4
#define BUFFER_SIZE            4
#define LENGTH_OFFSET         1

class LZSS {
public:
    std::tuple<std::vector<unsigned char>, int> encode(const std::vector<unsigned char> &input) const;

    std::vector<unsigned char> decode(const std::vector<unsigned char> &compressed, int size) const;
};

#endif
