#include "Logger.hpp"
#include "LZSS.hpp"

#include <iostream>

int main() {
	std::cout << "Start\n";

	LZSS codec;

	std::vector<char> source = {'a', 'b', 'c', 'd', 'e'};
	printVector("Source", source);
	auto comp = codec.encode(source);
	printVector("Compressed", comp);
	auto dec = codec.decode(comp);
	printVector("Decoded", dec);

	std::cout << "End\n";
	return 0;
}
