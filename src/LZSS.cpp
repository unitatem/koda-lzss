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

namespace encode {
    std::pair<std::vector<CodecBatchData>, int> prepareData(const std::vector<unsigned char> &input) {
        Dictionary dict{DICTIONARY_SIZE, input.front()};

        std::vector<CodecBatchData> output;
        output.push_back({false, input.front(), 0, 0});
        auto bitSize = 8u;

        INFO(std::string buffer;)
        for (auto i = 0u; i < input.size(); ++i) {
            DEBUG(dict.print());
            INFO(buffer = {};
                 auto restOfTheInput = input.size() - i;
                 for (int bufferIter = i;
                 bufferIter < i + (restOfTheInput > WINDOW_SIZE ? WINDOW_SIZE : restOfTheInput);
                     ++bufferIter) {
                    buffer += input[bufferIter];
                }
                loggerPrint("BUFFER : " + buffer);)
            auto start = 0;
            auto length = 0;
            std::tie(start, length) = dict.findMatch(input, i, i + WINDOW_SIZE);

            // if pattern in dictionary matched
            // and dictionary use is more efficient than direct data send
            if (length && (1 + 2 * DICTIONARY_BITS_COUNT < length * 9)) {
                INFO(loggerPrint("0 " + std::to_string(start) + " " + std::to_string(length));)

                output.push_back({true, 0, start, length - LENGTH_OFFSET});
                bitSize += 1 + 2 * DICTIONARY_BITS_COUNT;
                dict.shiftLeft(length);
                for (auto l = 0; l < length; ++l)
                    dict.insertFromBack(input[i + l], length - l);
                i += length - 1;
                continue;
            }
            // if do not match current dictionary
            output.push_back({false, input[i], 0, 0});
            bitSize += 1 + 8;
            dict.shiftLeft(1);
            dict.insertFromBack(input[i], 1);
            INFO(loggerPrint("1 " + std::to_string(input[i]));)
        }

        INFO(loggerPrint("sizeBeforeCompression = " + std::to_string(8 * input.size()));
            loggerPrint("sizeAfterCompression = " + std::to_string(bitSize));)
        return {output, bitSize};
    }

