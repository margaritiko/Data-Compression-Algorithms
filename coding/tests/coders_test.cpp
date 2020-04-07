#include <gtest/gtest.h>

#include "coders/ShannonFanoCoder.cpp"
#include "coders/LZWCoder.cpp"
#include "coders/LZ77Coder.cpp"


/**
 * Testing initializer with already calculated number of occurrences of each character in the word.
 */
TEST(ShannonFanoCoder, ShannonFano_1) {
    std::vector<int> numbersOfEachChar{36, 18, 18, 12, 9, 7};
    CharSequence values{'a', 'b', 'c', 'd', 'e', 'f'};

    ShannonFanoCoder* coder = new ShannonFanoCoder(values, numbersOfEachChar);
    ShannonFanoCoder::Result result = coder->code(CharSequence{'a', 'b', 'f', 'e', 'd', 'd', 'd', 'd', 'c'});
    std::shared_ptr<ShannonFanoCoder::Node> tree = ShannonFanoCoder::makeTree(result.asMap());

    CharSequence codedAnswer{'0', '0', '0', '1', '1', '1', '1', '1', '1', '1', '1', '0', '1', '1', '0', '1', '1', '0', '1', '1', '0', '1', '1', '0', '1', '0'};
    EXPECT_EQ(result.codedData, codedAnswer);

    CharSequence encodedAnswer{'a', 'b', 'f', 'e', 'd', 'd', 'd', 'd', 'c'};
    EXPECT_EQ(encodedAnswer, coder->encode(CharSequence{'0', '0', '0', '1', '1', '1', '1', '1', '1', '1', '1', '0', '1', '1', '0', '1', '1', '0', '1', '1', '0', '1', '1', '0', '1', '0'}, tree));
}


/**
 * Testing initializer with given string.
 */
TEST(ShannonFanoCoder, ShannonFano_2) {
    std::string testString = "acccccccccccccccccacaaaababaddddddddbabddddabababaeeeeeebabeeeaaabfffffffabbbbbbbbaaaaaaaaaaaaaaaaaa";
    CharSequence source(testString.begin(), testString.end());

    ShannonFanoCoder* coder = new ShannonFanoCoder(source);
    ShannonFanoCoder::Result result = coder->code(source);
    std::shared_ptr<ShannonFanoCoder::Node> tree = ShannonFanoCoder::makeTree(result.asMap());

    CharSequence answer{'a', 'c', 'f', 'e', 'd', 'd', 'd', 'd', 'b'};
    EXPECT_EQ(answer, coder->encode(CharSequence{'0', '0', '0', '1', '1', '1', '1', '1', '1', '1', '1', '0', '1', '1', '0', '1', '1', '0', '1', '1', '0', '1', '1', '0', '1', '0'}, tree));
}


/**
 * Testing coding and encoding with LZ77.
 */
TEST(LZ77Coder, LZ77_1) {
    LZ77Coder* coder = new LZ77Coder(4000, 5000);
    CharSequence data{'d', 'd', 'd', 'b', 'c', 'c', 'b', 'a', 'a', '#'};
    std::vector<LZ77Coder::Triple> codedInfo = coder->code(data);

    EXPECT_EQ(data, coder->encode(codedInfo));
}


/**
 * Testing coding and encoding with LZW.
 */
TEST(LZWCoder, LZW_1) {
    LZWCoder* coder = new LZWCoder();

    CharSequence source{'d', 'a', 'd', '_', 'a', '_', 'd', 'a', 'd', 'a', 'd', '_', 'd', 'a', 'd', 'd', 'a'};
    LZWCoder::Result result = coder->code(source);

    EXPECT_EQ(source, coder->encode(result));
}