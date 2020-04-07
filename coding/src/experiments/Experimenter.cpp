#include <functional>
#include <map>
#include <iomanip>
#include <cmath>
#include <vector>
#include <sys/stat.h>

#ifndef TIME_MEASUREMENT
#define TIME_MEASUREMENT

#include "Watch.cpp"

#endif

#ifndef COMMON_DECLARATIONS
#define COMMON_DECLARATIONS

#include "../common/declarations.cpp"

#endif

#ifndef FILE_CONVERTER
#define FILE_CONVERTER

#include "../common/Converter.cpp"

#endif

#ifndef CSV_WRITER
#define CSV_WRITER

#include "CSVWriter.cpp"

#endif

#include "../common/Packer.cpp"
#include "../common/Unpacker.cpp"

class Experimenter {

    const std::string pathPrefix = "../cmake-build-debug/src/DATA/";

    const std::vector<std::string> files {
            "1.pptx",
            "2.txt",
            "3.pdf",
            "4.docx",
            "5.jpg",
            "6.jpg",
            "7.BMP",
            "8.bmp",
            "9.dll",
            "10.exe",
            "11.mp4",
            "12.txt"
    };

    const std::vector<std::string> overallHeadings{"S1", "H",
                                                   "SF_S2", "SF_K", "SF_TU", "SF_TP",
                                                   "LZ77_5_S2", "LZ77_5_K", "LZ77_5_TU", "LZ77_5_TP",
                                                   "LZ77_10_S2", "LZ77_10_K", "LZ77_10_TU", "LZ77_10_TP",
                                                   "LZ77_20_S2", "LZ77_20_K", "LZ77_20_TU", "LZ77_20_TP",
                                                   "LZW_S2", "LZW_K", "LZW_TU", "LZW_TP"};

public:

