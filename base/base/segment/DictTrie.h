/*
 * CPPJIEBA 
 * modified by qinjian@sogou-inc.com
 */
#ifndef BA_DICT_TRIE_H
#define BA_DICT_TRIE_H

#include <iostream>
#include <fstream>
#include <map>
#include <cstring>
#include <stdint.h>
#include <cmath>
#include <limits>

#include "Trie.h"
#include "TransCode.h"
#include "segment/StringUtil.h"
#include "segment/InitOnOff.h"
#include "util.h"

namespace ba
{
    using namespace ba;

    const double MIN_DOUBLE = -3.14e+100;
    const double MAX_DOUBLE = 3.14e+100;
    const size_t DICT_COLUMN_NUM = 3;
    const char* const UNKNOWN_TAG = "x";

    struct DictUnit
    {
        Unicode word;
        double weight; 
        string tag;
    };

    inline ostream & operator << (ostream& os, const DictUnit& unit)
    {
        string s;
        s << unit.word;
        return os << string_format("%s %s %.3lf", s.c_str(), unit.tag.c_str(), unit.weight);
    }

    typedef map<size_t, const DictUnit*> DagType;

    class DictTrie: public InitOnOff
    {
        public:
            typedef Trie<Unicode::value_type, DictUnit, Unicode, vector<Unicode>, vector<const DictUnit*> > TrieType;

            DictTrie();
            DictTrie(const string& dictPath, const string& userDictPath = "");
            ~DictTrie();
            
            bool init(const string& dictPath, const string& userDictPath = "");

            const DictUnit* find(Unicode::const_iterator begin, Unicode::const_iterator end) const;
            bool find(Unicode::const_iterator begin, Unicode::const_iterator end, DagType& dag, size_t offset = 0) const;

            bool isUserDictSingleChineseWord(const Unicode::value_type& word) const;
            double getMinWeight() const {return _minWeight;};

        private:
            vector<DictUnit> _nodeInfos;
            TrieType * _trie;
            double _minWeight;
            unordered_set<Unicode::value_type> _userDictSingleChineseWord;

            TrieType * _creatTrie(const vector<DictUnit>& dictUnits);
            void _loadUserDict(const string& filePath, double defaultWeight, const string& defaultTag);
            void _loadDict(const string& filePath, vector<DictUnit>& nodeInfos) const;
            double _findMinWeight(const vector<DictUnit>& nodeInfos) const;
            double _findMaxWeight(const vector<DictUnit>& nodeInfos) const;
            void _calculateWeight(vector<DictUnit>& nodeInfos) const;
            void _shrink(vector<DictUnit>& units) const;
    };
}

#endif
