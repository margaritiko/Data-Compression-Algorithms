#include <vector>
#include <memory>
#include <map>
#include <algorithm>

#ifndef COMMON_DECLARATIONS
#define COMMON_DECLARATIONS

#include "../common/declarations.cpp"

#endif

/**
Class which provides methods for coding/encoding data with Shannon-Fano algorithm.
*/
class ShannonFanoCoder {
public:

    struct Result {
        Result(CharSequence& values, std::vector<CharSequence>& codes, CharSequence& codedData) {
            assert(values.size() == codes.size());
            assert(codedData.size() > 0);

            this->values = values;
            this->codes = codes;
            this->codedData = codedData;
        }

        CharSequence values;
        std::vector<CharSequence> codes;
        CharSequence codedData;

        std::map<char, CharSequence> asMap() {
            return Result::map(values, codes);
        }

        static std::map<char, CharSequence> map(const CharSequence& values, const std::vector<CharSequence>& codes) {
            std::map<char, CharSequence> codedMap;
            int numberOfValues = static_cast<int>(values.size());
            for (int index = 0; index < numberOfValues; ++index)
                codedMap[values[index]] = codes[index];

            return codedMap;
        }
    };

    class Node {
    public:
        /* Only for creating the root of a total tree. */
        Node(): left(nullptr), right(nullptr) {}
        Node(char code): left(nullptr), right(nullptr) {}

        /* Value which is associated with node only if this node is the end of some word. */
        char value;

        bool isLeaf() {
            return left == nullptr && right == nullptr;
        }

        // `0` if left direction has been chosen, `1` otherwise
        std::shared_ptr<Node> left;
        std::shared_ptr<Node> right;
    };


    ShannonFanoCoder(CharSequence data) {
        std::map<char, int> counter;
        for (char character: data)
            counter[character] += 1;

        std::vector<std::pair<int, char>> tmp;

        int totalSize = static_cast<int>(counter.size());
        tmp.reserve(totalSize);
        for (std::pair<char, int> item: counter)
            tmp.push_back(std::make_pair(item.second, item.first));

        sort(tmp.rbegin(), tmp.rend());

        this->values = CharSequence(totalSize);
        this->numberOfMatches = std::vector<int>(totalSize);

        for (int index = 0; index < totalSize; ++index) {
            values[index] = tmp[index].second;
            numberOfMatches[index] = tmp[index].first;
        }

        int numberOfItems = static_cast<int>(numberOfMatches.size());
        bitCodes = std::vector<CharSequence>(numberOfItems, CharSequence{});
    }


    ShannonFanoCoder(CharSequence& values, std::vector<int>& numberOfMatches) {
        this->values = values;
        this->numberOfMatches = numberOfMatches;

        int numberOfItems = static_cast<int>(numberOfMatches.size());
        bitCodes = std::vector<CharSequence>(numberOfItems, CharSequence{});
    }

    /**
    * As a result returns a binary tree with codes in not leaf nodes and values in the leafs.
    * Also fills an array with bitCodes.
    */
    Result code(const CharSequence& data) {
        int numberOfItems = static_cast<int>(numberOfMatches.size());
        build(0, numberOfItems - 1);

        CharSequence codedData;
        std::map<char, CharSequence> map = Result::map(values, bitCodes);
        for (const char& character: data)
            codedData.insert(codedData.end(), map[character].begin(), map[character].end());

        return Result(values, bitCodes, codedData);
    }


    /**
     * Encodes a char sequence from given binary tree with codes.
     */
    static CharSequence encodeSequence(const CharSequence& code, const std::shared_ptr<Node> tree) {
        CharSequence encodedData;
        std::shared_ptr<Node> positionInTree = tree;

        for (int indexOfPointer = 0; indexOfPointer <= static_cast<int>(code.size()); ++indexOfPointer) {
            if (positionInTree.get()->isLeaf()) {
                /// Updates encodedData variable which contains current result with new node's value.
                encodedData.push_back(positionInTree.get()->value);
                positionInTree = tree;

                indexOfPointer--;
                continue;
            }

            if (code[indexOfPointer] == '0')
                positionInTree = positionInTree.get()->left;
            else
                positionInTree = positionInTree.get()->right;
        }

        return encodedData;
    }

    CharSequence encode(const CharSequence& code, const std::shared_ptr<Node> tree) {
        return encodeSequence(code, tree);
    }


    static std::shared_ptr<Node> makeTree(const std::map<char, CharSequence>& map) {
        std::shared_ptr<Node> root = std::shared_ptr<Node>(new Node());

        for (const std::pair<char, CharSequence>& item: map) {
            std::shared_ptr<Node> pointer = root;
            for (char bit: item.second) {
                if (bit == '0') {
                    if (pointer.get()->left == nullptr)
                        pointer.get()->left = std::shared_ptr<Node>(new Node());
                    pointer = pointer.get()->left;
                } else {
                    if (pointer.get()->right == nullptr)
                        pointer.get()->right = std::shared_ptr<Node>(new Node());
                    pointer = pointer.get()->right;
                }
            }

            pointer.get()->value = item.first;
        }

        return root;
    }

private:
    void build(int start, int end) {
        if (start >= end)
            return;

        /// Search for the best separation of current sub-array.
        int middle = end - 1;
        long left = 0, right = numberOfMatches[end];
        for (int index = start; index < end; ++index)
            left += numberOfMatches[index];

        long distance = left - right;
        left -= numberOfMatches[middle];
        right += numberOfMatches[middle];

        while (abs(left - right) <= distance) {
            distance = left - right;
            --middle;
            left -= numberOfMatches[middle];
            right += numberOfMatches[middle];
        }

        /// Updates codes.
        for (int index = start; index <= middle; ++index)
            bitCodes[index].push_back('0');
        for (int index = middle + 1; index <= end; ++index)
            bitCodes[index].push_back('1');

        build(start, middle);
        build(middle + 1, end);
    }

    CharSequence values;
    std::vector<int> numberOfMatches;
    std::vector<CharSequence> bitCodes;
};