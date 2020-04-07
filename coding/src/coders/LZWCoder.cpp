#include <vector>
#include <memory>
#include <map>
#include <set>

#ifndef COMMON_DECLARATIONS
#define COMMON_DECLARATIONS

#include "../common/declarations.cpp"

#endif

/**
Class which provides methods for coding/encoding data with LZW algorithm.
*/
class LZWCoder {

public:


    class Node {
    public:
        Node(int index, int character): index(index), character(character) {}

        int index;
        char character;

        std::map<char, std::shared_ptr<Node>> next;
    };


    class Tree {
    public:
        Tree(): numberOfWords(0), root(new Node(-1, '\0')) {}

        int numberOfWords;
        std::shared_ptr<Node> root;
    };


    /** Represents the result of coding as vector with all codes and initial dictionary,
     * which contains only one-length unique characters of source char sequence.
     */
    struct Result {

        Result() {}

        Result(std::map<int, CharSequence>& dictionary, std::vector<int>& codes) {
            this->codes = codes;
            this->dictionary = dictionary;
        }

        std::map<int, CharSequence> dictionary;
        std::vector<int> codes;
    };


    CharSequence encode(const Result& code) {
        CharSequence encoded;
        std::map<int, CharSequence > dictionary = code.dictionary;

        /// Presents the word (without last character and with its index in dictionary),
        /// which will be inserted on the next step, when it will be possible to determine the next character.
        std::pair<CharSequence, int> notInitializedPair = std::make_pair(CharSequence{}, -1);
        std::pair<CharSequence, int> futureObject = notInitializedPair;

        int lastIndex = static_cast<int>(dictionary.size()) + 1;
        for (int index: code.codes) {

            if (dictionary.find(index) != dictionary.end()) {
                /// First case, when we have already found this word
                if (futureObject != notInitializedPair) {
                    CharSequence insertion(futureObject.first);
                    insertion.push_back(dictionary[index][0]);

                    dictionary[futureObject.second] = insertion;
                }

            } else {
                /// Second case, when the word which we need to take is current `futureObject`
                CharSequence insertion(futureObject.first);
                insertion.push_back(futureObject.first[0]);

                dictionary[futureObject.second] = insertion;
            }

            CharSequence prefix = dictionary[index];
            encoded.insert(encoded.end(), prefix.begin(), prefix.end());
            futureObject = std::make_pair(dictionary[index], lastIndex++);
        }


        return encoded;
    }


    Result code(const CharSequence& data) {
        /// Fills dictionary with init values (char sequences with size 1 which represents characters).
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

    /**
     * Returns pair, where first number is the index of word which was found
     * and the second number is the length of this word.
     */
    std::pair<int, int> searchAndInsertNextWord(const CharSequence& source, int position, std::shared_ptr<Tree> dictionary) {
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


    /**
     * Fills a dictionary with unique characters of given char sequence.
     * Characters are sorted in ascending order.
     */
    void fillDictionary(const CharSequence& data, std::shared_ptr<Tree> dictionary, std::map<int, CharSequence>& map) {
        std::set<char> uniqueCharacters;
        for (char character: data)
            uniqueCharacters.insert(character);

        int sourceSize = static_cast<int>(data.size());
        std::set<char>::iterator ptr = uniqueCharacters.begin();
        for (int index = 1; ptr != uniqueCharacters.end(); ++index, ++ptr) {
            if (dictionary.get()->root.get()->next[*ptr] == nullptr) {
                map[index] = CharSequence{*ptr};
                dictionary.get()->root.get()->next[*ptr] = std::shared_ptr<Node>(new Node(index, *ptr));
                dictionary.get()->numberOfWords++;
            }
        }
    }
};