    void overallTesting(const std::string csvFileName) {
        CSVWriter* csvWriter = new CSVWriter(csvFileName);
        csvWriter->addRow(overallHeadings.begin(), overallHeadings.end());

        std::string path = pathPrefix;

        std::function<void(std::string)> shannon_fano_coding = [this, path](std::string sourceFileName) {
            std::cout << "[Shannon-Fano] Preparing packer and getting source\n";

            std::string outputFile = path + this->cutExtension(sourceFileName) + ".shan";
            Packer* packer = new Packer(outputFile);
            CharSequence source = Converter::getInstance().readBinaryFile(path + sourceFileName);

            ShannonFanoCoder* coder = new ShannonFanoCoder(source);
            std::cout << "[Shannon-Fano] Starting coding\n";
            ShannonFanoCoder::Result result = coder->code(source);
            std::cout << "[Shannon-Fano] Have finished coding\n";
            packer->writeShannonFanoResult(result);
            std::cout << "[Shannon-Fano] Finished writing result to file\n";
        };

        std::function<void(std::string)> shannon_fano_decoding = [this, path](std::string fileName) {
            std::string sourceFile = path + this->cutExtension(fileName) + ".shan";
            Unpacker* unpacker = new Unpacker(sourceFile);

            ShannonFanoCoder::Result unpackedResult = unpacker->readShannonFanoResult();
            std::cout << "[Shannon-Fano] Read packed data\n";

            std::shared_ptr<ShannonFanoCoder::Node> tree = ShannonFanoCoder::makeTree(unpackedResult.asMap());

            std::string resultFileName = path + this->cutExtension(fileName) + ".unshan";
            /// Creating new file with the result of encoding.
            Converter::getInstance().writeCharSequenceToABinaryFile(resultFileName,
                                                                    ShannonFanoCoder::encodeSequence(unpackedResult.codedData, tree));
            std::cout << "[Shannon-Fano] Decoded and made new file with the result\n\n";
        };

        std::function<void(std::string)> lz77_5_Coding = [this, path](std::string sourceFileName) {
            std::cout << "[LZ77-5] Preparing packer and getting source\n";

            std::string outputFile = path + this->cutExtension(sourceFileName) + ".lz775";
            Packer* packer = new Packer(outputFile);
            CharSequence data = Converter::getInstance().readBinaryFile(path + sourceFileName);

            int dictSize = 4 * 1024;
            int windowSize = 5 * 1024;
            LZ77Coder* coder = new LZ77Coder(dictSize, windowSize);

            std::cout << "[LZ77-5] Starting coding\n";
            std::vector<LZ77Coder::Triple> codedInfo = coder->code(data);
            std::cout << "[LZ77-5] Have finished coding\n";

            packer->writeTriples(codedInfo, dictSize, windowSize - dictSize);
            std::cout << "[LZ77-5] Finished writing result to file\n";
        };

        std::function<void(std::string)> lz77_5_Decoding = [this, path](std::string fileName) {
            std::string sourceFile = path + this->cutExtension(fileName) + ".lz775";
            Unpacker* unpacker = new Unpacker(sourceFile);

            int dictSize = 4 * 1024;
            int windowSize = 5 * 1024;
            LZ77Coder* coder = new LZ77Coder(dictSize, windowSize);

            std::vector<LZ77Coder::Triple> unpacked = unpacker->readTriples(dictSize, windowSize - dictSize);
            std::cout << "[LZ77-5] Read packed data\n";

            /// Creating new file with the result of encoding.
            std::string resultFileName = path + this->cutExtension(fileName) + ".unlz775";
            Converter::getInstance().writeCharSequenceToABinaryFile(resultFileName,
                                                                    coder->encode(unpacked));
            std::cout << "[LZ77-5] Decoded and made new file with the result\n\n";
        };

        std::function<void(std::string)> lz77_10_Coding = [this, path](std::string sourceFileName) {
            std::cout << "[LZ77-10] Preparing packer and getting source\n";

            std::string outputFile = path + this->cutExtension(sourceFileName) + ".lz7710";
            Packer* packer = new Packer(outputFile);
            CharSequence data = Converter::getInstance().readBinaryFile(path + sourceFileName);

            int dictSize = 8 * 1024;
            int windowSize = 10 * 1024;
            LZ77Coder* coder = new LZ77Coder(dictSize, windowSize);

            std::cout << "[LZ77-10] Starting coding\n";
            std::vector<LZ77Coder::Triple> codedInfo = coder->code(data);
            std::cout << "[LZ77-10] Have finished coding\n";

            packer->writeTriples(codedInfo, dictSize, windowSize - dictSize);
            std::cout << "[LZ77-10] Finished writing result to file\n";
        };

        std::function<void(std::string)> lz77_10_Decoding = [this, path](std::string fileName) {
            std::string sourceFile = path + this->cutExtension(fileName) + ".lz7710";
            Unpacker* unpacker = new Unpacker(sourceFile);

            int dictSize = 8 * 1024;
            int windowSize = 10 * 1024;
            LZ77Coder* coder = new LZ77Coder(dictSize, windowSize);

            std::vector<LZ77Coder::Triple> unpacked = unpacker->readTriples(dictSize, windowSize - dictSize);
            std::cout << "[LZ77-10] Read packed data\n";

            /// Creating new file with the result of encoding.
            std::string resultFileName = path + this->cutExtension(fileName) + ".unlz7710";
            Converter::getInstance().writeCharSequenceToABinaryFile(resultFileName,
                                                                    coder->encode(unpacked));
            std::cout << "[LZ77-10] Decoded and made new file with the result\n\n";
        };

        std::function<void(std::string)> lz77_20_Coding = [this, path](std::string sourceFileName) {
            std::cout << "[LZ77-20] Preparing packer and getting source\n";

            std::string outputFile = path + this->cutExtension(sourceFileName) + ".lz7720";
            Packer* packer = new Packer(outputFile);
            CharSequence data = Converter::getInstance().readBinaryFile(path + sourceFileName);

            int dictSize = 16 * 1024;
            int windowSize = 20 * 1024;
            LZ77Coder* coder = new LZ77Coder(dictSize, windowSize);

            std::cout << "[LZ77-20] Starting coding\n";
            std::vector<LZ77Coder::Triple> codedInfo = coder->code(data);
            std::cout << "[LZ77-20] Have finished coding\n";

            packer->writeTriples(codedInfo, dictSize, windowSize - dictSize);
            std::cout << "[LZ77-20] Finished writing result to file\n";
        };

        std::function<void(std::string)> lz77_20_Decoding = [this, path](std::string fileName) {
            std::string sourceFile = path + this->cutExtension(fileName) + ".lz7720";
            Unpacker* unpacker = new Unpacker(sourceFile);

            int dictSize = 16 * 1024;
            int windowSize = 20 * 1024;
            LZ77Coder* coder = new LZ77Coder(dictSize, windowSize);

            std::vector<LZ77Coder::Triple> unpacked = unpacker->readTriples(dictSize, windowSize - dictSize);
            std::cout << "[LZ77-20] Read packed data\n";

            /// Creating new file with the result of encoding.
            std::string resultFileName = path + this->cutExtension(fileName) + ".unlz7720";
            Converter::getInstance().writeCharSequenceToABinaryFile(resultFileName,
                                                                    coder->encode(unpacked));
            std::cout << "[LZ77-20] Decoded and made new file with the result\n\n";
        };

        std::function<void(std::string)> lzw_coding = [this, path](std::string sourceFileName) {
            std::cout << "[LZW] Preparing packer and getting source\n";

            std::string outputFile = path + this->cutExtension(sourceFileName) + ".lzw";
            Packer* packer = new Packer(outputFile);
            CharSequence source = Converter::getInstance().readBinaryFile(path + sourceFileName);

            LZWCoder* coder = new LZWCoder();

            std::cout << "[LZ7W] Starting coding\n";
            LZWCoder::Result result = coder->code(source);
            std::cout << "[LZ7W] Have finished coding\n";

            packer->writeLZWResult(result);
            std::cout << "[LZW] Finished writing result to file\n";
        };

        std::function<void(std::string)> lzw_decoding = [this, path](std::string fileName) {
            std::string sourceFileName = path + this->cutExtension(fileName) + ".lzw";
            Unpacker* unpacker = new Unpacker(sourceFileName);
            LZWCoder::Result unpackedResult = unpacker->readLZWResult();
            std::cout << "[LZW] Read packed data\n";

            LZWCoder* coder = new LZWCoder();
            /// Creating new file with the result of encoding.
            std::string resultFileName = path + this->cutExtension(fileName) + ".unlzw";
            Converter::getInstance().writeCharSequenceToABinaryFile(resultFileName,
                                                                    coder->encode(unpackedResult));
            std::cout << "[LZW] Decoded and made new file with the result\n\n";
        };

        for (const std::string& fileName: files) {
            std::vector<TestResult> testsResults(5);

            testsResults[0] = launchAlgorithmsTest(fileName, cutExtension(fileName) + ".shan", shannon_fano_coding, shannon_fano_decoding);
            testsResults[1] = launchAlgorithmsTest(fileName, cutExtension(fileName) + ".lz775", lz77_5_Coding, lz77_5_Decoding);
            testsResults[2] = launchAlgorithmsTest(fileName, cutExtension(fileName) + ".lz7710", lz77_10_Coding, lz77_10_Decoding);
            testsResults[3] = launchAlgorithmsTest(fileName, cutExtension(fileName) + ".lz7720", lz77_20_Coding, lz77_20_Decoding);
            testsResults[4] = launchAlgorithmsTest(fileName, cutExtension(fileName) + ".lzw", lzw_coding, lzw_decoding);

            std::vector<double> overallResults;
            overallResults.push_back(getFileSizeInKBytes(pathPrefix + fileName));
            overallResults.push_back(calculateEntropy(pathPrefix + fileName));

            for (TestResult testResult: testsResults) {
                overallResults.push_back(testResult.averageSize);
                overallResults.push_back(testResult.averageSize / overallResults[0]);
                overallResults.push_back(testResult.decodingAverageTime);
                overallResults.push_back(testResult.codingAverageTime);
            }

            csvWriter->addRow(overallResults.begin(), overallResults.end());
        }

    }


