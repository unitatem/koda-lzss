#ifndef LZSS_HPP
#define LZSS_HPP

#include <vector>

#define DICTIONARY_SIZE        4
#define BUFFER_SIZE            4

class LZSS {
public:
    std::vector<char> encode(const std::vector<char> &input) const;

    std::vector<char> decode(const std::vector<char> &compressed) const;
};

#endif
