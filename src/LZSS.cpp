#include "Dictionary.hpp"
#include "Logger.hpp"
#include "LZSS.hpp"

#include <cassert>
#include <cmath>
#include <tuple>
#include <bitset>

struct ExtendedByte {
    bool useDictionary;
#if DICTIONARY_SIZE <= 16
    unsigned char compositeValue;
#else
#error "Unsupported buffer size"
#endif
};

struct DecodeStruct {
    bool useDictionary;
    unsigned char value;
    int startPositionIdx;
    int matchLength;
};

namespace encode {
    int idxToDictionaryPositionSizeInBinary = std::log2(DICTIONARY_SIZE);
	
    std::pair<std::vector<ExtendedByte>, int> prepareData(const std::vector<unsigned char> &input) {
        Dictionary dict{DICTIONARY_SIZE, input.front()};
        std::vector<ExtendedByte> output;
        output.push_back({false, input.front()});
        auto bitSize = 8u;

        for (auto i = 0u; i < input.size(); ++i) {
            DEBUG(dict.print());
			std::string buffer;
            auto restOfTheInput = input.size() - i;
			for (int bufferIter = i; bufferIter < i + (restOfTheInput > BUFFER_SIZE ? BUFFER_SIZE : restOfTheInput); ++bufferIter)
				buffer += input[bufferIter];
			loggerPrint("BUFFER : " + buffer);
            auto start = 0;
            auto length = 0;
            std::tie(start, length) = dict.findMatch(input, i, i + BUFFER_SIZE);

            // pattern in dictionary matched
            if (length) {
                loggerPrint("0 " + std::to_string(start) + " " + std::to_string(length));

                output.push_back({true, static_cast<unsigned char>((start << idxToDictionaryPositionSizeInBinary) |
                                                          length - LENGTH_OFFSET)});
                bitSize += 1 + 2 * idxToDictionaryPositionSizeInBinary;
                for (auto j = 0; j < length; ++j) {
                    dict.shiftOneLeft();
                    dict.insertBack(input[i + j]);
                }
                i += length - 1;
                continue;
            }
			std::string newValue;
			newValue += input[i];
            // do not match current dictionary
            loggerPrint("1 " + newValue);
            output.push_back({false, input[i]});
            bitSize += 1 + 8;
            dict.shiftOneLeft();
            dict.insertBack(input[i]);
        }

        loggerPrint("sizeBeforeCompression = " + std::to_string(8 * input.size()));
        loggerPrint("sizeAfterCompression = " + std::to_string(bitSize));
        return {output, bitSize};
    }

    std::vector<unsigned char> toBinary(const std::vector<ExtendedByte> data, int bitCount) {
        unsigned int outputSizeInBytes = bitCount / 8 + (bitCount % 8 ? 1 : 0);
        std::vector<unsigned char> output(outputSizeInBytes);

        loggerPrint("Binary encoded bits string");
        output[0] = data[0].compositeValue;
        loggerPrint((std::bitset<8>(output[0]).to_string()));

        auto outputIdx = 1u;
        auto buffer = 0u;
        auto size = 0u;
        for (auto i = 1u; i < data.size(); ++i) {
            if (data[i].useDictionary) {
                buffer <<= 1 + 2 * idxToDictionaryPositionSizeInBinary;
                buffer |= data[i].compositeValue;
                loggerPrint((std::bitset<5>(data[i].compositeValue)).to_string());
                size += 1 + 2 * idxToDictionaryPositionSizeInBinary;
            } else {
                buffer <<= 1 + 8;
                buffer |= (1 << 8) | data[i].compositeValue;
                loggerPrint((std::bitset<9>((1 << 8) | data[i].compositeValue)).to_string());
                size += 1 + 8;
            }
            while (size >= 8) {
                size -= 8;
                output[outputIdx++] = static_cast<unsigned char>(buffer >> size);
            }
        }
        if (size)
            output[outputIdx] = static_cast<unsigned char>(buffer << (8 - size));
        assert(outputIdx + (bitCount % 8 ? 1 : 0) == outputSizeInBytes);

        return output;
    }
}

