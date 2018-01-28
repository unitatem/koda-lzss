#include "Logger.hpp"
#include "LZSS.hpp"
#include "MediaOperations.hpp"
#include "codec.hpp"
#include <thread>
#include <fstream>

#define DATA
//#define WIKI_CASE
#define DATADEPTH 256

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

	/*threads.push_back(std::thread(performComputations, imagesFolder, images[0], codec, outputFolder));*/

	for (std::string object : images) {
		threads.push_back(std::thread(performComputations, imagesFolder, object, codec, outputFolder));
	}

	for (std::string object : distributions) {
		threads.push_back(std::thread(performComputations, distributionsFolder, object, codec, outputFolder));
	}

	for (auto i = 0u; i < threads.size(); ++i) {
		threads[i].join();
	}


	
#endif
#ifdef WIKI_CASE
    // wikipedia test case, https://pl.wikipedia.org/wiki/LZSS
    std::vector<unsigned char> source = {'a', 'a', 'b', 'b', 'c', 'a', 'b', 'b', 'c', 'a', 'b', 'd'};

//    std::vector<unsigned char> source = {47, 120};

//    std::vector<unsigned char> source = {115, 142, 163, 170, 170, 169, 161, 160, 161, 160, 160, 155, 160, 161};

    printVector("Source", source);
    EncodedDataParams dataEncoded = codec.encode(source);
    printVectorAsBits("Transmission in Bytes: ", dataEncoded.data);
    auto decodedData = codec.decode(dataEncoded.data, dataEncoded.bitSize);
    printVector("Decoded", decodedData);
#endif

    loggerPrint("*******************End*******************");
    return 0;
}

void performComputations(std::string objectsFolder, std::string object, LZSS codec, std::string outputFolder)
{
	std::ofstream calculationsFile;
	auto loadedPgmImage = readPgmImage(objectsFolder + object + ".pgm");
	auto rows = loadedPgmImage.rows;
	auto cols = loadedPgmImage.cols;
	assert(rows != 0 && cols != 0);
	auto matType = loadedPgmImage.type();
	std::cout << "Object loaded: " << object << "\n";
	std::string dictWndSize = "_dictSize=" + std::to_string(DICTIONARY_SIZE) + "_windowSize=" + std::to_string(WINDOW_SIZE);
	calculationsFile.open(objectsFolder + outputFolder + object + dictWndSize + "_calc.txt", std::ios_base::out);

	auto imageToEncode = getImagePixels(loadedPgmImage);
	auto histogram = calculateHistogram(imageToEncode, DATADEPTH);
	auto blockHistogram2Degree = calculateBlockHistogram2Degree(imageToEncode, DATADEPTH);
	auto blockHistogram3Degree = calculateBlockHistogram3Degree(imageToEncode, DATADEPTH);
	double entropy = calculateEntropy(imageToEncode, histogram);

	double blockEntropy2Degree = calculateBlockEntropy2Degree(imageToEncode, blockHistogram2Degree);
	double blockEntropy3Degree = calculateBlockEntropy3Degree(imageToEncode, blockHistogram3Degree);

	calculationsFile << "*****INPUT_ENTROPY*****\n";
	calculationsFile << "Entropy : " << entropy << "\n";
	calculationsFile << "Block Entropy 2 Degree : " << blockEntropy2Degree << "\n";
	calculationsFile << "Block Entropy 3 Degree : " << blockEntropy3Degree << "\n";
	calculationsFile << "*****DICTIONARY&WINDOW_SIZE*****\n";
	calculationsFile << dictWndSize << "\n";

    EncodedDataParams dataEncoded = codec.encode(imageToEncode);
	calculationsFile << "*****FILES_SIZE*****\n";
	calculationsFile << "Original size (bytes) : " << std::to_string(imageToEncode.size()) << "\n";
	calculationsFile << "Encoded size (bytes) : " << std::to_string(dataEncoded.data.size()) << "\n";
    calculationsFile << "Average byte length (bytes) : " << std::to_string(dataEncoded.avgByteLength) << "\n";
	std::cout << "Object encoded: " << object << "\n";
	createFile(dataEncoded.data, objectsFolder + outputFolder + object + dictWndSize + "Encoded.txt");
	std::cout << "Encoded Object saved: " << object << "\n";
	auto imageDecoded = codec.decode(dataEncoded.data, dataEncoded.bitSize);
	calculationsFile << "Decoded size (bytes) : " << std::to_string(imageDecoded.size()) << "\n";
	std::cout << "Object decoded: " << object << "\n";
	std::string imagesMatch;
	if (areVectorsIdentical(imageToEncode, imageDecoded))
		imagesMatch = "_match_";
	else
		imagesMatch = "_no_match_";
	calculationsFile << "*****MATCH_STATUS*****\n";
	calculationsFile << imagesMatch << "\n";
	calculationsFile << "*****INPUT_HISTOGRAM*****\n";
	for (auto i = 0u; i < histogram.size(); i++) {
		calculationsFile << i << " : " << histogram[i] << "\n";
	}
	createPgmImage(imageDecoded, rows, cols, matType, objectsFolder + outputFolder + object + imagesMatch + dictWndSize + "Decoded.pgm");
	std::cout << "Decoded Object saved: " << object << "\n";
	calculationsFile.close();
}
