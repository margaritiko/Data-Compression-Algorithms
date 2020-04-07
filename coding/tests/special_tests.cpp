#include <gtest/gtest.h>

#include "common/Packer.cpp"
#include "common/Unpacker.cpp"

const std::string outputFileName = "../../tests/files/special_coding.txt";
const std::string commonSourcePrefix = "../../tests/files/small-test-data-sources/";
const std::string commonResultsPrefix = "../../tests/files/small-test-data-results/";

const std::string jpgFileName = "cat.jpg";
const std::string txtFileName = "cat.txt";
const std::string pptxFileName = "cat.pptx";
const std::string docxFileName = "cat.docx";
const std::string bmpFileName = "shapes.bmp";
const std::string exeFileName = "cat.exe";


using ShannonFanoResults = std::pair<ShannonFanoCoder::Result, ShannonFanoCoder::Result>;
using LZ77Results = std::pair<std::vector<LZ77Coder::Triple>, std::vector<LZ77Coder::Triple>>;
using LZWResults = std::pair<LZWCoder::Result, LZWCoder::Result>;


ShannonFanoResults makeShannonFanoResult(const std::string& sourceName) {
    Packer* packer = new Packer(outputFileName);
    Unpacker* unpacker = new Unpacker(outputFileName);

    CharSequence source = Converter::getInstance().readBinaryFile(commonSourcePrefix + sourceName);

    ShannonFanoCoder* coder = new ShannonFanoCoder(source);
    ShannonFanoCoder::Result result = coder->code(source);

    packer->writeShannonFanoResult(result);
    ShannonFanoCoder::Result unpackedResult = unpacker->readShannonFanoResult();
    std::shared_ptr<ShannonFanoCoder::Node> tree = ShannonFanoCoder::makeTree(unpackedResult.asMap());

    /// Creating new file with the result of encoding.
    Converter::getInstance().writeCharSequenceToABinaryFile(commonResultsPrefix + "sf+" + sourceName,
                                                            coder->encode(unpackedResult.codedData, tree));

    return std::make_pair(result, unpackedResult);
}


LZ77Results makeLZ77Results(const std::string& sourceName) {
    Packer* packer = new Packer(outputFileName);
    Unpacker* unpacker = new Unpacker(outputFileName);

    int dictSize = 5 * 1024;
    int windowSize = 9 * 1024;

    LZ77Coder* coder = new LZ77Coder(dictSize, windowSize);
    CharSequence data = Converter::getInstance().readBinaryFile(commonSourcePrefix + sourceName);
    std::vector<LZ77Coder::Triple> codedInfo = coder->code(data);

    packer->writeTriples(codedInfo, dictSize, windowSize - dictSize);
    std::vector<LZ77Coder::Triple> unpacked = unpacker->readTriples(dictSize, windowSize - dictSize);

    /// Creating new file with the result of encoding.
    Converter::getInstance().writeCharSequenceToABinaryFile(commonResultsPrefix + "lz77+" + sourceName,
                                                            coder->encode(unpacked));

    return std::make_pair(codedInfo, unpacked);
}


LZWResults makeLZWResults(const std::string& sourceName) {
    Packer* packer = new Packer(outputFileName);
    Unpacker* unpacker = new Unpacker(outputFileName);

    LZWCoder* coder = new LZWCoder();
    CharSequence source = Converter::getInstance().readBinaryFile(commonSourcePrefix + sourceName);
    LZWCoder::Result result = coder->code(source);

    packer->writeLZWResult(result);
    LZWCoder::Result unpackedResult = unpacker->readLZWResult();

    /// Creating new file with the result of encoding.
    Converter::getInstance().writeCharSequenceToABinaryFile(commonResultsPrefix + "lzw+" + sourceName,
                                                            coder->encode(unpackedResult));

    return std::make_pair(result, unpackedResult);
}


// =============================
// Coding images
// =============================


