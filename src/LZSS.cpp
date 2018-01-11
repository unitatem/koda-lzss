#include "Dictionary.hpp"
#include "Logger.hpp"
#include "LZSS.hpp"
#include <bitset>
#include <cassert>
#include <cmath>
#include <algorithm>

struct CodecBatchData {
    bool useDictionary;
    unsigned char value;
    int startPositionIdx;
    int matchLength;
};

const int LENGTH_OFFSET = 1;
#if ((1 + DICTIONARY_SIZE_BITS + DICTIONARY_MATCH_LENGTH_BITS) > 9)
#define BITSET_SIZE_BITS (1 + DICTIONARY_SIZE_BITS + DICTIONARY_MATCH_LENGTH_BITS)
#else
#define BITSET_SIZE_BITS 9
#endif

namespace encode {
    std::pair<std::vector<CodecBatchData>, int> prepareData(const std::vector<unsigned char> &input) {
        Dictionary dict{DICTIONARY_SIZE, DICTIONARY_MATCH_LENGTH, input.front()};

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
			auto endIter = (i + WINDOW_SIZE) >= input.size() ? input.size() - 1 : i + WINDOW_SIZE;
			if (i < endIter)
				std::tie(start, length) = dict.findMatch(input, i, endIter);

            // if pattern in dictionary matched
            // and dictionary use is more efficient than direct data send
            if (length && (1 + DICTIONARY_SIZE_BITS + DICTIONARY_MATCH_LENGTH_BITS < length * 9)) {
                INFO(loggerPrint("0 " + std::to_string(start) + " " + std::to_string(length));)

                assert(length - LENGTH_OFFSET < DICTIONARY_MATCH_LENGTH);
                output.push_back({true, 0, start, length - LENGTH_OFFSET});
                bitSize += 1 + DICTIONARY_SIZE_BITS + DICTIONARY_MATCH_LENGTH_BITS;
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

        const unsigned int outputSizeInBytes = bitCount / 8 + (bitCount % 8 ? 1 : 0);
        std::vector<unsigned char> output(outputSizeInBytes);

        output[0] = data[0].value;
        auto outputIdx = 1u;
        INFO(loggerPrint((std::bitset<8>(output[0]).to_string()));)

        assert(1 + DICTIONARY_SIZE_BITS + DICTIONARY_MATCH_LENGTH_BITS <= 56);
        uint64_t buffer = 0;
        auto size = 0u;
        for (auto d = 1u; d < data.size(); ++d) {
            if (data[d].useDictionary) {
                buffer <<= 1 + DICTIONARY_SIZE_BITS + DICTIONARY_MATCH_LENGTH_BITS;
                if (data[d].matchLength >= DICTIONARY_MATCH_LENGTH) {
                    std::cout << data[d].matchLength << " " << DICTIONARY_MATCH_LENGTH << std::endl;
                    int mock = 7;
                }
                assert(data[d].startPositionIdx < DICTIONARY_SIZE);
                assert(data[d].matchLength < DICTIONARY_MATCH_LENGTH);
                buffer |= (data[d].startPositionIdx << DICTIONARY_MATCH_LENGTH_BITS) | data[d].matchLength;
                size += 1 + DICTIONARY_SIZE_BITS + DICTIONARY_MATCH_LENGTH_BITS;

                INFO(loggerPrint(std::to_string((data[d].startPositionIdx << DICTIONARY_MATCH_LENGTH_BITS) | data[d].matchLength));)
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

        assert(output.size() == outputSizeInBytes);
        // outputIdx show next available space therefore this formula is correct
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

        auto globalBitSet = std::bitset<BITSET_SIZE_BITS>(0);
        int numberOfMeaningBits = 0;
        auto currentBitSet = std::bitset<BITSET_SIZE_BITS - 1 + 8>(0);
        int numberOfCurrentBits = 0;

        for (auto d = 1u; d <= data.size() && outputSize < sizeTotal; ++d) {
            if (d < data.size()) {
                // take next 8 data bits maybe multiple times
                assert(numberOfCurrentBits == 0);
                for (; numberOfCurrentBits < BITSET_SIZE_BITS && d < data.size();) {
                    currentBitSet <<= 8;
                    currentBitSet |= data[d];
                    ++d;
                    numberOfCurrentBits += 8;
                }
                assert(numberOfCurrentBits < BITSET_SIZE_BITS + 8);
                --d;
            }

            // push as many bits as possible from currentBitSet to globalBitsSet which is working set
            for (int m = BITSET_SIZE_BITS - numberOfMeaningBits; m > 0 && numberOfCurrentBits > 0; --m) {
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
                if (globalBitSet[BITSET_SIZE_BITS - 1] == 0) {
                    auto position = std::bitset<DICTIONARY_SIZE_BITS>(0);
                    // start from least significant bit
                    for (auto b = 0; b < DICTIONARY_SIZE_BITS; ++b)
                        position[b] = globalBitSet[BITSET_SIZE_BITS - 1 - DICTIONARY_SIZE_BITS + b];
                    auto length = std::bitset<DICTIONARY_MATCH_LENGTH_BITS>(0);
                    for (auto b = 0; b < DICTIONARY_MATCH_LENGTH_BITS; ++b)
                        length[b] = globalBitSet[BITSET_SIZE_BITS - 1 - DICTIONARY_SIZE_BITS - DICTIONARY_MATCH_LENGTH_BITS + b];
                    INFO(loggerPrint(std::to_string(position.to_ulong()) + " " + std::to_string(length.to_ulong() + LENGTH_OFFSET));)

                    output.push_back({true, ' ', (int) position.to_ulong(), (int) length.to_ulong() + LENGTH_OFFSET});
                    outputSize += 1 + DICTIONARY_SIZE_BITS + DICTIONARY_MATCH_LENGTH_BITS;
                    globalBitSet <<= 1 + DICTIONARY_SIZE_BITS + DICTIONARY_MATCH_LENGTH_BITS;
                    numberOfMeaningBits -= (1 + DICTIONARY_SIZE_BITS + DICTIONARY_MATCH_LENGTH_BITS);
                    assert(numberOfMeaningBits >= 0);
                    INFO(loggerPrint("New GlobalBitSet  " + globalBitSet.to_string());)
                } else if (numberOfMeaningBits >= 9) {
                    auto valueBitSet = std::bitset<8>(0);
                    for (int b = 0; b < 8; ++b)
                        valueBitSet[b] = globalBitSet[BITSET_SIZE_BITS - 9 + b];
                    auto value = valueBitSet.to_ulong();
                    output.push_back({false, static_cast<unsigned char>(value), 0, 0});
                    outputSize += 9;
                    globalBitSet <<= 9;
                    numberOfMeaningBits -= 9;
                    assert(numberOfMeaningBits >= 0);
                    INFO(loggerPrint(std::to_string(static_cast<unsigned char>(value)));)
                }
                if (numberOfMeaningBits == BITSET_SIZE_BITS) {
                    repeatLoop = true;
                } else if (numberOfCurrentBits > 0) {
                    for (int j = BITSET_SIZE_BITS - numberOfMeaningBits; j > 0 && numberOfCurrentBits > 0; j--) {
                        globalBitSet[j - 1] = currentBitSet[numberOfCurrentBits - 1];
                        numberOfCurrentBits--;
                        numberOfMeaningBits++;
                        if (numberOfMeaningBits == BITSET_SIZE_BITS) {
                            repeatLoop = true;
                            break;
                        }
                    }
                }
                if (d == data.size() && outputSize < sizeTotal && numberOfMeaningBits > 0) {
                    repeatLoop = true;
                }
            } while (repeatLoop);
        }

        assert(outputSize == sizeTotal);
        assert(outputSize == sizeTotal);
        return output;
    }

    std::vector<unsigned char> decode(const std::vector<CodecBatchData> &input) {
        INFO(loggerPrint("Started Decoding");)

        //create dictionary as list of first character
        Dictionary dict{DICTIONARY_SIZE, DICTIONARY_MATCH_LENGTH, input.front().value};
        std::vector<unsigned char> output;

        for (auto i = 1u; i < input.size(); ++i) {
            DEBUG(dict.print());
            INFO(std::string position = input[i].useDictionary ? (" " + std::to_string(input[i].startPositionIdx) + " " +
                                                                 std::to_string(input[i].matchLength)) : "";
                loggerPrint(std::to_string(input[i].useDictionary ? 0 : 1)
                                    += (input[i].useDictionary ? "" : " " + std::to_string(input[i].value)) += position);)

            if (input[i].useDictionary) {
                auto endPosition = input[i].startPositionIdx + input[i].matchLength;
                for (int j = input[i].startPositionIdx; j < endPosition; ++j) {
                    auto outputValue = dict.getCharAtGivenIdx(input[i].startPositionIdx);
                    output.push_back(outputValue);
                    dict.shiftLeft(1);
                    dict.insertFromBack(outputValue, 1);
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
    assert(compressed.size() == (unsigned)size / 8 + (size % 8 ? 1 : 0));
    auto metaData = decode::fromBinary(compressed, size);
    return decode::decode(metaData);
}
