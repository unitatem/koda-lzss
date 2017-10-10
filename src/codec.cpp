#include "Logger.hpp"
#include "LZSS.hpp"

int main() {
	DEBUG(std::cout << "Start\n");

	LZSS codec;

	// wikipedia test case
	std::vector<char> source = { 'a', 'a', 'b', 'b', 'c', 'a', 'b', 'b', 'c',
			'a', 'b', 'd' };
	printVector("Source", source);
	auto comp = codec.encode(source);
	std::cout << "Compressed\n";
	auto dec = codec.decode(comp);
	printVector("Decoded", dec);

	DEBUG(std::cout << "End\n");
	return 0;
}
