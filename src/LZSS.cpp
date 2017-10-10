#include "Dictionary.hpp"
#include "Logger.hpp"
#include "LZSS.hpp"

#include <tuple>
#include <utility>

struct ExtendedByte {
	bool matchInDictionary;
	char compositeValue;
};

namespace encode {
	std::pair<std::vector<ExtendedByte>, int> prepareData(const std::vector<char> &input) {
		Dictionary dict { DICTIONARY_SIZE, input.front() };
		std::vector<ExtendedByte> output;
		output.push_back( { false, input.front() });
		auto size = 0;
		// TODO: add current size counting

		for (auto i = 0u; i < input.size(); ++i) {
			DEBUG(dict.print());
			auto start = 0;
			auto length = 0;
			std::tie(start, length) = dict.findMatch(input, i, i + BUFFER_SIZE);

			// pattern in dictionary matched
			if (length) {
				DEBUG(std::cout << "match =" << start << " " << length << "\n");

				output.push_back(
						{ false, static_cast<char>((start << 4) | length) });
				for (auto j = 0; j < length; ++j) {
					dict.shiftOneLeft();
					dict.insertBack(input[i + j]);
				}
				i += length - 1;
				continue;
			}

			// do not match current dictionary
			DEBUG(std::cout << "new = " << input[i] << "\n");
			output.push_back( { true, input[i] });
			dict.shiftOneLeft();
			dict.insertBack(input[i]);
		}

		return {output, size};
	}

	std::vector<char> toBinary(const std::vector<ExtendedByte> data, int size) {
		//TODO: implement
		return {};
	}
}

std::vector<char> LZSS::encode(const std::vector<char> &input) const {
	DEBUG(std::cout << "encode()\n");
	if (input.empty()) return {};

	std::vector<ExtendedByte> data;
	auto size = 0;
	std::tie(data, size) = encode::prepareData(input);
	return encode::toBinary(data, size);
}

std::vector<char> LZSS::decode(const std::vector<char> &compressed) const {
	DEBUG(std::cout << "decode()\n");
	return {};
}