TEST(ShannonFano, ShannonFanoImagesPacking_1) {
    ShannonFanoResults results = makeShannonFanoResult(jpgFileName);

    EXPECT_EQ(results.first.codedData, results.second.codedData);
    EXPECT_EQ(results.first.values.size(), results.second.values.size());
    EXPECT_EQ(results.first.codes.size(), results.second.codes.size());

    std::map<char, CharSequence> mapBefore = results.first.asMap();
    std::map<char, CharSequence> mapAfter = results.second.asMap();
    auto condition = [] (decltype(*mapBefore.begin()) a, decltype(a) b)
    { return a.first == b.first; };
    ASSERT_TRUE(std::equal(mapBefore.begin(), mapBefore.end(), mapAfter.begin(), condition));
}


TEST(LZ77, LZ77ImagesPacking_1) {
    LZ77Results results = makeLZ77Results(jpgFileName);

    EXPECT_EQ(results.second.size(), results.first.size());
    for (int i = 0; i < results.second.size(); ++i) {
        EXPECT_EQ(results.second[i].character, results.first[i].character);
        EXPECT_EQ(results.second[i].offset, results.first[i].offset);
        EXPECT_EQ(results.second[i].length, results.first[i].length);
    }
}


TEST(LZW, LZWImagesPacking_1) {
    LZWResults results = makeLZWResults(jpgFileName);

    EXPECT_EQ(results.first.codes.size(), results.second.codes.size());
    ASSERT_TRUE(std::equal(results.first.codes.begin(), results.first.codes.end(), results.second.codes.begin()));

    EXPECT_EQ(results.first.dictionary.size(), results.second.dictionary.size());
    auto condition = [] (decltype(*results.first.dictionary.begin()) a, decltype(a) b)
    { return a.first == b.first; };
    ASSERT_TRUE(std::equal(results.first.dictionary.begin(), results.first.dictionary.end(), results.second.dictionary.begin(), condition));
}


// =============================
// Coding txt files
// =============================


TEST(ShannonFano, ShannonFanoTxtPacking_1) {
    ShannonFanoResults results = makeShannonFanoResult(txtFileName);

    EXPECT_EQ(results.first.codedData, results.second.codedData);
    EXPECT_EQ(results.first.values.size(), results.second.values.size());
    EXPECT_EQ(results.first.codes.size(), results.second.codes.size());

    std::map<char, CharSequence> mapBefore = results.first.asMap();
    std::map<char, CharSequence> mapAfter = results.second.asMap();
    auto condition = [] (decltype(*mapBefore.begin()) a, decltype(a) b)
    { return a.first == b.first; };
    ASSERT_TRUE(std::equal(mapBefore.begin(), mapBefore.end(), mapAfter.begin(), condition));
}


TEST(LZ77, LZ77TxtPacking_1) {
    LZ77Results results = makeLZ77Results(txtFileName);

    EXPECT_EQ(results.second.size(), results.first.size());
    for (int i = 0; i < results.second.size(); ++i) {
        EXPECT_EQ(results.second[i].character, results.first[i].character);
        EXPECT_EQ(results.second[i].offset, results.first[i].offset);
        EXPECT_EQ(results.second[i].length, results.first[i].length);
    }
}


TEST(LZW, LZWTxtPacking_1) {
    LZWResults results = makeLZWResults(txtFileName);

    EXPECT_EQ(results.first.codes.size(), results.second.codes.size());
    ASSERT_TRUE(std::equal(results.first.codes.begin(), results.first.codes.end(), results.second.codes.begin()));

    EXPECT_EQ(results.first.dictionary.size(), results.second.dictionary.size());
    auto condition = [] (decltype(*results.first.dictionary.begin()) a, decltype(a) b)
    { return a.first == b.first; };
    ASSERT_TRUE(std::equal(results.first.dictionary.begin(), results.first.dictionary.end(), results.second.dictionary.begin(), condition));
}


// =============================
// Coding docs files
// =============================


