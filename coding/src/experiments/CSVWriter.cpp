#include <string>
#include <fstream>

class CSVWriter {

public:
    CSVWriter(std::string filename): fileName(filename), separator(","), numberOfLines(0) {}

    template<typename T>
    void addRow(T first, T last) {
        std::fstream file;
        file.open(fileName, std::ios::out | (numberOfLines ? std::ios::app : std::ios::trunc));

        while (first != last) {
            file << *first;
            if (++first != last)
                file << separator;
        }
        file << "\n";

        numberOfLines++;
        file.close();
    }

private:

    std::string fileName;
    std::string separator;
    int numberOfLines;
};