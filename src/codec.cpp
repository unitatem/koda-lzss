#include "Logger.hpp"
#include "LZSS.hpp"

int main() {
	loggerPrint("*******************Start*******************");
	loggerPrint("Dictionary size: " + std::to_string(DICTIONARY_SIZE));
	loggerPrint("Buffer size: " + std::to_string(BUFFER_SIZE));
    LZSS codec;

    // wikipedia test case, https://pl.wikipedia.org/wiki/LZSS
    std::vector<char> source = {'a', 'a', 'b', 'b', 'c', 'a', 'b', 'b', 'c',
                                'a', 'b', 'd'};
    printVector("Source", source);
    auto comp = codec.encode(source);
    printVectorAsBits("Transmission in Bytes: ", comp);
    auto dec = codec.decode(comp);
    printVector("Decoded", dec);


	loggerPrint("*******************End*******************");
	system("pause");
    return 0;
}