TEST(ShannonFano, ShannonFanoDocxPacking_1) {
    ShannonFanoResults results = makeShannonFanoResult(docxFileName);

    EXPECT_EQ(results.first.codedData, results.second.codedData);
    EXPECT_EQ(results.first.values.size(), results.second.values.size());
    EXPECT_EQ(results.first.codes.size(), results.second.codes.size());

    std::map<char, CharSequence> mapBefore = results.first.asMap();
    std::map<char, CharSequence> mapAfter = results.second.asMap();
    auto condition = [] (decltype(*mapBefore.begin()) a, decltype(a) b)
    { return a.first == b.first; };
    ASSERT_TRUE(std::equal(mapBefore.begin(), mapBefore.end(), mapAfter.begin(), condition));
}


TEST(LZ77, LZ77DocxPacking_1) {
    LZ77Results results = makeLZ77Results(docxFileName);

    EXPECT_EQ(results.second.size(), results.first.size());
    for (int i = 0; i < results.second.size(); ++i) {
        EXPECT_EQ(results.second[i].character, results.first[i].character);
        EXPECT_EQ(results.second[i].offset, results.first[i].offset);
        EXPECT_EQ(results.second[i].length, results.first[i].length);
    }
}


TEST(LZW, LZWDocxPacking_1) {
    LZWResults results = makeLZWResults(docxFileName);

    EXPECT_EQ(results.first.codes.size(), results.second.codes.size());
    ASSERT_TRUE(std::equal(results.first.codes.begin(), results.first.codes.end(), results.second.codes.begin()));

    EXPECT_EQ(results.first.dictionary.size(), results.second.dictionary.size());
    auto condition = [] (decltype(*results.first.dictionary.begin()) a, decltype(a) b)
    { return a.first == b.first; };
    ASSERT_TRUE(std::equal(results.first.dictionary.begin(), results.first.dictionary.end(), results.second.dictionary.begin(), condition));
}


// =============================
// Coding exe files
// =============================


TEST(ShannonFano, ShannonFanoExePacking_1) {
    ShannonFanoResults results = makeShannonFanoResult(exeFileName);

    EXPECT_EQ(results.first.codedData, results.second.codedData);
    EXPECT_EQ(results.first.values.size(), results.second.values.size());
    EXPECT_EQ(results.first.codes.size(), results.second.codes.size());

    std::map<char, CharSequence> mapBefore = results.first.asMap();
    std::map<char, CharSequence> mapAfter = results.second.asMap();
    auto condition = [] (decltype(*mapBefore.begin()) a, decltype(a) b)
    { return a.first == b.first; };
    ASSERT_TRUE(std::equal(mapBefore.begin(), mapBefore.end(), mapAfter.begin(), condition));
}


TEST(LZ77, LZ77ExePacking_1) {
    LZ77Results results = makeLZ77Results(exeFileName);

    EXPECT_EQ(results.second.size(), results.first.size());
    for (int i = 0; i < results.second.size(); ++i) {
        EXPECT_EQ(results.second[i].character, results.first[i].character);
        EXPECT_EQ(results.second[i].offset, results.first[i].offset);
        EXPECT_EQ(results.second[i].length, results.first[i].length);
    }
}


TEST(LZW, LZWExePacking_1) {
    LZWResults results = makeLZWResults(exeFileName);

    EXPECT_EQ(results.first.codes.size(), results.second.codes.size());
    ASSERT_TRUE(std::equal(results.first.codes.begin(), results.first.codes.end(), results.second.codes.begin()));

    EXPECT_EQ(results.first.dictionary.size(), results.second.dictionary.size());
    auto condition = [] (decltype(*results.first.dictionary.begin()) a, decltype(a) b)
    { return a.first == b.first; };
    ASSERT_TRUE(std::equal(results.first.dictionary.begin(), results.first.dictionary.end(), results.second.dictionary.begin(), condition));
}


// =============================
// Coding pptx files
// =============================


