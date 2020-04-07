#include <string>
#include <bitset>
#include <math.h>

#ifndef CODING_ALGORITHMS
#define CODING_ALGORITHMS

#include "../coders/LZ77Coder.cpp"
#include "../coders/LZWCoder.cpp"
#include "../coders/ShannonFanoCoder.cpp"

#endif

#ifndef FILE_CONVERTER
#define FILE_CONVERTER

#include "Converter.cpp"

#endif


class Unpacker {

public:
    Unpacker(std::string sourceFileName) {
        this->sourceFileName = sourceFileName;
    }


    LZWCoder::Result readLZWResult() {
        CharSequence bits = readOrigin();
        assert(bits.size() > 0);

        int mapSize = unpackBits(bits, 0, constants::DICTIONARY_SIZE_LZW) + 1;
        std::map<int, CharSequence> dictionary;
        int pointer = constants::DICTIONARY_SIZE_LZW;
        for (int numberOfItemsInMap = 0; numberOfItemsInMap < mapSize; ++numberOfItemsInMap) {
            char character = unpackBits(bits, pointer, constants::CHARACTER_BITS_LZW);
            dictionary[numberOfItemsInMap + 1] = CharSequence{character};
            pointer += constants::CHARACTER_BITS_LZW;
        }

        int codesSize = unpackBits(bits, pointer, constants::NUMBER_OF_CODES_LZW);
        std::vector<int> codes(codesSize);
        pointer += constants::NUMBER_OF_CODES_LZW;

        for (int numberOfCodes = 0; numberOfCodes < codesSize; ++numberOfCodes) {
            codes[numberOfCodes] = unpackBits(bits, pointer, constants::CODES_BITS_PRESENT_LZW);
            pointer += constants::CODES_BITS_PRESENT_LZW;
        }

        return LZWCoder::Result(dictionary, codes);
    }


    ShannonFanoCoder::Result readShannonFanoResult() {
        CharSequence bits = readOrigin();
        assert(bits.size() > 0);

        int mapSize = unpackBits(bits, 0, constants::NUMBER_OF_CODES_SF) + 1;
        std::vector<char> values(mapSize);
        std::vector<CharSequence> codes(mapSize);

        int pointer = constants::NUMBER_OF_CODES_SF;
        for (int numberOfItemsInMap = 0; numberOfItemsInMap < mapSize; ++numberOfItemsInMap) {
            char character = unpackBits(bits, pointer, constants::CHAR_LENGTH_BITS_SF);
            int codesLength = unpackBits(bits, pointer + constants::CHAR_LENGTH_BITS_SF, constants::CODES_LENGTH_BITS_SF);
            CharSequence code = utils::subsequence(bits, pointer + constants::CHAR_LENGTH_BITS_SF +
                                                   constants::CODES_LENGTH_BITS_SF, codesLength);

            values[numberOfItemsInMap] = character;
            codes[numberOfItemsInMap] = code;

            pointer += constants::CHAR_LENGTH_BITS_SF + constants::CODES_LENGTH_BITS_SF + codesLength;
        }

        int totalCodeLength = unpackBits(bits, pointer, constants::TOTAL_CODE_LENGTH_BITS_SF);
        pointer += constants::TOTAL_CODE_LENGTH_BITS_SF;
        CharSequence code = utils::subsequence(bits, pointer, totalCodeLength);

        return ShannonFanoCoder::Result(values, codes, code);
    }


    /**
     * Reading LZ77Coder output.
     * @param charsInDictionary Max possible number of characters in dictionary.
     * @param charsInBuffer Max possible number of characters in buffer.
     * @return An array with triples, which are used in LZ77 algorithm to save the result of coding.
     */
    std::vector<LZ77Coder::Triple> readTriples(int charsInDictionary, int charsInBuffer) {
        CharSequence bits = readOrigin();
        assert(bits.size() > 0);

        int numberOfBitsForOffset = static_cast<int>(ceil(log2(charsInDictionary)));
        int numberOfBitsForLength = static_cast<int>(ceil(log2(charsInBuffer)));
        if (utils::isPowerOfTwo(charsInBuffer))
            numberOfBitsForLength++;

        int step = numberOfBitsForLength + numberOfBitsForOffset + constants::BITS_PER_CHARACTER_LZ77;

        int bitsSize = static_cast<int>(bits.size());
        std::vector<LZ77Coder::Triple> encodedInfo;
        for (int index = 0; index + numberOfBitsForLength + numberOfBitsForOffset < bitsSize; index += step) {
            int unpackedOffset = unpackBits(bits, index, numberOfBitsForOffset);
            int unpackedLength = unpackBits(bits, index + numberOfBitsForOffset, numberOfBitsForLength);
            unpackedOffset = unpackedOffset == 0 && unpackedLength == 0 ? unpackedOffset : unpackedOffset + 1;
            int unpackedCharacter = unpackBits(bits, index + numberOfBitsForLength +
                                               numberOfBitsForOffset, constants::BITS_PER_CHARACTER_LZ77);

            encodedInfo.push_back(LZ77Coder::Triple(unpackedOffset, unpackedLength, unpackedCharacter));
        }

        return encodedInfo;
    }


private:


    std::string sourceFileName;


    /**
     * Read a content of a binary file.
     * @return String with data from a binary file.
     */
    CharSequence readOrigin() {
        CharSequence chars = Converter::getInstance().readBinaryFile(sourceFileName);
        CharSequence input;

        for (const char& byte: chars) {
            std::bitset<sizeof(byte) * CHAR_BIT> bitset(byte);
            std::string bitsStr = bitset.to_string();

            input.insert(input.end(), bitsStr.begin(), bitsStr.end());
        }

        return input;
    }


    /**
     * Converts given binary sequence into integer.
     */
    int unpackBits(const CharSequence& bits, int start, int length) {
        int value = 0;

        for (int index = length - 1; index >= 0; --index) {
            if (bits[start + length - 1 - index] == '1') {
                value |= (1 << index);
            }
        }

        return value;
    }
};
