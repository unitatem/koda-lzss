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
    //jezeli odnalazl to od zera zaczyna
    printVectorAsBits("Encoded", comp);
    /*
    01100001 -> a
    0 00 01    -> 0 0 1+1
    1 01100010 -> 1 b
    0 11 00    -> 0 3 0+1
    1 01100011 -> 1 c
    0 00 11    -> 0 0 3+1
    0 00 01    -> 0 0 1+1
    1 00110010 -> 1 (d to 01100100  wiec tak jakby ostatnia litera nie weszla do konca, w sensie ostatnia cyfra jest znaczaca a ta druga w tej linii do olania)
    0
     */
    auto dec = codec.decode(comp);
    printVector("Decoded", dec);


    DEBUG(std::cout << "End\n");
    return 0;
}
