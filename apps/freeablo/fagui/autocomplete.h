#pragma once
#include <iostream>
#include <map>
#include <memory>
#include <string>
#include <vector>

namespace FAGui
{

    class AutoComplete
    {

    public:
        AutoComplete();
        void insertWord(const std::string& word);
        std::string getNextWord(std::string& prefix);

    private:
        class Trie
        {
        private:
            struct TrieNode
            {
                bool isEnd;
                int prefixCount;
                std::map<char, std::shared_ptr<TrieNode>> child;
            };

            std::shared_ptr<TrieNode> mHead;

        public:
            Trie();
            void insert(const std::string& word);
            void findWordsWithPrefix(std::string& prefix, std::vector<std::string>& words);

        private:
            void traverse(std::string& prefix, TrieNode const& node, std::vector<std::string>& words);
            TrieNode* findParentWithPrefix(const std::string& prefix);
        };

    private:
        void prefixChanged(std::string prefix = "");

        Trie mTrie;
        std::string mLastPrefix;
        bool mPrefixChanged;
        unsigned int mCounter;
        std::vector<std::string> mWords;
    };
}
