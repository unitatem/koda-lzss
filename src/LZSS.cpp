#include "Dictionary.hpp"
#include "Logger.hpp"
#include "LZSS.hpp"

#include <bitset>
#include <cassert>
#include <cmath>

struct CodecBatchData {
    bool useDictionary;
    unsigned char value;
    int startPositionIdx;
    int matchLength;
};

const int LENGTH_OFFSET = 1;
const int DICTIONARY_BITS_COUNT = static_cast<int>(std::ceil(std::log2(DICTIONARY_SIZE)));
const int WINDOW_BITS_COUNT = static_cast<int>(std::ceil(std::log2(WINDOW_SIZE)));

namespace encode {
    std::pair<std::vector<CodecBatchData>, int> prepareData(const std::vector<unsigned char> &input) {
        Dictionary dict{DICTIONARY_SIZE, input.front()};

        std::vector<CodecBatchData> output;
        output.push_back({false, input.front(), 0, 0});
        auto bitSize = 8u;

        std::string buffer;
        for (auto i = 0u; i < input.size(); ++i) {
            DEBUG(dict.print());
            buffer = {};
            auto restOfTheInput = input.size() - i;
            for (int bufferIter = i;
                 bufferIter < i + (restOfTheInput > WINDOW_SIZE ? WINDOW_SIZE : restOfTheInput);
                 ++bufferIter) {
                buffer += input[bufferIter];
            }
            loggerPrint("BUFFER : " + buffer);
            auto start = 0;
            auto length = 0;
            std::tie(start, length) = dict.findMatch(input, i, i + WINDOW_SIZE);

            // if pattern in dictionary matched
            if (length) {
                loggerPrint("0 " + std::to_string(start) + " " + std::to_string(length));

                output.push_back({true, 0, start, length - LENGTH_OFFSET});
                bitSize += 1 + DICTIONARY_BITS_COUNT + WINDOW_BITS_COUNT;
                for (auto j = 0; j < length; ++j) {
                    dict.shiftOneLeft();
                    dict.insertBack(input[i + j]);
                }
                i += length - 1;
                continue;
            }
            // if do not match current dictionary
            output.push_back({false, input[i], 0, 0});
            bitSize += 1 + 8;
            dict.shiftOneLeft();
            dict.insertBack(input[i]);
            loggerPrint("1 " + std::to_string(input[i]));
        }

        loggerPrint("sizeBeforeCompression = " + std::to_string(8 * input.size()));
        loggerPrint("sizeAfterCompression = " + std::to_string(bitSize));
        return {output, bitSize};
    }

    std::vector<unsigned char> toBinary(const std::vector<CodecBatchData> &data, unsigned int bitCount) {
        loggerPrint("Binary encoded bits string");

        unsigned int outputSizeInBytes = bitCount / 8 + (bitCount % 8 ? 1 : 0);
        std::vector<unsigned char> output(outputSizeInBytes);

        output[0] = data[0].value;
        auto outputIdx = 1u;
        loggerPrint((std::bitset<8>(output[0]).to_string()));

        assert(1 + DICTIONARY_BITS_COUNT + WINDOW_BITS_COUNT <= 32);
        auto buffer = 0u;
        auto size = 0u;
        for (auto i = 1u; i < data.size(); ++i) {
            if (data[i].useDictionary) {
                buffer <<= 1 + DICTIONARY_BITS_COUNT + WINDOW_BITS_COUNT;
                buffer |= (data[i].startPositionIdx << WINDOW_BITS_COUNT) | data[i].matchLength;
                size += 1 + DICTIONARY_BITS_COUNT + WINDOW_BITS_COUNT;
                loggerPrint(std::to_string((data[i].startPositionIdx << WINDOW_BITS_COUNT) | data[i].matchLength));
            } else {
                buffer <<= 1 + 8;
                buffer |= (1 << 8) | data[i].value;
                size += 1 + 8;
                loggerPrint((std::bitset<9>((1u << 8) | data[i].value)).to_string());
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
    std::vector<CodecBatchData> fromBinary(const std::vector<unsigned char> &data, int sizeTotal) {
        loggerPrint("From binary started");
        std::vector<CodecBatchData> output;

        //First value is character which fills dictionary
        output.push_back({false, data.front(), 0, 0});
        int outputSize = 8;
        loggerPrint(std::to_string(output[0].value));

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

                loggerPrint("GlobalBitSet  " + globalBitSet.to_string());
                loggerPrint("CurrentBitSet  " + currentBitSet.to_string());
                //bits  data[currentId];
                if (globalBitSet[8] == 0) {
                    // TODO implement dict and window size flexibility
                    assert(DICTIONARY_BITS_COUNT == 2 && WINDOW_BITS_COUNT == 2);
                    auto var1 = std::bitset<2>(0);
                    var1[1] = globalBitSet[7];
                    var1[0] = globalBitSet[6];
                    auto var2 = std::bitset<2>(0);
                    var2[1] = globalBitSet[5];
                    var2[0] = globalBitSet[4];
                    loggerPrint(std::to_string(var1.to_ulong()) + " " + std::to_string(var2.to_ulong() + 1));

                    output.push_back({true, ' ', (int) var1.to_ulong(), (int) var2.to_ulong() + LENGTH_OFFSET});
                    outputSize += 1 + DICTIONARY_BITS_COUNT + WINDOW_BITS_COUNT;
                    globalBitSet <<= 1 + DICTIONARY_BITS_COUNT + WINDOW_BITS_COUNT;
                    numberOfMeaningBits -= (1 + DICTIONARY_BITS_COUNT + WINDOW_BITS_COUNT);
                    loggerPrint("New GlobalBitSet  " + globalBitSet.to_string());
                } else if (numberOfMeaningBits == 9) {
                    auto value = std::bitset<8>(0);
                    for (int z = 0; z < 8; ++z) {
                        value[z] = globalBitSet[z];
                    }
                    auto val = value.to_ulong();
                    output.push_back({false, static_cast<unsigned char>(val), 0, 0});
                    outputSize += 9;
                    numberOfMeaningBits -= 9;
                    loggerPrint(std::to_string(static_cast<unsigned char>(val)));
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
            } while (repeatLoop);
        }

        return output;
    }

    std::vector<unsigned char> decode(const std::vector<CodecBatchData> &input) {
        loggerPrint("Started Decoding");

        //create dictionary as list of first character
        Dictionary dict{DICTIONARY_SIZE, input.front().value};
        std::vector<unsigned char> output;

        for (auto i = 1u; i < input.size(); ++i) {
            DEBUG(dict.print());
            std::string position = input[i].useDictionary ? (" " + std::to_string(input[i].startPositionIdx) + " " +
                                                             std::to_string(input[i].matchLength)) : "";
            loggerPrint(std::to_string(input[i].useDictionary ? 0 : 1)
                                += (input[i].useDictionary ? "" : " " + std::to_string(input[i].value)) += position);

            if (input[i].useDictionary) {
                for (int j = input[i].startPositionIdx; j < input[i].startPositionIdx + input[i].matchLength; ++j) {
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

    std::vector<CodecBatchData> data;
    auto size = 0u;
    std::tie(data, size) = encode::prepareData(input);
    return {encode::toBinary(data, size), size};
}

std::vector<unsigned char> LZSS::decode(const std::vector<unsigned char> &compressed, int size) const {
    auto metaData = decode::fromBinary(compressed, size);
    return decode::decode(metaData);
}
