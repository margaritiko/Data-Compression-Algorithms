#include <vector>

using CharSequence = std::vector<char>;

namespace utils {

    static void append(CharSequence &source, const CharSequence &suffix) {
        source.insert(source.end(), suffix.begin(), suffix.end());
    }

    static CharSequence subsequence(const CharSequence &source, int startIndex, int length) {
        return CharSequence(source.begin() + startIndex, source.begin() + startIndex + length);
    }

    static bool isPowerOfTwo(int number) {
        while (number % 2 == 0)
            number /= 2;
        return number == 1;
    }

}

namespace constants {

    const int CHAR_LENGTH_BITS_SF = 8;
    const int NUMBER_OF_CODES_SF = 8;
    const int CODES_LENGTH_BITS_SF = 8;
    const int TOTAL_CODE_LENGTH_BITS_SF = 32;

    const int BITS_PER_CHARACTER_LZ77 = 8;

    const int DICTIONARY_SIZE_LZW = 8;
    const int CHARACTER_BITS_LZW = 8;
    const int NUMBER_OF_CODES_LZW = 32;
    const int CODES_BITS_PRESENT_LZW = 32;

}