TEST(ShannonFano, ShannonFanoPptxPacking_1) {
    ShannonFanoResults results = makeShannonFanoResult(pptxFileName);

    EXPECT_EQ(results.first.codedData, results.second.codedData);
    EXPECT_EQ(results.first.values.size(), results.second.values.size());
    EXPECT_EQ(results.first.codes.size(), results.second.codes.size());

    std::map<char, CharSequence> mapBefore = results.first.asMap();
    std::map<char, CharSequence> mapAfter = results.second.asMap();
    auto condition = [] (decltype(*mapBefore.begin()) a, decltype(a) b)
    { return a.first == b.first; };
    ASSERT_TRUE(std::equal(mapBefore.begin(), mapBefore.end(), mapAfter.begin(), condition));
}


TEST(LZ77, LZ77PptxPacking_1) {
    LZ77Results results = makeLZ77Results(pptxFileName);

    EXPECT_EQ(results.second.size(), results.first.size());
    for (int i = 0; i < results.second.size(); ++i) {
        EXPECT_EQ(results.second[i].character, results.first[i].character);
        EXPECT_EQ(results.second[i].offset, results.first[i].offset);
        EXPECT_EQ(results.second[i].length, results.first[i].length);
    }
}


TEST(LZW, LZWPptxPacking_1) {
    LZWResults results = makeLZWResults(pptxFileName);

    EXPECT_EQ(results.first.codes.size(), results.second.codes.size());
    ASSERT_TRUE(std::equal(results.first.codes.begin(), results.first.codes.end(), results.second.codes.begin()));

    EXPECT_EQ(results.first.dictionary.size(), results.second.dictionary.size());
    auto condition = [] (decltype(*results.first.dictionary.begin()) a, decltype(a) b)
    { return a.first == b.first; };
    ASSERT_TRUE(std::equal(results.first.dictionary.begin(), results.first.dictionary.end(), results.second.dictionary.begin(), condition));
}


// =============================
// Coding bmp files
// =============================


TEST(ShannonFano, ShannonFanoBmpPacking_1) {
    ShannonFanoResults results = makeShannonFanoResult(bmpFileName);

    EXPECT_EQ(results.first.codedData, results.second.codedData);
    EXPECT_EQ(results.first.values.size(), results.second.values.size());
    EXPECT_EQ(results.first.codes.size(), results.second.codes.size());

    std::map<char, CharSequence> mapBefore = results.first.asMap();
    std::map<char, CharSequence> mapAfter = results.second.asMap();
    auto condition = [] (decltype(*mapBefore.begin()) a, decltype(a) b)
    { return a.first == b.first; };
    ASSERT_TRUE(std::equal(mapBefore.begin(), mapBefore.end(), mapAfter.begin(), condition));
}


TEST(LZ77, LZ77BmpPacking_1) {
    LZ77Results results = makeLZ77Results(bmpFileName);

    EXPECT_EQ(results.second.size(), results.first.size());
    for (int i = 0; i < results.second.size(); ++i) {
        EXPECT_EQ(results.second[i].character, results.first[i].character);
        EXPECT_EQ(results.second[i].offset, results.first[i].offset);
        EXPECT_EQ(results.second[i].length, results.first[i].length);
    }
}

TEST(LZW, LZWBmpPacking_1) {
    LZWResults results = makeLZWResults(bmpFileName);

    EXPECT_EQ(results.first.codes.size(), results.second.codes.size());
    ASSERT_TRUE(std::equal(results.first.codes.begin(), results.first.codes.end(), results.second.codes.begin()));

    EXPECT_EQ(results.first.dictionary.size(), results.second.dictionary.size());
    auto condition = [] (decltype(*results.first.dictionary.begin()) a, decltype(a) b)
    { return a.first == b.first; };
    ASSERT_TRUE(std::equal(results.first.dictionary.begin(), results.first.dictionary.end(), results.second.dictionary.begin(), condition));
}