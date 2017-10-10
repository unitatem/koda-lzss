#include "Dictionary.hpp"
#include "Logger.hpp"

Dictionary::Dictionary(int size, char firstElement)
	: dictionary(size, firstElement)
{}

void Dictionary::insertBack(char element) {
	dictionary[dictionary.size() - 1] = element;
}

void Dictionary::shiftOneLeft() {
	for (auto i = 0u; i < dictionary.size() - 1; ++i)
		dictionary[i] = dictionary[i + 1];
}

// TODO: profile, if too slow refactor with use of moving hash
std::pair<int, int> Dictionary::findMatch(const std::vector<char> &data,
		int begin, int end) {
	auto maxLength = 0;
	auto idxOfBest = 0;

	auto length = 0;
	auto idx = 0;
	// O(n^2), calculated similarly to convolution, one shifts in reference to other
	for (auto i = 0u; i < dictionary.size(); ++i)
		for (auto s = 0u; s < end - begin - i; ++s)
			if (data[begin + length] == dictionary[i + s]) {
				if (length == 0) idx = i + s;
				++length;
			} else {
				if (length > maxLength) {
					maxLength = length;
					idxOfBest = idx;
				}
				length = 0;
			}

	return {idxOfBest, maxLength};
}

void Dictionary::print() {
	printVector("dict", dictionary);
}
