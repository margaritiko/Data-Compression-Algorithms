#include <vector>
#include <memory>

#ifndef COMMON_DECLARATIONS
#define COMMON_DECLARATIONS

#include "../common/declarations.cpp"

#endif

/**
Class which provides methods for coding/encoding data with LZ77 algorithm.
*/
class LZ77Coder {

public:
    /**
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

    /**
    * Creates a new instance of LZ77 coder with given dictionary's and window's sizes.
    */
    LZ77Coder(int dictSize, int windowSize) {
        this->dictSize = dictSize;
        this->windowSize = windowSize;
    }

    /**
    * Encodes given code, which was generated with LZ77 algorithm.
    */
    CharSequence encode(const std::vector<Triple>& input) {
        CharSequence encoded;
        int pointer = 0;

        for (Triple triple: input) {
            for (size_t index = pointer -  triple.offset; index < pointer - triple.offset + triple.length; ++index)
                encoded.push_back(encoded[index]);

            pointer += triple.length + 1;
            encoded.push_back(triple.character);
        }

        return encoded;
    }

    /**
    * Code given char sequence with LZ77 algorithm.
    */
    std::vector<Triple> code(const CharSequence& data) {
        std::vector<Triple> coded;
        int dataSize = static_cast<int>(data.size());
        coded.reserve(dataSize);

        int dictLeftBorder = -dictSize;
        int dictRightBorder = -1;
        int bufferLeftBorder = 0;
        int bufferRightBorder = windowSize - dictSize - 1;

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

                if (secondIndex >= bufferRightBorder) {
                    secondIndex--;
                    firstIndex--;
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

            dictRightBorder = std::min(dictRightBorder, dataSize - 1);
            bufferRightBorder = std::min(bufferRightBorder, dataSize - 1);
        }

        return coded;
    }

private:

    /* Size of a dictionary in number of characters. */
    int dictSize;

    /* Size of a window in number of characters. */
    int windowSize;
};