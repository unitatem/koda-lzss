#include "Logger.hpp"
#include "LZSS.hpp"

#include <tuple>
#include <utility>

struct CompressedByte {
	bool addToDictionary;
	char compositePtr;
};

namespace {
	void shiftDictionaryOneLeft(std::vector<char> &dict) {
		for (auto i = 0; i < DICTIONARY_SIZE - 1; ++i)
			dict[i] = dict[i + 1];
	}

	// TODO: profile, if too slow refactor with use of moving hash
	std::pair<int, int> findInDictionary(const std::vector<char> &input,
			int inputFrameBeginIdx, std::vector<char> dict) {
		auto maxLength = 0;
		auto idxOfBest = 0;

		auto length = 0;
		auto idx = 0;
		// O(n^2), calculated similarly to convolution, one shifts in reference to other
		for (auto i = 0; i < DICTIONARY_SIZE; ++i)
			for (auto s = 0; s < BUFFER_SIZE - i; ++s)
				if (input[inputFrameBeginIdx + length] == dict[i + s]) {
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
}

std::vector<char> LZSS::encode(const std::vector<char> &input) const {
	DEBUG(std::cout << "encode()\n");
	if (input.empty()) return {};

	// TODO: add current size counter
	std::vector<char> dict(DICTIONARY_SIZE, input.front());
	std::vector<CompressedByte> output;
	output.push_back( { false, input.front() });

	for (auto i = 0u; i < input.size(); ++i) {
		DEBUG(printVector("dict", dict));
		auto start = 0;
		auto length = 0;
		std::tie(start, length) = findInDictionary(input, i, dict);

		// pattern in dictionary matched
		if (length) {
			DEBUG(std::cout << "match = " << start << " " << length << "\n");

			output.push_back(
					{ false, static_cast<char>((start << 4) | length) });
			for (auto j = 0; j < length; ++j) {
				shiftDictionaryOneLeft(dict);
				dict[DICTIONARY_SIZE - 1] = input[i + j];
			}
			i += length - 1;
			continue;
		}

		// do not match current dictionary
		DEBUG(std::cout << "new = " << input[i] << "\n");
		output.push_back( { true, input[i] });
		shiftDictionaryOneLeft(dict);
		dict[DICTIONARY_SIZE - 1] = input[i];
	}

	std::cout << "Output =\n";
	for (auto i = 0u; i < output.size(); ++i)
		std::cout << output[i].addToDictionary << " " << output[i].compositePtr
				<< "\n";

	return {};
}

std::vector<char> LZSS::decode(const std::vector<char> &compressed) const {
	DEBUG(std::cout << "decode()\n");
	return {};
}
