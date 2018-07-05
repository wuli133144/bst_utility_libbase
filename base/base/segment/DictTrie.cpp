/*
 * CPPJIEBA 
 * modified by qinjian@sogou-inc.com
 */
#include "DictTrie.h"

using namespace ba;

/*
extern CSLog g_imlog;
#ifndef CUSTOM_IMLOG
#define CUSTOM_IMLOG
    #define logInfo(fmt, args...)  g_imlog.Info("<%s>|<%d>|<%s>," fmt, __FILENAME__, __LINE__, __FUNCTION__, ##args)
#endif
*/

DictTrie::DictTrie()
{
	_trie = NULL;
	_minWeight = MAX_DOUBLE;
	_setInitFlag(false);
}

DictTrie::DictTrie(const string& dictPath, const string& userDictPath )
{
	new (this) DictTrie();
	_setInitFlag(init(dictPath, userDictPath));
}

DictTrie::~DictTrie()
{
	if(_trie)
	{
		delete _trie;
	}
}

bool DictTrie::init(const string& dictPath, const string& userDictPath)
{
	assert(!_getInitFlag());
	_loadDict(dictPath, _nodeInfos);
	_calculateWeight(_nodeInfos);
	_minWeight = _findMinWeight(_nodeInfos);

	if(userDictPath.size())
	{
		double maxWeight = _findMaxWeight(_nodeInfos);
		_loadUserDict(userDictPath, maxWeight, UNKNOWN_TAG);
	}
	_shrink(_nodeInfos);
	_trie = _creatTrie(_nodeInfos);
	assert(_trie);
	return _setInitFlag(true);
}

const DictUnit* DictTrie::find(Unicode::const_iterator begin, Unicode::const_iterator end) const
{
	return _trie->find(begin, end);
}

bool DictTrie::find(Unicode::const_iterator begin, Unicode::const_iterator end, DagType& dag, size_t offset ) const
{
	return _trie->find(begin, end, dag, offset);
}

bool DictTrie::isUserDictSingleChineseWord(const Unicode::value_type& word) const
{
	return isIn(_userDictSingleChineseWord, word);
}

DictTrie::TrieType * DictTrie::_creatTrie(const vector<DictUnit>& dictUnits)
{
	assert(dictUnits.size());

	vector<Unicode> words;
	vector<const DictUnit*> valuePointers;

	for(size_t i = 0 ; i < dictUnits.size(); i ++)
	{
		words.push_back(dictUnits[i].word);
		valuePointers.push_back(&dictUnits[i]);
	}

	TrieType * trie = new TrieType(words, valuePointers);
	return trie;
}

void DictTrie::_loadUserDict(const string& filePath, double defaultWeight, const string& defaultTag)
{

	ifstream ifs(filePath.c_str());
	if(!ifs){
		exit(-1);
	}

	string line;
	DictUnit nodeInfo;
	size_t lineno;

	for(lineno = 0; getline(ifs, line); lineno++)
	{
		if(line.size() == 0)
			continue;

		if(!TransCode::decode(line, nodeInfo.word))
		{
			INFO("DictTrie::_loadUserDict, lineno: %d decode error, content: %s", lineno+1, line.c_str());
			continue;
		}
		if(nodeInfo.word.size() == 1)
		{
			_userDictSingleChineseWord.insert(nodeInfo.word[0]);
		}
		nodeInfo.weight = defaultWeight;
		nodeInfo.tag = defaultTag;
		_nodeInfos.push_back(nodeInfo);

		//INFO("DictTrie::_loadUserDict, lineno:%d, content:%s", lineno+1, line.c_str());
	}
	INFO("stop load MPSegment user dict file, total line number: %d", lineno);
}

void DictTrie::_loadDict(const string& filePath, vector<DictUnit>& nodeInfos) const
{
	INFO("start load MPSegment dict file, file: %s", filePath.c_str());

	ifstream ifs(filePath.c_str());
	if(!ifs){
		INFO("DictTrie::_loadDict, open file error, file: %s", filePath.c_str());
		exit(-1);
	}

	string line;
	vector<string> buf;
	DictUnit nodeInfo;
	size_t lineno;

	for(lineno = 0 ; getline(ifs, line); lineno++)
	{
		if(line.size() == 0)
			continue;

		split(line, buf, " ");
		if(buf.size() != DICT_COLUMN_NUM){
			INFO("DictTrie::_loadDict, lineno: %d col number error, content: %s", lineno+1, line.c_str());
			continue;
		}

		if(!TransCode::decode(buf[0], nodeInfo.word))
		{
			INFO("DictTrie::_loadDict, lineno: %d decode error, content: %s", lineno+1, line.c_str());
			continue;
		}
		nodeInfo.weight = atof(buf[1].c_str());
		nodeInfo.tag = buf[2];

		nodeInfos.push_back(nodeInfo);
		//INFO("DictTrie::_loadDict, lineno: %d, content: %s", lineno+1, line.c_str());
	}
	INFO("stop load MPSegment dict file, total line number: %d", lineno);
}

double DictTrie::_findMinWeight(const vector<DictUnit>& nodeInfos) const
{
	double ret = MAX_DOUBLE;
	for(size_t i = 0; i < nodeInfos.size(); i++)
	{
		ret = min(nodeInfos[i].weight, ret);
	}
	INFO("DictTrie::_findMinWeight, min weight: %f", ret);
	return ret;
}

double DictTrie::_findMaxWeight(const vector<DictUnit>& nodeInfos) const
{
	double ret = MIN_DOUBLE;
	for(size_t i = 0; i < nodeInfos.size(); i++)
	{
		ret = max(nodeInfos[i].weight, ret);
	}
	INFO("DictTrie::_findMaxWeight, max weight: %f", ret);
	return ret;
}

void DictTrie::_calculateWeight(vector<DictUnit>& nodeInfos) const
{
	INFO("DictTrie::_calculateWeight...");
	double sum = 0.0;
	for(size_t i = 0; i < nodeInfos.size(); i++)
	{
		sum += nodeInfos[i].weight;
	}
	assert(sum);
	for(size_t i = 0; i < nodeInfos.size(); i++)
	{
		DictUnit& nodeInfo = nodeInfos[i];
		assert(nodeInfo.weight);
        
		nodeInfo.weight = log(nodeInfo.weight/sum);
	}
    INFO("DictTrie::_calculateWeight, over");
}

void DictTrie::_shrink(vector<DictUnit>& units) const
{
	vector<DictUnit>(units.begin(), units.end()).swap(units);
}

