#include <cassert>
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

class Packer {
public:

    Packer(std::string outputFileName) {
        this->outputFileName = outputFileName;
    }


    /**
     * Writing the result of coding with LZW algorithm.
     *
     * Parts of output:
     *     - Size N of a dictionary minus 1: 8 bits.
     *     - N notes with structure <number of word (int, 32 bits)><character (8 bits)>.
     *     - Number of codes M (32 bits).
     *     - M words' indices (each index takes 32 bits).
     *
     * @param map Dictionary with characters as keys and their codes as values.
     * @param code Total code of a data with given encoding.
     */
    void writeLZWResult(LZWCoder::Result& result) {
        CharSequence bits;

        assert(result.dictionary.size() > 0);

        utils::append(bits, getBinaryString(static_cast<int>(result.dictionary.size() - 1),
                                                      constants::DICTIONARY_SIZE_LZW));
        for (const std::pair<int, CharSequence>& item: result.dictionary)
            utils::append(bits, getBinaryString(item.second[0], constants::CHARACTER_BITS_LZW));

        utils::append(bits, getBinaryString(static_cast<int>(result.codes.size()), constants::NUMBER_OF_CODES_LZW));
        for (const int item: result.codes)
            utils::append(bits, getBinaryString(item, constants::CODES_BITS_PRESENT_LZW));

        Converter::getInstance().writeBinaryStringToFile(bits, outputFileName);
    }


    /**
     * Writing the result of coding with ShannonFano algorithm.
     *
     * Parts of output:
     *     - Size N of a dictionary minus one (we assume that dictionary is always not empty): 8 bits.
     *     - N notes with structure <character (8 bits)><length of code (32 bits)><code with this variable length>.
     *     - Size of the total code (32 bits).
     *     - Total code of the string.
     *
     * @param map Dictionary with characters as keys and their codes as values.
     * @param code Total code of a data with given encoding.
     */
    void writeShannonFanoResult(ShannonFanoCoder::Result& result) {
        std::map<char, CharSequence> map = result.asMap();
        assert(map.size() > 0);

        CharSequence code = result.codedData;
        int mapSize = static_cast<int>(map.size() - 1);
        CharSequence bits;

        utils::append(bits, getBinaryString(mapSize, constants::NUMBER_OF_CODES_SF));
        for (const std::pair<char, CharSequence>& item: map) {
            utils::append(bits, getBinaryString(item.first, constants::CHAR_LENGTH_BITS_SF));
            int itemSecondSize = static_cast<int>(item.second.size());
            utils::append(bits, getBinaryString(itemSecondSize, constants::CODES_LENGTH_BITS_SF));
            utils::append(bits, item.second);
        }

        int codeSize = static_cast<int>(code.size());
        utils::append(bits, getBinaryString(codeSize, constants::TOTAL_CODE_LENGTH_BITS_SF));
        utils::append(bits, code);

        Converter::getInstance().writeBinaryStringToFile(bits, outputFileName);
    }


    /**
     * Writing the result of coding with LZ77.
     * @param triples An array with `triples` which represents the result of coding with LZ77.
     * Each triple contain offset (which calculates from right to left), length and character.
     * @param charsInDictionary Max possible number of characters in dictionary.
     * @param charsInBuffer Max possible number of characters in buffer.
     */
    void writeTriples(const std::vector<LZ77Coder::Triple>& triples, int charsInDictionary, int charsInBuffer) {
        CharSequence bits;
        for (size_t index = 0; index < triples.size(); ++index) {
            CharSequence codedTriples = combineTriplesIntoBinaryString(triples[index], charsInDictionary, charsInBuffer);
            bits.insert(bits.end(), codedTriples.begin(), codedTriples.end());
        }

        Converter::getInstance().writeBinaryStringToFile(bits, outputFileName);
    }


private:


    std::string outputFileName;


    /**
     * Makes a binary string from given type.
     * @tparam T Type of value, which will be coverted into binary string.
     * @param value Value to be converted.
     * @param targetLength Required length of the result.
     * @return Binary string, which represents a given value.
     */
    template<class T>
    CharSequence getBinaryString(T value, int targetLength) {
        std::bitset<sizeof(T) * CHAR_BIT> bits(value);
        std::string strData = bits.to_string();

        if (targetLength != -1)
            strData = strData.substr(static_cast<int>(strData.size()) - targetLength, targetLength);
        return CharSequence(strData.begin(), strData.end());
    }


    /**
     * Makes a binary string from given triples.
     */
    CharSequence combineTriplesIntoBinaryString(const LZ77Coder::Triple& triple, int charsInDictionary, int charsInBuffer) {
        int numberOfBitsForOffset = static_cast<int>(ceil(log2(charsInDictionary)));
        int numberOfBitsForLength = static_cast<int>(ceil(log2(charsInBuffer)));

        if (utils::isPowerOfTwo(charsInBuffer))
            numberOfBitsForLength++;

        CharSequence bits;
        utils::append(bits, getBinaryString<int>(triple.offset == 0 ? 0 : triple.offset - 1, numberOfBitsForOffset));
        utils::append(bits, getBinaryString<int>(triple.length, numberOfBitsForLength));
        utils::append(bits, getBinaryString<char>(triple.character, constants::BITS_PER_CHARACTER_LZ77));

        return bits;
    }
};
