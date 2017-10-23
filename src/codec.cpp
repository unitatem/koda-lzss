#include "Logger.hpp"
#include "LZSS.hpp"

int main() {
    DEBUG(std::cout << "Start\n");

    LZSS codec;

    // wikipedia test case, https://pl.wikipedia.org/wiki/LZSS
    std::vector<char> source = {'a', 'a', 'b', 'b', 'c', 'a', 'b', 'b', 'c',
                                'a', 'b', 'd'};
    printVector("Source", source);
    auto comp = codec.encode(source);
    printVectorAsBits("Encoded", comp);
    //jezeli odnalazl to od zera zaczyna
    /*
    01100001 -> a
    0 00 10    -> 0 0 2
    0 01100010 -> 0 b
    0 00100    -> 0 1 0
    110100110 -> 1 ¦ ???
     001100100000100001100100
     */
    //a
    //0 0 2
    //
    auto dec = codec.decode(comp);
    printVector("Decoded", dec);

    DEBUG(std::cout << "End\n");
    return 0;
}
