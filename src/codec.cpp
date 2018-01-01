#include "Logger.hpp"
#include "LZSS.hpp"
#include "MediaOperations.hpp"
#include "codec.h"
#include <thread>
#include <fstream>

#define DATA
//#define WIKI_CASE

int main() {
    loggerPrint("*******************Start*******************");
    loggerPrint("Dictionary size: " + std::to_string(DICTIONARY_SIZE));
    loggerPrint("Buffer size: " + std::to_string(WINDOW_SIZE));
    LZSS codec;

#ifdef DATA
	std::string dataFolder = "../dane/";
    std::string imagesFolder = dataFolder + "obrazy/";
	std::string distributionsFolder = dataFolder + "rozklady/";
	std::string outputFolder = "output/";
	std::string images[] = { "barbara" , "boat", "chronometer", "lena", "mandril" , "peppers"};
	std::string distributions[] = { "geometr_05" , "geometr_09", "geometr_099", "laplace_10", "laplace_20" , "laplace_30", "normal_10", "normal_30", "normal_50", "uniform" };

	std::vector<std::thread> threads;

	//threads.push_back(std::thread(performComputations, imagesFolder, images[0], codec, outputFolder));

	for each(std::string object in images) {
		threads.push_back(std::thread(performComputations, imagesFolder, object, codec, outputFolder));
	}

	for each(std::string object in distributions) {
		threads.push_back(std::thread(performComputations, distributionsFolder, object, codec, outputFolder));
	}

	for (int i = 0; i < threads.size(); i++) {
		threads[i].join();
	}


	
#endif
#ifdef WIKI_CASE
    // wikipedia test case, https://pl.wikipedia.org/wiki/LZSS
    std::vector<unsigned char> source = {'a', 'a', 'b', 'b', 'c', 'a', 'b', 'b', 'c', 'a', 'b', 'd'};

//    std::vector<unsigned char> source = {47, 120};

//    std::vector<unsigned char> source = {115, 142, 163, 170, 170, 169, 161, 160, 161, 160, 160, 155, 160, 161};

    printVector("Source", source);
    std::vector<unsigned char> encodedData;
    int size;
    std::tie(encodedData, size) = codec.encode(source);
    printVectorAsBits("Transmission in Bytes: ", encodedData);
    auto decodedData = codec.decode(encodedData, size);
    printVector("Decoded", decodedData);
#endif

    loggerPrint("*******************End*******************");
    return 0;
}

void performComputations(std::string &objectsFolder, std::string &object, LZSS &codec, std::string &outputFolder)
{
	std::ofstream calculationsFile;
	auto loadedPgmImage = readPgmImage(objectsFolder + object + ".pgm");
	auto rows = loadedPgmImage.rows;
	auto cols = loadedPgmImage.cols;
	assert(rows != 0 && cols != 0);
	auto matType = loadedPgmImage.type();
	std::cout << "Object loaded: " << object << "\n";

	calculationsFile.open(objectsFolder + outputFolder + object + "_calc.txt", std::ios_base::out);

	auto imageToEncode = getImagePixels(loadedPgmImage);
	auto histogram = calculateHistogram(imageToEncode, 256);
	double entropy = calculateEntropy(imageToEncode, histogram);

	calculationsFile << "*****INPUT_HISTOGRAM*****\n";
	for (int i = 0; i < histogram.size(); i++) {
		calculationsFile << i << " : " << histogram[i] << "\n";
	}
	calculationsFile << "*****INPUT_ENTROPY*****\n";
	calculationsFile << "Entropy : " << entropy;

	std::vector<unsigned char> imageEncoded;
	int size;
	std::tie(imageEncoded, size) = codec.encode(imageToEncode);
	std::cout << "Object encoded: " << object << "\n";
	createFile(imageEncoded, objectsFolder + outputFolder + object + "Encoded.txt");
	std::cout << "Encoded Object saved: " << object << "\n";
	auto imageDecoded = codec.decode(imageEncoded, size);
	std::cout << "Object decoded: " << object << "\n";
	std::string imagesMatch;
	if (areVectorsIdentical(imageToEncode, imageDecoded))
		imagesMatch = "_match_";
	else
		imagesMatch = "_no_match_";
	createPgmImage(imageDecoded, rows, cols, matType, objectsFolder + outputFolder + object + imagesMatch + "Decoded.pgm");
	std::cout << "Decoded Object saved: " << object << "\n";
	calculationsFile.close();
}
