#include <gtest/gtest.h>

#include "common/Packer.cpp"
#include "common/Unpacker.cpp"

const std::string outputFileName = "../../tests/files/coding.txt";

/**
 * Testing packing and unpacking the result of coding with ShannonFano.
 */
TEST(ShannonFanoPacking, ShannonFanoPacking_1) {
    Packer* packer = new Packer(outputFileName);
    Unpacker* unpacker = new Unpacker(outputFileName);

    std::string testString = "Мальчик Финн и собака Джейк живут в доме на дереве, "
                             "расположенном рядом с Конфетным Королевством в Землях Ооо.";
    CharSequence source(testString.begin(), testString.end());

    ShannonFanoCoder* coder = new ShannonFanoCoder(source);
    ShannonFanoCoder::Result result = coder->code(source);

    packer->writeShannonFanoResult(result);
    ShannonFanoCoder::Result unpackedResult = unpacker->readShannonFanoResult();

    EXPECT_EQ(result.codedData, unpackedResult.codedData);
    EXPECT_EQ(result.values.size(), unpackedResult.values.size());
    EXPECT_EQ(result.codes.size(), unpackedResult.codes.size());

    std::map<char, CharSequence> mapBefore = result.asMap();
    std::map<char, CharSequence> mapAfter = unpackedResult.asMap();
    auto condition = [] (decltype(*mapBefore.begin()) a, decltype(a) b)
    { return a.first == b.first; };
    ASSERT_TRUE(std::equal(mapBefore.begin(), mapBefore.end(), mapAfter.begin(), condition));
}




/**
 * Testing packing and unpacking the result of coding with LZ77.
 */
TEST(LZ77Packing, LZ77Packing_1) {
    Packer* packer = new Packer(outputFileName);
    Unpacker* unpacker = new Unpacker(outputFileName);

    int dictSize = 5 * 1024;
    int windowSize = 9 * 1024;

    LZ77Coder* coder = new LZ77Coder(dictSize, windowSize);
    CharSequence data{'s', 's', 's', 'd', 'd', 'd', 'd', '#'};
    std::vector<LZ77Coder::Triple> codedInfo = coder->code(data);

    packer->writeTriples(codedInfo, dictSize, windowSize - dictSize);
    std::vector<LZ77Coder::Triple> unpacked = unpacker->readTriples(dictSize, windowSize - dictSize);

    EXPECT_EQ(unpacked.size(), codedInfo.size());
    for (int i = 0; i < unpacked.size(); ++i) {
        EXPECT_EQ(unpacked[i].character, codedInfo[i].character);
        EXPECT_EQ(unpacked[i].offset, codedInfo[i].offset);
        EXPECT_EQ(unpacked[i].length, codedInfo[i].length);
    }
}

/*
 * Testing packing and unpacking the result of coding with LZW.
 */
TEST(LZWPacking, LZWPacking_1) {
    Packer* packer = new Packer(outputFileName);
    Unpacker* unpacker = new Unpacker(outputFileName);

    LZWCoder* coder = new LZWCoder();
    CharSequence source{'d', 'a', 'd', '_', 'a', '_', 'd', 'a', 'd', 'a', 'd', '_', 'd', 'a', 'd', 'd', 'a'};
    LZWCoder::Result result = coder->code(source);

    packer->writeLZWResult(result);
    LZWCoder::Result unpackedResult = unpacker->readLZWResult();

    EXPECT_EQ(result.codes.size(), unpackedResult.codes.size());
    ASSERT_TRUE(std::equal(result.codes.begin(), result.codes.end(), unpackedResult.codes.begin()));

    EXPECT_EQ(result.dictionary.size(), unpackedResult.dictionary.size());
    auto condition = [] (decltype(*result.dictionary.begin()) a, decltype(a) b)
    { return a.first == b.first; };
    ASSERT_TRUE(std::equal(result.dictionary.begin(), result.dictionary.end(), unpackedResult.dictionary.begin(), condition));
}

