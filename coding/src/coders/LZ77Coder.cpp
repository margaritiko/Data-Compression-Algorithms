#pragma once

#include <iostream>
#include <vector>
#include <memory>

class LZ77Coder {

public:
    /*
    * `Triple` struct contains offset, which is calculated from the right to the left, 
    * length of an equal part and the first one chracter, which is different.
    */
    struct Triple {
        int offset;
        int length;
        char character;

        Triple() {
            this->offset = 0;
            this->length = 0;
            this->character = '0';
        }

        Triple(int offset, int length, char character) {
            this->offset = offset;
            this->length = length;
            this->character = character;
        }
    };

    /*
    * Creates a new instance of LZ77 coder with given dictionary's and window's sizes.
    */
    LZ77Coder(int dictSize, int windowSize) {
        this->dictSize = dictSize;
        this->windowSize = windowSize;
    }

    /**
    * Encodes given code, which was generated with LZ77 algorithm.
    */
    std::string encode(const std::vector<Triple>& input) {
        std::string encodedString = "";
        int pointer = 0;

        for (Triple triple: input) {
            for (size_t index = pointer -  triple.offset; index < pointer - triple.offset + triple.length; ++index)
                encodedString += std::string(1, encodedString[index]);

            pointer += triple.length + 1;
            encodedString += std::string(1, triple.character);
        }

        return encodedString;
    }

    /**
    * Code given string with LZ77 algorithm.
    */
    std::vector<Triple> code(const std::string& data) {
        std::vector<Triple> coded;
        coded.reserve(data.size());

        int dictLeftBorder = -dictSize;
        int dictRightBorder = -1;
        int bufferLeftBorder = 0;
        int bufferRightBorder = windowSize - dictSize - 1;

        int dataSize = static_cast<int>(data.size());
        while (bufferLeftBorder < dataSize) {

            int possibleStart = dictLeftBorder;
            int bestStart = dictLeftBorder;
            int bestLength = 0;
            for (; possibleStart <= dictRightBorder; ++possibleStart) {
                if (possibleStart < 0)
                    continue;

                int firstIndex = possibleStart, secondIndex = bufferLeftBorder;

                while (firstIndex >= 0 && secondIndex >= 0 && firstIndex <= bufferRightBorder &&
                       secondIndex <= bufferRightBorder && data[firstIndex] == data[secondIndex]) {
                    firstIndex++;
                    secondIndex++;
                }

                int newLength = firstIndex - possibleStart;
                if (newLength > bestLength) {
                    bestLength = newLength;
                    bestStart = possibleStart;
                }
            }

            int bestOffset = bestLength == 0 ? 0 : bufferLeftBorder - bestStart;
            int nextCharacter = data[bufferLeftBorder + bestLength];
            coded.push_back(Triple(bestOffset, bestLength, nextCharacter));

            dictLeftBorder += bestLength + 1;
            dictRightBorder += bestLength + 1;
            bufferLeftBorder += bestLength + 1;
            bufferRightBorder += bestLength + 1;
        }

        return coded;
    }

private:

    /// Calculations for possible sizes:
    ///
    /// 5 * 1024 = 5 * 2**10 and 4 * 1024 = 2**12
    /// TOTAL: 13 + 12 + 8 = 33
    ///
    /// 10 * 1024 = 5 * 2**11 and 8 * 1024 = 2**13
    /// TOTAL: 14 + 13 + 8 = 35
    ///
    /// 20 * 1024 = 5 * 2**12 and 16 * 1024 = 2**14
    /// TOTAL: 15 + 14 + 8 = 37

    /* Size of a dictionary in number of characters. */
    int dictSize;

    /* Size of a window in number of characters. */
    int windowSize;
};