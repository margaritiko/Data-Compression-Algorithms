#pragma once

#include <iostream>
#include <vector>
#include <memory>
#include <map>
#include <algorithm>

/*
Class which provides methods for coding/encoding data with Shannon-Fano algorithm.
*/
class ShannonFanoCoder {
public:

    struct Result {
        Result(std::vector<char>& values, std::vector<std::string>& codes, std::string& codedData) {
            assert(values.size() == codes.size());
            assert(codedData.size() > 0);

            this->values = values;
            this->codes = codes;
            this->codedData = codedData;
        }

        std::vector<char> values;
        std::vector<std::string> codes;
        std::string codedData;

        std::map<char, std::string> asMap() {
            return Result::map(values, codes);
        }

        static std::map<char, std::string> map(std::vector<char> values, std::vector<std::string> codes) {
            std::map<char, std::string> map;
            int numberOfValues = static_cast<int>(values.size());
            for (int index = 0; index < numberOfValues; ++index)
                map[values[index]] = codes[index];

            return map;
        }
    };

    class Node {
    public:
        /* Only for creating the root of a total tree. */
        Node(): left(nullptr), right(nullptr), flag(false) {}
        Node(char code): left(nullptr), right(nullptr), flag(false) {}

        /* Value which is associated with node only if this node is the end of some word. */
        char value;

        /* `True` if node contains a value, `false` otherwise. */
        bool flag;

        // `0` if left direction has been chosen, `1` otherwise
        std::shared_ptr<Node> left;
        std::shared_ptr<Node> right;
    };


    ShannonFanoCoder(std::string data) {
        std::map<char, int> counter;
        for (char character: data)
            counter[character] += 1;

        std::vector<std::pair<int, char>> tmp;

        int totalSize = static_cast<int>(counter.size());
        tmp.reserve(totalSize);
        for (std::pair<char, int> item: counter)
            tmp.push_back(std::make_pair(item.second, item.first));

        sort(tmp.rbegin(), tmp.rend());

        this->values = std::vector<char>(totalSize);
        this->numberOfMatches = std::vector<int>(totalSize);

        for (int index = 0; index < totalSize; ++index) {
            values[index] = tmp[index].second;
            numberOfMatches[index] = tmp[index].first;
        }

        int numberOfItems = static_cast<int>(numberOfMatches.size());
        bitCodes = std::vector<std::string>(numberOfItems, "");
    }


    ShannonFanoCoder(std::vector<char>& values, std::vector<int>& numberOfMatches) {
        this->values = values;
        this->numberOfMatches = numberOfMatches;

        int numberOfItems = static_cast<int>(numberOfMatches.size());
        bitCodes = std::vector<std::string>(numberOfItems, "");
    }

    /*
    * As a result returns a binary tree with codes in not leaf nodes and values in the leafs.
    * Also fills an array with bitCodes.
    */
    Result code(const std::string& data) {
        int numberOfItems = static_cast<int>(numberOfMatches.size());
        build(0, numberOfItems - 1);

        std::string codedData = "";
        std::map<char, std::string> map = Result::map(values, bitCodes);
        for (const char& character: data)
            codedData += map[character];

        return Result(values, bitCodes, codedData);
    }


    /**
     * Encodes a string from given binary tree with codes.
     */
    std::string encode(const std::string& code, const std::shared_ptr<Node> tree) {
        std::string encodedData = "";
        std::shared_ptr<Node> positionInTree = tree;

        for (int indexOfPointer = 0; indexOfPointer <= static_cast<int>(code.size()); ++indexOfPointer) {
            if (positionInTree.get()->flag) {
                /// Updates encodedData variable which contains current result with new node's value.
                encodedData += positionInTree.get()->value;
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


    std::shared_ptr<Node> makeTree(const std::map<char, std::string>& map) {
        std::shared_ptr<Node> root = std::shared_ptr<Node>(new Node());

        for (const std::pair<char, std::string>& item: map) {
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
            pointer.get()->flag = true;
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
            bitCodes[index] += "0";
        for (int index = middle + 1; index <= end; ++index)
            bitCodes[index] += "1";

        build(start, middle);
        build(middle + 1, end);
    }

    std::vector<char> values;
    std::vector<int> numberOfMatches;
    std::vector<std::string> bitCodes;
};