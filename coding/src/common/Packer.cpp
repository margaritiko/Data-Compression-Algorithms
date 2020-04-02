#pragma once

#include <memory.h>
#include <cassert>
#include <string>
#include <bitset>
#include <math.h>
#include <fstream>
#include <iostream>
#include <map>

#include "../coders/LZ77Coder.cpp"
#include "../coders/LZWCoder.cpp"
#include "../coders/ShannonFanoCoder.cpp"

class Packer {
public:

    Packer(std::string outputFileName) {
        this->outputFileName = outputFileName;
    }


    /**
     * Writing the result of coding with LZW algorithm.
     *
     * Parts of output:
     *     - Size N of a dictionary: 8 bits.
     *     - N notes with structure <number of word (int, 32 bits)><character (8 bits)>.
     *     - Number of codes M (32 bits).
     *     - M words' indices (each index takes 32 bits).
     *
     * @param map Dictionary with characters as keys and their codes as values.
     * @param code Total code of a data with given encoding.
     */
    void writeLZWResult(LZWCoder::Result& result) {
        std::string bits = "";

        bits += getBinaryString(static_cast<int>(result.dictionary.size()), 8);
        for (const std::pair<int, std::string>& item: result.dictionary) {
            bits += getBinaryString(item.first, 32);
            bits += getBinaryString(item.second[0], 8);
        }

        bits += getBinaryString(static_cast<int>(result.codes.size()), 32);
        for (const int item: result.codes)
            bits += getBinaryString(item, 32);

        writeBinaryStringToFile(bits);
    }


    // TODO: Может, вынести константы типа 8 bit?
    /**
     * Writing the result of coding with ShannonFano algorithm.
     *
     * Parts of output:
     *     - Size N of a dictionary: 8 bits.
     *     - N notes with structure <character (8 bits)><length of code (32 bits)><code with this variable length>.
     *     - Size of the total code (32 bits).
     *     - Total code of the string.
     *
     * @param map Dictionary with characters as keys and their codes as values.
     * @param code Total code of a data with given encoding.
     */
    void writeShannonFanoResult(ShannonFanoCoder::Result& result) {
        std::map<char, std::string> map = result.asMap();
        std::string code = result.codedData;
        int mapSize = static_cast<int>(map.size());
        std::string bits = "";

        bits += getBinaryString(mapSize, 8);
        for (const std::pair<char, std::string>& item: map) {
            bits += getBinaryString(item.first, 8);
            bits += getBinaryString(item.second.size(), 32);
            bits += item.second;
        }

        bits += getBinaryString(code.size(), 32);
        bits += code;
        writeBinaryStringToFile(bits);
    }


    /**
     * Writing the result of coding with LZ77.
     * @param triples An array with `triples` which represents the result of coding with LZ77.
     * Each triple contain offset (which calculates from right to left), length and character.
     * @param charsInDictionary Max possible number of characters in dictionary.
     * @param charsInBuffer Max possible number of characters in buffer.
     */
    void writeTriples(const std::vector<LZ77Coder::Triple>& triples, int charsInDictionary, int charsInBuffer) {
        std::string bits = "";
        for (size_t index = 0; index < triples.size(); ++index)
            bits += combineTriplesIntoBinaryString(triples[index], charsInDictionary, charsInBuffer);

        writeBinaryStringToFile(bits);
    }


private:
    typedef unsigned char byte;


    std::string outputFileName;


    /**
     * Looks at a substring of given string with length 8 starting from index `left` and converts it to a byte.
     * @param bits Source string with all bits.
     * @param left Starting index.
     * @return A byte which represents a substring.
     */
    byte packByte(const std::string& bits, int left) {
        assert(left + 8 <= bits.size());

        byte result = 0;
        for (unsigned i = 0; i < 8; ++i) {
            if (bits[left + i] == '1') {
                result |= (1 << (7 - i));
            }
        }

        return result;
    }


    /**
     * Writes given string to the binary file.
     * @param bits A string which contains only `1` or `0`.
     */
    void writeBinaryStringToFile(std::string& bits) {
        bits += std::string((bits.size() % 8 == 0) ? 0 : (8 - bits.size() % 8), '0');
        std::ofstream out(outputFileName, std::ios::binary); // Binary mode

        int bitsSize = static_cast<int>(bits.size());

        for (int index = 0; index < bitsSize; index += 8) {
            byte next = packByte(bits, index);
            out.write((char*)&next, sizeof(next));
        }

        out.close();
    }


    /**
     * Makes a binary string from given type.
     * @tparam T Type of value, which will be coverted into binary string.
     * @param value Value to be converted.
     * @param targetLength Required length of the result.
     * @return Binary string, which represents a given value.
     */
    template<class T>
    std::string getBinaryString(T value, int targetLength) {
        std::bitset<sizeof(T) * CHAR_BIT> bits(value);
        std::string totalString = bits.to_string();

        if (targetLength == -1)
            return totalString;

        return totalString.substr(static_cast<int>(totalString.size()) - targetLength, targetLength);
    }


    /**
     * Makes a binary string from given triples.
     */
    std::string combineTriplesIntoBinaryString(const LZ77Coder::Triple& triple, int charsInDictionary, int charsInBuffer) {
        int numberOfBitsForOffset = static_cast<int>(ceil(log2(charsInDictionary)));
        int numberOfBitsForLength = static_cast<int>(ceil(log2(charsInBuffer)));

        std::string bits = getBinaryString<int>(triple.offset, numberOfBitsForOffset) +
                           getBinaryString<int>(triple.length, numberOfBitsForLength) +
                           getBinaryString<char>(triple.character, 8);

        return bits;
    }
};
