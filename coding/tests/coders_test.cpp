#include <gtest/gtest.h>

#include "coders/ShannonFanoCoder.cpp"
#include "coders/LZWCoder.cpp"
#include "coders/LZ77Coder.cpp"


/**
 * Testing initializer with already calculated number of occurrences of each character in the word.
 */
TEST(ShannonFanoCoder, ShannonFano_1) {
    std::vector<int> numbersOfEachChar{36, 18, 18, 12, 9, 7};
    std::vector<char> values{'a', 'b', 'c', 'd', 'e', 'f'};

    ShannonFanoCoder* coder = new ShannonFanoCoder(values, numbersOfEachChar);
    ShannonFanoCoder::Result result = coder->code("abfeddddc");
    std::shared_ptr<ShannonFanoCoder::Node> tree = coder->makeTree(result.asMap());

    EXPECT_EQ(result.codedData, "00011111111011011011011010");
    EXPECT_EQ("abfeddddc", coder->encode("00011111111011011011011010", tree));
}


/**
 * Testing initializer with given string.
 */
TEST(ShannonFanoCoder, ShannonFano_2) {
    std::string testString = "acccccccccccccccccacaaaababaddddddddbabddddabababaeeeeeebabeeeaaabfffffffabbbbbbbbaaaaaaaaaaaaaaaaaa";
    ShannonFanoCoder* coder = new ShannonFanoCoder(testString);
    ShannonFanoCoder::Result result = coder->code(testString);
    std::shared_ptr<ShannonFanoCoder::Node> tree = coder->makeTree(result.asMap());

    EXPECT_EQ("acfeddddb", coder->encode("00011111111011011011011010", tree));
}


/**
 * Testing coding and encoding with LZ77.
 */
TEST(LZ77Coder, LZ77_1) {
    LZ77Coder* coder = new LZ77Coder(4000, 5000);
    std::string data = "dddbccbaa привет#";
    std::vector<LZ77Coder::Triple> codedInfo = coder->code(data);

    EXPECT_EQ(data, coder->encode(codedInfo));
}


/**
 * Testing coding and encoding with LZW.
 */
TEST(LZWCoder, LZW_1) {
    LZWCoder* coder = new LZWCoder();

    std::string source = "dad_a_dadad_dadda";
    LZWCoder::Result result = coder->code(source);

    EXPECT_EQ(source, coder->encode(result));
}