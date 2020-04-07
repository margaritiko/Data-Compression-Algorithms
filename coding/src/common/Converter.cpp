#include <vector>
#include <string>
#include <fstream>

#ifndef COMMON_DECLARATIONS
#define COMMON_DECLARATIONS

#include "declarations.cpp"

#endif

class Converter {

    typedef unsigned char byte;

public:
    static Converter& getInstance() {
        static Converter instance;
        return instance;
    }


    /**
     * Writes given string to the binary file.
     * @param bits A string which contains only `1` or `0`.
     */
    void writeBinaryStringToFile(CharSequence& bits, const std::string& outputFileName) const {
        CharSequence suffix((bits.size() % 8 == 0) ? 0 : (8 - bits.size() % 8), '0');
        bits.insert(bits.end(), suffix.begin(), suffix.end());

        std::ofstream out(outputFileName, std::ios::binary); // Binary mode

        int bitsSize = static_cast<int>(bits.size());

        for (int index = 0; index < bitsSize; index += 8) {
            byte next = packByte(bits, index);
            out.write((char*)&next, sizeof(next));
        }

        out.close();
    }


    void writeCharSequenceToABinaryFile(const std::string& fileName, const CharSequence& source) const {
        std::ofstream out(fileName, std::ios::binary | std::ios::out);
        for (const char& character: source) {
            out.write((char*)&character, sizeof(character));
        }
        out.close();
    }


    CharSequence readBinaryFile(const std::string& fileName) const {
        std::ifstream ifs(fileName);

        std::vector<char> data;
        if (!ifs)
            return data;

        data = std::vector<char>(std::istreambuf_iterator<char>(ifs), std::istreambuf_iterator<char>());

        ifs.close();
        assert(data.size() > 0);
        return data;
    }

private:
    Converter() {}
    Converter(const Converter&);
    Converter& operator=(Converter&);

    /**
     * Looks at a substring of given string with length 8 starting from index `left` and converts it to a byte.
     * @param bits Source string with all bits.
     * @param left Starting index.
     * @return A byte which represents a substring.
     */
    byte packByte(const CharSequence& bits, int left) const {
        assert(left + 8 <= bits.size());

        byte result = 0;
        for (unsigned i = 0; i < 8; ++i) {
            if (bits[left + i] == '1') {
                result |= (1 << (7 - i));
            }
        }

        return result;
    }
};
