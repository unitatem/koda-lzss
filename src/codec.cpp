#include "Logger.hpp"
#include "LZSS.hpp"
#include "MediaOperations.hpp"

#define IMAGES
//#define WIKI_CASE

int main() {
    loggerPrint("*******************Start*******************");
    loggerPrint("Dictionary size: " + std::to_string(DICTIONARY_SIZE));
    loggerPrint("Buffer size: " + std::to_string(WINDOW_SIZE));
    LZSS codec;

#ifdef IMAGES
    std::string imagesFolder = "../obrazy/";
    std::string image = "lena";

    auto loadedPgmImage = readPgmImage(imagesFolder + image + ".pgm");
    auto rows = loadedPgmImage.rows;
    auto cols = loadedPgmImage.cols;
    assert(rows != 0 && cols != 0);
    auto matType = loadedPgmImage.type();

    auto imageToEncode = getImagePixels(loadedPgmImage);
    std::vector<unsigned char> imageEncoded;
    int size;
    std::tie(imageEncoded, size) = codec.encode(imageToEncode);
    createFile(imageEncoded, imagesFolder + image + "Encoded.txt");
    auto imageDecoded = codec.decode(imageEncoded, size);

    createPgmImage(imageDecoded, rows, cols, matType, imagesFolder + image + "Decoded.pgm");
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