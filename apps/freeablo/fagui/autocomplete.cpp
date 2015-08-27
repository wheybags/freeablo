#include "autocomplete.h"

using namespace std;

namespace FAGui
{

    AutoComplete::Trie::Trie()
    {
        mHead = make_shared<TrieNode>();
        mHead->prefixCount = 0;
        mHead->isEnd = false;
    }

    void AutoComplete::Trie::insert(const string& word)
    {
        TrieNode * current = mHead.get();
        current->prefixCount++;

        unsigned int wordLen = word.length();
        for(unsigned int i = 0 ; i < wordLen ; i++ )
        {
            char letter = word[i];
            if(current->child[letter] == NULL)
                current->child[letter] = make_shared<TrieNode>();
            current->child[letter]->prefixCount++;
            current = current->child[letter].get();
        }

        current->isEnd = true;
    }

    void AutoComplete::Trie::findWordsWithPrefix(string& prefix, vector<string> & words)
    {
        TrieNode * node = findParentWithPrefix(prefix);
        traverse(prefix, *node, words);
    }

    void AutoComplete::Trie::traverse(std::string& prefix, TrieNode const& node, vector<string> & words)
    {
        if (node.isEnd)
        {
            words.push_back(prefix);
        }

        const TrieNode * current = &node;

        for(map<char, shared_ptr<TrieNode> >::const_iterator it = current->child.begin() ; it != current->child.end() ; it++)
        {
            Trie::TrieNode const* pChild = it->second.get();
            if (pChild)
            {
                prefix.push_back(it->first);
                traverse(prefix, *pChild, words);
                prefix.pop_back();
            }
         }
    }


    AutoComplete::Trie::TrieNode * AutoComplete::Trie::findParentWithPrefix(const string& prefix)
    {
        Trie::TrieNode * current = mHead.get();
        unsigned int prefixLen = prefix.length();
        for(unsigned int i = 0 ; i < prefixLen ; i++ )
        {
            if(current->child[prefix[i]])
                current = current->child[prefix[i]].get();
        }

        return current;
    }

    AutoComplete::AutoComplete()
    {
        prefixChanged();
    }

    void AutoComplete::insertWord(const string& word)
    {
        mTrie.insert(word);
    }

    string AutoComplete::getNextWord(string& prefix)
    {
        if(prefix == "")
        {
            prefixChanged();
            return "";
        }

        if(prefix != mLastPrefix)
        {
            prefixChanged(prefix);
            mTrie.findWordsWithPrefix(prefix, mWords);
        }

        string result = prefix;

        if(mWords.size() > 0)
        {
            result = mWords[mCounter % mWords.size()];
            mCounter++;
        }

        return result;
    }

    void AutoComplete::prefixChanged(string prefix)
    {
        mLastPrefix = prefix;
        mPrefixChanged = true;
        mCounter = 0;
        mWords.clear();
    }
}
