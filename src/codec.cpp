#include "Logger.hpp"
#include "LZSS.hpp"
#include "MediaOperations.hpp"


#include <iostream>
#include <conio.h>
#include <fstream>
#include <vector>
#include <iterator>

#define IMAGES
//#define WIKI_CASE

int main() {
	loggerPrint("*******************Start*******************");
	loggerPrint("Dictionary size: " + std::to_string(DICTIONARY_SIZE));
	loggerPrint("Buffer size: " + std::to_string(BUFFER_SIZE));
    LZSS codec;

#ifdef IMAGES
	std::string imagesFolder = "E:\\EITI\\EITI-II.SID\\KODA\\Projekt\\Dane\\obrazy\\";
	std::string image = "barbara";


	auto loadedPgmImage = readPgmImage(imagesFolder + image + ".pgm");

	auto rows = loadedPgmImage.rows;
	auto cols = loadedPgmImage.cols;
	auto matType = loadedPgmImage.type();
	auto imagetoEncode = getImagePixels(loadedPgmImage); //imageData
	auto imageEncoded = codec.encode(imagetoEncode);
	createFile(imageEncoded, imagesFolder + image + "Encoded.txt");
	auto imageDecoded = codec.decode(imageEncoded);
	createPgmImage(imageDecoded, rows, cols, matType, imagesFolder + image + "Decoded.pgm");
#endif
#ifdef WIKI_CASE
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
#endif
    return 0;
}