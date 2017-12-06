#include "Logger.hpp"
#include "LZSS.hpp"
#include "MediaOperations.hpp"

#define IMAGES
//#define WIKI_CASE

int main() {
    loggerPrint("*******************Start*******************");
    loggerPrint("Dictionary size: " + std::to_string(DICTIONARY_SIZE));
    loggerPrint("Buffer size: " + std::to_string(BUFFER_SIZE));
    LZSS codec;

#ifdef IMAGES
    std::string imagesFolder = "/home/mariusz/workingDirectory/koda-lzss/obrazy/";
    std::string image = "lena";

    auto loadedPgmImage = readPgmImage(imagesFolder + image + ".pgm");

    auto rows = loadedPgmImage.rows;
    auto cols = loadedPgmImage.cols;
    auto matType = loadedPgmImage.type();
    auto imageToEncode = getImagePixels(loadedPgmImage); //imageData
    std::vector<unsigned char> imageEncoded;
    int size;
    std::tie(imageEncoded, size) = codec.encode(imageToEncode);
    createFile(imageEncoded, imagesFolder + image + "Encoded.txt");
    auto imageDecoded = codec.decode(imageEncoded, size);

    auto sizeFinal = imageDecoded.size();

    createPgmImage(imageDecoded, rows, cols, matType, imagesFolder + image + "Decoded.pgm");
#endif
#ifdef WIKI_CASE
    // wikipedia test case, https://pl.wikipedia.org/wiki/LZSS
    std::vector<unsigned char> source = {'a', 'a', 'b', 'b', 'c', 'a', 'b', 'b', 'c', 'a', 'b', 'd'};

//    std::vector<unsigned char> source = {47, 120};

//    std::vector<unsigned char> source = {115, 142, 163, 170, 170, 169, 161, 160, 161, 160, 160, 155, 160, 161};

    printVector("Source", source);
    std::vector<unsigned char> comp;
    int size;
    std::tie(comp, size) = codec.encode(source);
    printVectorAsBits("Transmission in Bytes: ", comp);
    auto dec = codec.decode(comp, size);
    printVector("Decoded", dec);


    loggerPrint("*******************End*******************");
//	system("pause");
#endif
    return 0;
}