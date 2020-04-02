#pragma once

#include <fstream>
#include <string>
#include <bitset>
#include <math.h>

#include "../coders/LZ77Coder.cpp"
#include "../coders/ShannonFanoCoder.cpp"
#include "../coders/LZWCoder.cpp"


class Unpacker {

public:
    Unpacker(std::string sourceFileName) {
        this->sourceFileName = sourceFileName;
    }


    LZWCoder::Result readLZWResult() {
        std::string bits = readOrigin();
        assert(bits.size() > 0);

        int N = 8;
        int WORD_NUMBER = 32;
        int CHARACTER_BITS = 8;
        int M = 32;
        int CODES_BITS_PRESENT = 32;

        int mapSize = unpackBits(bits, 0, N);
        std::map<int, std::string> dictionary;
        int pointer = N;
        for (int numberOfItemsInMap = 0; numberOfItemsInMap < mapSize; ++numberOfItemsInMap) {
            int wordNumber = unpackBits(bits, pointer, WORD_NUMBER);
            char character = unpackBits(bits, pointer + WORD_NUMBER, CHARACTER_BITS);

            dictionary[wordNumber] = character;

            pointer += WORD_NUMBER + CHARACTER_BITS;
        }

        int codesSize = unpackBits(bits, pointer, M);
        std::vector<int> codes(codesSize);
        pointer += M;

        for (int numberOfCodes = 0; numberOfCodes < codesSize; ++numberOfCodes) {
            codes[numberOfCodes] = unpackBits(bits, pointer, CODES_BITS_PRESENT);
            pointer += CODES_BITS_PRESENT;
        }

        return LZWCoder::Result(dictionary, codes);
    }


    ShannonFanoCoder::Result readShannonFanoResult() {
        std::string bits = readOrigin();
        assert(bits.size() > 0);

        // TODO: Перенести константы
        int N = 8;
        int CHAR_LENGTH_BITS = 8;
        int CODES_LENGTH_BITS = 32;
        int TOTAL_CODE_LENGTH_BITS = 32;

        int mapSize = unpackBits(bits, 0, N);
        std::vector<char> values(mapSize);
        std::vector<std::string> codes(mapSize);

        int pointer = N;
        for (int numberOfItemsInMap = 0; numberOfItemsInMap < mapSize; ++numberOfItemsInMap) {
            char character = unpackBits(bits, pointer, CHAR_LENGTH_BITS);
            int codesLength = unpackBits(bits, pointer + CHAR_LENGTH_BITS, CODES_LENGTH_BITS);
            std::string code = bits.substr(pointer + CHAR_LENGTH_BITS + CODES_LENGTH_BITS, codesLength);

            values[numberOfItemsInMap] = character;
            codes[numberOfItemsInMap] = code;

            pointer += CHAR_LENGTH_BITS + CODES_LENGTH_BITS + codesLength;
        }

        int totalCodeLength = unpackBits(bits, pointer, TOTAL_CODE_LENGTH_BITS);
        pointer += TOTAL_CODE_LENGTH_BITS;
        std::string code = bits.substr(pointer, totalCodeLength);

        return ShannonFanoCoder::Result(values, codes, code);
    }


    /**
     * Reading LZ77Coder output.
     * @param charsInDictionary Max possible number of characters in dictionary.
     * @param charsInBuffer Max possible number of characters in buffer.
     * @return An array with triples, which are used in LZ77 algorithm to save the result of coding.
     */
    std::vector<LZ77Coder::Triple> readTriples(int charsInDictionary, int charsInBuffer) {
        std::string bits = readOrigin();
        assert(bits.size() > 0);

        int numberOfBitsForOffset = static_cast<int>(ceil(log2(charsInDictionary)));
        int numberOfBitsForLength = static_cast<int>(ceil(log2(charsInBuffer)));
        int step = numberOfBitsForLength + numberOfBitsForOffset + 8;

        int bitsSize = static_cast<int>(bits.size());
        std::vector<LZ77Coder::Triple> encodedInfo;
        for (int index = 0; index + numberOfBitsForLength + numberOfBitsForOffset < bitsSize; index += step) {
            int unpackedOffset = unpackBits(bits, index, numberOfBitsForOffset);
            int unpackedLength = unpackBits(bits, index + numberOfBitsForOffset, numberOfBitsForLength);
            int unpackedCharacter = unpackBits(bits, index + numberOfBitsForLength + numberOfBitsForOffset, 8);

            encodedInfo.push_back(LZ77Coder::Triple(unpackedOffset, unpackedLength, unpackedCharacter));
        }

        return encodedInfo;
    }


private:
    typedef unsigned char byte;


    std::string sourceFileName;


    /**
     * Read a content of a binary file.
     * @return String with data from a binary file.
     */
    std::string readOrigin() {
        std::string input = "";
        byte nextByte;
        std::ifstream in(sourceFileName,std::ios::binary);

        while (in.read((char *) &nextByte, sizeof(nextByte))) {
            std::bitset<sizeof(byte) * CHAR_BIT> bitset(nextByte);
            input += bitset.to_string();
        }

        in.close();
        return input;
    }


    /**
     * Converts given binary string into integer.
     */
    int unpackBits(const std::string& bits, int start, int length) {
        int value = 0;

        for (int index = length - 1; index >= 0; --index) {
            if (bits[start + length - 1 - index] == '1') {
                value |= (1 << index);
            }
        }

        return value;
    }
};
