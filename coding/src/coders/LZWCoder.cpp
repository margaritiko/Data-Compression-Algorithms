#pragma once

#include <iostream>
#include <vector>
#include <memory>
#include <map>
#include <set>

class LZWCoder {
public:


    class Node {
    public:
        Node(int index, int character): index(index), character(character) {}

        int index;
        char character;

        std::map<char, std::shared_ptr<Node>> next;
    };


    /* Prefix tree. */
    class Tree {
    public:
        Tree(): numberOfWords(0), root(new Node(-1, '\0')) {}

        int numberOfWords;
        std::shared_ptr<Node> root;
    };


    /// Represents the result of coding as vector with all codes and initial dictionary,
    /// which contains only one-length unique characters of source string.
    struct Result {

        Result() {}

        Result(std::map<int, std::string>& dictionary, std::vector<int>& codes) {
            this->codes = codes;
            this->dictionary = dictionary;
        }

        std::map<int, std::string> dictionary;
        std::vector<int> codes;
    };


    std::string encode(const Result& code) {
        std::string encodedString = "";
        std::map<int, std::string> dictionary = code.dictionary;

        /// Presents the word (without last character and with its index in dictionary),
        /// which will be inserted on the next step, when it will be possible to determine the next character.
        std::pair<std::string, int> notInitializedPair = std::make_pair("", -1);
        std::pair<std::string, int> futureObject = notInitializedPair;

        int lastIndex = static_cast<int>(dictionary.size()) + 1;
        for (int index: code.codes) {

            if (dictionary.find(index) != dictionary.end()) {
                /// First case, when we have already found this word
                if (futureObject != notInitializedPair)
                    dictionary[futureObject.second] = futureObject.first + dictionary[index][0];

            } else {
                /// Second case, when the word which we need to take is current `futureObject`
                dictionary[futureObject.second] = futureObject.first + futureObject.first[0];
            }

            std::string prefix = dictionary[index];
            encodedString += prefix;
            futureObject = std::make_pair(dictionary[index], lastIndex++);
        }


        return encodedString;
    }


    Result code(const std::string& data) {
        /// Fills dictionary with init values (strings with size 1 which represents characters).
        std::shared_ptr<Tree> dictionary = std::shared_ptr<Tree>(new Tree());

        /// Creates a default result, which will be filled with codes later.
        Result codedResult = Result();
        fillDictionary(data, dictionary, codedResult.dictionary);

        int position = 0;
        int sourceSize = static_cast<int>(data.size());
        while (position < sourceSize) {
            std::pair<int, int> updateResult = searchAndInsertNextWord(data, position, dictionary);
            position += updateResult.second - 1;
            codedResult.codes.push_back(updateResult.first);
        }

        return codedResult;
    }

private:
    /*
     * Returns pair, where first number is the index of word which was found
     * and the second number is the length of this word.
     */
    std::pair<int, int> searchAndInsertNextWord(const std::string& source, int position, std::shared_ptr<Tree> dictionary) {
        int sourceSize = static_cast<int>(source.size());
        std::shared_ptr<Node> pointer = dictionary.get()->root;

        int length = 1;
        while (position < sourceSize) {
            if (pointer.get()->next[source[position]] == nullptr) {
                pointer.get()->next[source[position]] = std::shared_ptr<Node>(new Node(++(dictionary.get()->numberOfWords), source[position]));
                break;
            }

            length += 1;
            pointer = pointer.get()->next[source[position]];
            position++;
        }

        return std::make_pair(pointer.get()->index, length);
    }


    /*
     * Fills a dictionary with unique characters of given string.
     * Characters are sorted in ascending order.
     */
    void fillDictionary(const std::string& data, std::shared_ptr<Tree> dictionary, std::map<int, std::string>& map) {
        std::set<char> uniqueCharacters;
        for (char character: data)
            uniqueCharacters.insert(character);

        int sourceSize = static_cast<int>(data.size());
        std::set<char>::iterator ptr = uniqueCharacters.begin();
        for (int index = 1; ptr != uniqueCharacters.end(); ++index, ++ptr) {
            if (dictionary.get()->root.get()->next[*ptr] == nullptr) {
                map[index] = *ptr;
                dictionary.get()->root.get()->next[*ptr] = std::shared_ptr<Node>(new Node(index, *ptr));
                dictionary.get()->numberOfWords++;
            }
        }
    }
};
