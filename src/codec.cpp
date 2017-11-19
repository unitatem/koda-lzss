#include "Logger.hpp"
#include "LZSS.hpp"
#include "MediaOperations.hpp"


#include <iostream>
#include <conio.h>
#include <fstream>
#include <vector>
#include <iterator>

int main() {
	loggerPrint("*******************Start*******************");
	loggerPrint("Dictionary size: " + std::to_string(DICTIONARY_SIZE));
	loggerPrint("Buffer size: " + std::to_string(BUFFER_SIZE));
    LZSS codec;

	//Treating images as any files
	auto loadedImage = readFile("E:\\EITI\\EITI-II.SID\\KODA\\Projekt\\koda-lzss\\image.jpg");
	//createFile(loadedImage, "E:\\EITI\\EITI-II.SID\\KODA\\Projekt\\koda-lzss\\newImage.jpg");

	auto loadedPgmImage = readPgmImage("E:\\EITI\\EITI-II.SID\\KODA\\Projekt\\Dane\\obrazy\\barbara.pgm");
	//createFile(loadedPgmImage, "E:\\EITI\\EITI-II.SID\\KODA\\Projekt\\koda-lzss\\barbara.txt");
	auto rows = loadedPgmImage.rows;
	auto cols = loadedPgmImage.cols;
	//auto maxValue = 
	auto matType = loadedPgmImage.type();
	auto imagetoEncode = getImagePixels(loadedPgmImage);
	createPgmImage(imagetoEncode, rows, cols, matType, "E:\\EITI\\EITI-II.SID\\KODA\\Projekt\\koda-lzss\\barbara.pgm");
	//printVector("Source", imagetoEncode);
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