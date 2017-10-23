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
    char compositeValue;
#else
#error "Unsupported buffer size"
#endif
};

struct DecodeStruct {
    bool useDictionary;
    char value;
    int val1;
    int val2;
};

namespace encode {
    constexpr int idxToDictionaryPositionSizeInBinary = std::log2(DICTIONARY_SIZE);

    std::pair<std::vector<ExtendedByte>, int> prepareData(const std::vector<char> &input) {
        Dictionary dict{DICTIONARY_SIZE, input.front()};
        std::vector<ExtendedByte> output;
        output.push_back({false, input.front()});
        auto bitSize = 8;

        for (auto i = 0u; i < input.size(); ++i) {
            DEBUG(dict.print());
            auto start = 0;
            auto length = 0;
            std::tie(start, length) = dict.findMatch(input, i, i + BUFFER_SIZE);

            // pattern in dictionary matched
            if (length) {
                DEBUG(std::cout << "0 " << start << " " << length << "\n");

                output.push_back({true, static_cast<char>((start << idxToDictionaryPositionSizeInBinary) | length - LENGTH_OFFESET)});
                bitSize += 1 + 2 * idxToDictionaryPositionSizeInBinary;
                for (auto j = 0; j < length; ++j) {
                    dict.shiftOneLeft();
                    dict.insertBack(input[i + j]);
                }
                i += length - 1;
                continue;
            }

            // do not match current dictionary
            DEBUG(std::cout << "1 " << input[i] << "\n");
            output.push_back({false, input[i]});
            bitSize += 1 + 8;
            dict.shiftOneLeft();
            dict.insertBack(input[i]);
        }

        DEBUG(std::cout << "sizeBeforeCompression = " << 8 * input.size() << "\n");
        DEBUG(std::cout << "sizeAfterCompression = " << bitSize << "\n");
        return {output, bitSize};
    }

    std::vector<char> toBinary(const std::vector<ExtendedByte> data, int bitCount) {
        int outputSizeInBytes = bitCount / 8 + (bitCount % 8 ? 1 : 0);
        std::vector<char> output(outputSizeInBytes);

        DEBUG(std::cout << "Binary encoded bits string\n");
        output[0] = data[0].compositeValue;
        DEBUG(std::cout << std::bitset<8>(output[0]) << "\n");

        auto outputIdx = 1;
        int buffer = 0;
        auto size = 0;
        for (auto i = 1; i < data.size(); ++i) {
            if (data[i].useDictionary) {
                buffer <<= 1 + 2 * idxToDictionaryPositionSizeInBinary;
                buffer |= data[i].compositeValue;
                DEBUG(std::cout << std::bitset<5>(data[i].compositeValue) << "\n");
                size += 1 + 2 * idxToDictionaryPositionSizeInBinary;
            } else {
                buffer <<= 1 + 8;
                buffer |= (1 << 8) | data[i].compositeValue;
                DEBUG(std::cout << std::bitset<9>((1 << 8) | data[i].compositeValue) << "\n");
                size += 1 + 8;
            }
            while (size >= 8) {
                size -= 8;
                output[outputIdx++] = static_cast<char>(buffer >> size);
            }
        }
        if (size)
            output[outputIdx] = static_cast<char>(buffer << (8 - size));
        assert(outputIdx + 1 == outputSizeInBytes);

        return output;
    }
}

namespace decode {

    std::vector<char> decode(const std::vector<DecodeStruct> &input) {
        //create dictionary as list of first character
        Dictionary dict{DICTIONARY_SIZE, input.front().value};
        std::vector<char> output;
        auto bitSize = 8;

        for (auto i = 0u; i < input.size(); ++i) {
            DEBUG(dict.print());

            DEBUG(std::cout << input[i].useDictionary << " " << input[i].value << " " << input[i].val1 << " " << input[i].val2 <<"\n");

            //dict.shiftOneLeft();
            //dict.insertBack(input[i]);
        }

        return output;
    }

    std::vector<DecodeStruct> fromBinary(const std::vector<char> data, int bitCount) {
        std::vector<DecodeStruct> output;

        DEBUG(std::cout << "From binary started \n");
        //First value is character which fills dictionary
        output.push_back({false, data.front(), 0, 0});
        DEBUG(std::cout << output[0].value << "\n");

        auto globalBitSet = std::bitset<9>(0);
        int numberOfMeaningBits = 0;
        auto currentBitSet = std::bitset<8>(0);
        int numberOfCurrentBits = 0;

        for (auto i = 1; i <= data.size(); ++i) {
            if (i < data.size()){
                currentBitSet = std::bitset<8>(data[i]);
                numberOfCurrentBits = 8;
            }

            for (int j = 9 - numberOfMeaningBits; j > 0 && numberOfCurrentBits > 0; j --) {
                globalBitSet[j-1] = currentBitSet[numberOfCurrentBits-1];
                numberOfCurrentBits--;
                numberOfMeaningBits++;
            }

            //bits  data[currentId];
            DEBUG(std::cout << globalBitSet << std::endl);
            if (globalBitSet[8] == 0) {
                auto var1 = std::bitset<2>(0);
                auto var2 = std::bitset<2>(0);
                var1[1] = globalBitSet[7];
                var1[0] = globalBitSet[6];
                var2[1] = globalBitSet[5];
                var2[0] = globalBitSet[4];
                DEBUG(std::cout << (int)var1.to_ulong() << std::endl);
                DEBUG(std::cout << (int)var2.to_ulong()+1 << std::endl);
                output.push_back({true, ' ', (int)var1.to_ulong(), (int)var2.to_ulong()+1});
                globalBitSet = globalBitSet << 5;
                numberOfMeaningBits -= 5;
                DEBUG(std::cout << globalBitSet << std::endl);
            } else if (numberOfMeaningBits == 9){
                auto value = std::bitset<8>(0);
                for (int z= 0; z<8; z++) {
                    value[z] = globalBitSet[z];
                }
                auto val = value.to_ulong();
                output.push_back({false, static_cast<unsigned char>(val), 0, 0});
                numberOfMeaningBits -= 9;
            }
            if (numberOfCurrentBits > 0) {
                for (int j = 9 - numberOfMeaningBits; j > 0 && numberOfCurrentBits > 0; j --) {
                    globalBitSet[j-1] = currentBitSet[numberOfCurrentBits-1];
                    numberOfCurrentBits--;
                    numberOfMeaningBits++;
                }
            };
        }

        return output;
    }
}

std::vector<char> LZSS::encode(const std::vector<char> &input) const {
    DEBUG(std::cout << "encode()\n");
    if (input.empty()) return {};

    std::vector<ExtendedByte> data;
    auto size = 0;
    std::tie(data, size) = encode::prepareData(input);
    return encode::toBinary(data, size);
}

std::vector<char> LZSS::decode(const std::vector<char> &compressed) const {
    DEBUG(std::cout << "decode()\n");
    return decode::decode(decode::fromBinary(compressed, 8));
}