namespace decode {
    std::vector<DecodeStruct> fromBinary(const std::vector<unsigned char> data, int sizeTotal) {
        std::vector<DecodeStruct> output;

        loggerPrint("From binary started");
        //First value is character which fills dictionary
        output.push_back({false, data.front(), 0, 0});
        int outputSize = 8;
		std::string firstValue;
		firstValue += output[0].value;
        loggerPrint(firstValue);

        auto globalBitSet = std::bitset<9>(0);
        int numberOfMeaningBits = 0;
        auto currentBitSet = std::bitset<8>(0);
        int numberOfCurrentBits = 0;

        for (auto i = 1u; i <= data.size() && outputSize < sizeTotal; ++i) {
            if (i < data.size()) {
                assert(numberOfCurrentBits == 0);
                currentBitSet = std::bitset<8>(data[i]);
                numberOfCurrentBits = 8;
            }

            for (int j = 9 - numberOfMeaningBits; j > 0 && numberOfCurrentBits > 0; --j) {
                globalBitSet[j - 1] = currentBitSet[numberOfCurrentBits - 1];
                numberOfCurrentBits--;
                numberOfMeaningBits++;
            }

            bool repeatLoop;
            do {
                repeatLoop = false;

                //bits  data[currentId];
                loggerPrint("GlobalBitSet  " + globalBitSet.to_string());
                loggerPrint("CurrentBitSet  " + currentBitSet.to_string());
                if (globalBitSet[8] == 0) {
                    auto var1 = std::bitset<2>(0);
                    auto var2 = std::bitset<2>(0);
                    var1[1] = globalBitSet[7];
                    var1[0] = globalBitSet[6];
                    var2[1] = globalBitSet[5];
                    var2[0] = globalBitSet[4];
                    //                loggerPrint(std::to_string(var1.to_ulong()) + " " + std::to_string(var2.to_ulong() + 1));
                    output.push_back({true, ' ', (int) var1.to_ulong(), (int) var2.to_ulong() + LENGTH_OFFSET});
                    outputSize += 1 + std::log2(DICTIONARY_SIZE) * 2;
                    globalBitSet <<= 1 + std::log2(DICTIONARY_SIZE) * 2;
                    numberOfMeaningBits -= (1 + std::log2(DICTIONARY_SIZE) * 2);
                    //                loggerPrint("New GlobalBitSet  " + globalBitSet.to_string());
                } else if (numberOfMeaningBits == 9) {
                    auto value = std::bitset<8>(0);
                    for (int z = 0; z < 8; ++z) {
                        value[z] = globalBitSet[z];
                    }
                    auto val = value.to_ulong();
                    output.push_back({false, static_cast<unsigned char>(val), 0, 0});
                    outputSize += 9;
                    //				std::string nextSepararateValue;
                    //				nextSepararateValue += static_cast<unsigned char>(val);
                    //				loggerPrint(nextSepararateValue);
                    numberOfMeaningBits -= 9;
                }
                if (numberOfCurrentBits > 0) {
                    for (int j = 9 - numberOfMeaningBits; j > 0 && numberOfCurrentBits > 0; j--) {
                        globalBitSet[j - 1] = currentBitSet[numberOfCurrentBits - 1];
                        numberOfCurrentBits--;
                        numberOfMeaningBits++;
                        if (numberOfMeaningBits == 9) {
                            repeatLoop = true;
                            break;
                        }
                    }
                }
            } while(repeatLoop);
        }

        return output;
    }

    std::vector<unsigned char> decode(const std::vector<DecodeStruct> &input) {
        loggerPrint("Started Decoding");
        //create dictionary as list of first character
        Dictionary dict{DICTIONARY_SIZE, input.front().value};
        std::vector<unsigned char> output;

        for (auto i = 1u; i < input.size(); ++i) {
            DEBUG(dict.print());
            std::string Value;
            Value += input[i].value;
            std::string position;
            position = input[i].useDictionary ? (" " + std::to_string(input[i].startPositionIdx) + " " + std::to_string(input[i].matchLength)) : "";
            loggerPrint(std::to_string(input[i].useDictionary ? 0 : 1) += (input[i].useDictionary ? "" :" " + Value) += position);

            if (input[i].useDictionary) {
                for (int j = input[i].startPositionIdx; j < input[i].startPositionIdx + input[i].matchLength; j++) {
                    auto outputValue = dict.getCharAtGivenIdx(input[i].startPositionIdx);
                    output.push_back(outputValue);
                    dict.shiftOneLeft();
                    dict.insertBack(outputValue);
                }
            } else {
                output.push_back(input[i].value);
                dict.shiftOneLeft();
                dict.insertBack(input[i].value);
            }
        }

        return output;
    }
}

std::tuple<std::vector<unsigned char>, int> LZSS::encode(const std::vector<unsigned char> &input) const {
    loggerPrint("Started Encoding");
    if (input.empty()) return {};

    std::vector<ExtendedByte> data;
    auto size = 0;
    std::tie(data, size) = encode::prepareData(input);
    return {encode::toBinary(data, size), size};
}

std::vector<unsigned char> LZSS::decode(const std::vector<unsigned char> &compressed, int size) const {
    return decode::decode(decode::fromBinary(compressed, size));
}