    std::vector<unsigned char> toBinary(const std::vector<CodecBatchData> &data, unsigned int bitCount) {
        INFO(loggerPrint("Binary encoded bits string");)

        unsigned int outputSizeInBytes = bitCount / 8 + (bitCount % 8 ? 1 : 0);
        std::vector<unsigned char> output(outputSizeInBytes);

        output[0] = data[0].value;
        auto outputIdx = 1u;
        INFO(loggerPrint((std::bitset<8>(output[0]).to_string()));)

        assert(1 + 2 * DICTIONARY_BITS_COUNT <= 56);
        uint64_t buffer = 0;
        auto size = 0u;
        for (auto d = 1u; d < data.size(); ++d) {
            if (data[d].useDictionary) {
                buffer <<= 1 + 2 * DICTIONARY_BITS_COUNT;
                buffer |= (data[d].startPositionIdx << DICTIONARY_BITS_COUNT) | data[d].matchLength;
                size += 1 + 2 * DICTIONARY_BITS_COUNT;
                INFO(loggerPrint(std::to_string((data[d].startPositionIdx << DICTIONARY_BITS_COUNT) | data[d].matchLength));)
            } else {
                buffer <<= 1 + 8;
                buffer |= (1 << 8) | data[d].value;
                size += 1 + 8;
                INFO(loggerPrint((std::bitset<9>((1u << 8) | data[d].value)).to_string());)
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
        INFO(loggerPrint("From binary started");)
        std::vector<CodecBatchData> output;

        //First value is character which fills dictionary
        output.push_back({false, data.front(), 0, 0});
        int outputSize = 8;
        INFO(loggerPrint(std::to_string(output[0].value));)

        const auto bitSetSize = std::max(9, 1 + 2 * DICTIONARY_BITS_COUNT);
        auto globalBitSet = std::bitset<bitSetSize>(0);
        int numberOfMeaningBits = 0;
        auto currentBitSet = std::bitset<bitSetSize - 1 + 8>(0);
        int numberOfCurrentBits = 0;

        for (auto d = 1u; d <= data.size() && outputSize < sizeTotal; ++d) {
            if (d < data.size()) {
                // take next 8 data bits maybe multiple times
                assert(numberOfCurrentBits == 0);
                for (; numberOfCurrentBits < bitSetSize && d < data.size();) {
                    currentBitSet <<= 8;
                    currentBitSet |= data[d];
                    ++d;
                    numberOfCurrentBits += 8;
                }
                assert(numberOfCurrentBits < bitSetSize + 8);
                --d;
            }

            // push as many bits as possible from currentBitSet to globalBitsSet which is working set
            for (int m = bitSetSize - numberOfMeaningBits; m > 0 && numberOfCurrentBits > 0; --m) {
                globalBitSet[m - 1] = currentBitSet[numberOfCurrentBits - 1];
                numberOfCurrentBits--;
                numberOfMeaningBits++;
            }

            bool repeatLoop;
            do {
                repeatLoop = false;

                INFO(loggerPrint("GlobalBitSet  " + globalBitSet.to_string());)
                INFO(loggerPrint("CurrentBitSet  " + currentBitSet.to_string());)
                //bits  data[currentId];
                if (globalBitSet[bitSetSize - 1] == 0) {
                    auto position = std::bitset<DICTIONARY_BITS_COUNT>(0);
                    // start from least significant bit
                    for (auto b = 0; b < DICTIONARY_BITS_COUNT; ++b)
                        position[b] = globalBitSet[bitSetSize - 1 - DICTIONARY_BITS_COUNT + b];
                    auto length = std::bitset<DICTIONARY_BITS_COUNT>(0);
                    for (auto b = 0; b < DICTIONARY_BITS_COUNT; ++b)
                        length[b] = globalBitSet[bitSetSize - 1 - 2 * DICTIONARY_BITS_COUNT + b];
                    INFO(loggerPrint(std::to_string(position.to_ulong()) + " " + std::to_string(length.to_ulong() + LENGTH_OFFSET));)

                    output.push_back({true, ' ', (int) position.to_ulong(), (int) length.to_ulong() + LENGTH_OFFSET});
                    outputSize += 1 + 2 * DICTIONARY_BITS_COUNT;
                    globalBitSet <<= 1 + 2 * DICTIONARY_BITS_COUNT;
                    numberOfMeaningBits -= (1 + 2 * DICTIONARY_BITS_COUNT);
                    INFO(loggerPrint("New GlobalBitSet  " + globalBitSet.to_string());)
                } else if (numberOfMeaningBits >= 9) {
                    auto valueBitSet = std::bitset<8>(0);
                    for (int b = 0; b < 8; ++b)
                        valueBitSet[b] = globalBitSet[bitSetSize - 9 + b];
                    auto value = valueBitSet.to_ulong();
                    output.push_back({false, static_cast<unsigned char>(value), 0, 0});
                    outputSize += 9;
                    globalBitSet <<= 9;
                    numberOfMeaningBits -= 9;
                    INFO(loggerPrint(std::to_string(static_cast<unsigned char>(value)));)
                }
                if (numberOfCurrentBits > 0) {
                    for (int j = bitSetSize - numberOfMeaningBits; j > 0 && numberOfCurrentBits > 0; j--) {
                        globalBitSet[j - 1] = currentBitSet[numberOfCurrentBits - 1];
                        numberOfCurrentBits--;
                        numberOfMeaningBits++;
                        if (numberOfMeaningBits == bitSetSize) {
                            repeatLoop = true;
                            break;
                        }
                    }
                }
            } while (repeatLoop);
        }

        assert(outputSize == sizeTotal);
        return output;
    }

    std::vector<unsigned char> decode(const std::vector<CodecBatchData> &input) {
        INFO(loggerPrint("Started Decoding");)

        //create dictionary as list of first character
        Dictionary dict{DICTIONARY_SIZE, input.front().value};
        std::vector<unsigned char> output;

        for (auto i = 1u; i < input.size(); ++i) {
            DEBUG(dict.print());
            INFO(std::string position = input[i].useDictionary ? (" " + std::to_string(input[i].startPositionIdx) + " " +
                                                                 std::to_string(input[i].matchLength)) : "";
                loggerPrint(std::to_string(input[i].useDictionary ? 0 : 1)
                                    += (input[i].useDictionary ? "" : " " + std::to_string(input[i].value)) += position);)

            if (input[i].useDictionary) {
                auto endPosition = input[i].startPositionIdx + input[i].matchLength;
                dict.shiftLeft(endPosition - input[i].startPositionIdx);
                for (int j = input[i].startPositionIdx; j < input[i].startPositionIdx + input[i].matchLength; ++j) {
                    auto outputValue = dict.getCharAtGivenIdx(input[i].startPositionIdx);
                    output.push_back(outputValue);
                    dict.insertFromBack(outputValue, endPosition - j);
                }
            } else {
                output.push_back(input[i].value);
                dict.shiftLeft(1);
                dict.insertFromBack(input[i].value, 1);
            }
        }

        return output;
    }
}

std::tuple<std::vector<unsigned char>, int> LZSS::encode(const std::vector<unsigned char> &input) const {
    INFO(loggerPrint("Started Encoding");)
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
