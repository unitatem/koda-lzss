#ifndef LZSS_HPP
#define LZSS_HPP

#include <vector>

constexpr int DICTIONARY_SIZE = 8;
constexpr int BUFFER_SIZE = 5;

class LZSS {
public:
	std::vector<char> encode(const std::vector<char> &input) const;
	std::vector<char> decode(const std::vector<char> &compressed) const;
};

#endif
