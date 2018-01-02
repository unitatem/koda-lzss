#ifndef LZSS_HPP
#define LZSS_HPP

#include <tuple>
#include <vector>

#define DICTIONARY_SIZE        10000
#define WINDOW_SIZE            DICTIONARY_SIZE

class LZSS {
public:
    std::tuple<std::vector<unsigned char>, int> encode(const std::vector<unsigned char> &input) const;

    std::vector<unsigned char> decode(const std::vector<unsigned char> &compressed, int size) const;
};

#endif