    /**
     * Returns file size in bytes.
     * @param fileName The name of file which will be measured.
     */
    long long getFileSize(const std::string& fileName) const {
        char fileNameCharArray[fileName.size() + 1];
        strcpy(fileNameCharArray, fileName.c_str());

        struct stat file;
        stat(fileNameCharArray, &file);

        return file.st_size;
    }


    /**
     * Returns file size in Kilobytes.
     * @param fileName The name of file which will be measured.
     */
    double getFileSizeInKBytes(const std::string& fileName) const {
        return getFileSize(fileName) / 1024.0;
    }


    /**
     * Calculates entropy of given file.
     * @param fileName The name of given file.
     * @return Entropy of file as a feature of a random variable.
     */
    double calculateEntropy(const std::string& fileName) {
        std::map<char, int> frequency = measureFileInNumbersOfCharacters(fileName);
        double fileSize = static_cast<double>(getFileSize(fileName));

        double entropy = 0;
        for (int i = 0; i <= 255; ++i) {
            if (frequency[i] == 0)
                continue;

            double wi = frequency[i] / fileSize;
            entropy += wi * std::log2(wi);
        }

        return -entropy;
    }


    /**
     * Calculates frequency for each character in file and prints this information.
     * @param fileName The name of file which will be measured.
     */
    void measureFilesPrintResult(const std::string& csvFileName) const {
        std::map<std::string, std::map<char, double>> experiments;

        for (std::string fileName: files) {
            std::map<char, double> frequency = measureFileInCharactersFrequency(pathPrefix + fileName);
            experiments[fileName] = frequency;
        }

        CSVWriter* csvWriter = new CSVWriter(csvFileName);
        csvWriter->addRow(files.begin(), files.end());

        for (int i = 0; i <= 255; ++i) {
            std::vector<double> row;
            for (std::string fileName: files)
                row.push_back(experiments[fileName][static_cast<char>(i)]);

            csvWriter->addRow(row.begin(), row.end());
        }
    }


private:

    struct TestResult {
        TestResult(): codingAverageTime(0), decodingAverageTime(0), averageSize(0) {}
        TestResult(double codingAverageTime, double decodingAverageTime, double averageSize):
                   codingAverageTime(codingAverageTime), decodingAverageTime(decodingAverageTime), averageSize(averageSize)
        {}

        double codingAverageTime;
        double decodingAverageTime;
        double averageSize;
    };

    /**
     * Variable which is used for time measurements.
     */
    Watch watch;


    /**
     * Run given procedure and measures time while it is running.
     * @param Procedure to be launched.
     * @return Time of running given procedure.
     */
    double runIterationWithMeasuringSeconds(const std::function<void(std::string)>& iteration,
                                            const std::string& fileName) {
        watch.start();
        iteration(fileName);
        watch.stop();

        return watch.elapsedSeconds();
    }


    /**
     * Cuts suffix with extension.
     * @param path Full file name with extension.
     * @return File name without extension.
     */
    std::string cutExtension(const std::string& path) {
        int index = path.find(".");
        assert(index != -1);

        return path.substr(0, index);
    }


    /**
     * Calculates for each character how many times it was appeared in the file.
     * @param fileName The name of file to be measured.
     * @return Map, with characters as keys and the number of times they met in the text as values.
     */
    std::map<char, int> measureFileInNumbersOfCharacters(const std::string& fileName) const {
        CharSequence sequence = Converter::getInstance().readBinaryFile(fileName);
        std::map<char, int> numberOfCharacters;
        for (const char character: sequence)
            numberOfCharacters[character] += 1;

        return numberOfCharacters;
    }


    /**
     * Launches test with given procedure for specified file name.
     * @param fileName Needs for packing as source file.
     * Also using for unpacking for generating new file name for decoding result.
     * @param codingAndPacking Procedure which do coding and packing the result.
     * @param unpackingAndDecoding Procedure which do unpacking the result and decoding.
     */
    TestResult launchAlgorithmsTest(const std::string& fileName, const std::string& outputFileName,
                              const std::function<void(std::string)>& codingAndPacking,
                              const std::function<void(std::string)>& unpackingAndDecoding) {

        std::pair<double, double> packingResult = makeFullTest(codingAndPacking, fileName, outputFileName);
        TestResult testResult = TestResult(packingResult.first,
                makeTimeTest(unpackingAndDecoding, fileName), packingResult.second);
        std::cout << "[RESULT] coding " << testResult.codingAverageTime <<
                     ", decoding " << testResult.decodingAverageTime << ", size " << testResult.averageSize << "\n\n";

        return testResult;
    }


    /**
     * Measures time of given iteration and size of output file, which is launching given number of times.
     * @param numberOfTimesToRun How many times given procedure will be launched.
     * @param iteration Procedure which will be launched.
     * @return The average time of running given procedure and the average size of output file.
     */
    std::pair<double, double> makeFullTest(const std::function<void(std::string)>& iteration, const std::string& testFileName,
                                           const std::string& outputFileName, size_t numberOfTimesToRun = 10) {
        double totalSeconds = 0;
        double totalSize = 0;

        for (size_t step = 0; step < numberOfTimesToRun; ++step) {
            std::cout << "\n[STEP " << step + 1 << "]\n";
            totalSeconds += runIterationWithMeasuringSeconds(iteration, testFileName);
            totalSize += getFileSizeInKBytes(outputFileName);
        }

        return std::make_pair(totalSeconds / numberOfTimesToRun, totalSize / numberOfTimesToRun);
    }

    /**
     * Measures time of given iteration, which is launching given number of times.
     */
    double makeTimeTest(const std::function<void(std::string)>& iteration,
                        const std::string& testFileName, size_t numberOfTimesToRun = 10) {
        double totalSeconds = 0;

        for (size_t step = 0; step < numberOfTimesToRun; ++step)
            totalSeconds += runIterationWithMeasuringSeconds(iteration, testFileName);

        return totalSeconds / numberOfTimesToRun;
    }


    /**
     * Calculates for each character its frequency in the file.
     * @param fileName The name of file to be measured.
     * @return Map, with characters as keys and the frequency as values.
     */
    std::map<char, double> measureFileInCharactersFrequency(const std::string& fileName) const {
        std::map<char, int> numberOfCharacters = measureFileInNumbersOfCharacters(fileName);
        std::map<char, double> frequency;
        double fileSize = getFileSize(fileName);

        for (std::pair<char, int> figure: numberOfCharacters) {
            frequency[figure.first] = figure.second / fileSize;
        }

        return frequency;
    }
};
