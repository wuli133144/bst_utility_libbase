/*
 * CPPJIEBA 
 * modified by qinjian@sogou-inc.com
 */
#include "StopWordDict.h"

using namespace ba;

bool StopWordDict::init(const string& stopWordPath)
{
	_loadStopWordDict(stopWordPath);
	INFO("StopWordDict::init, StopWordDict init ok!");
	return _setInitFlag(true);
}

bool StopWordDict::removeStopWords(vector<string>& srcVec, vector<string>& dstVec, vector<string>& stopWordsVec) const
{
	string src_str;
	string dst_str;
	string stop_str;

	dstVec.clear();
	stopWordsVec.clear();
	dstVec.reserve(srcVec.size());
	stopWordsVec.reserve(srcVec.size());

	for(size_t i = 0; i< srcVec.size(); i++){
		unordered_set<string>::const_iterator it = _stopWords.find(srcVec[i]);
		if(it != _stopWords.end()){
			stopWordsVec.push_back(srcVec[i]);
		}else{
			dstVec.push_back(srcVec[i]);
		}
	}

	dstVec.resize(dstVec.size());
	stopWordsVec.resize(stopWordsVec.size());

	src_str << srcVec;
	dst_str << dstVec;
	stop_str << stopWordsVec;

	INFO("StopWordDict::removeStopWords, src : %s, dst : %s, stop : %s", src_str.c_str(), dst_str.c_str(), stop_str.c_str());
	return true;
}

void StopWordDict::_loadStopWordDict(const string& filePath)
{
	INFO("StopWordDict::_loadStopWordDict,  filePath: %s", filePath.c_str());
	ifstream ifs(filePath.c_str());
	if(!ifs)
	{
		INFO("StopWordDict::_loadStopWordDict, open file error, filePath: %s", filePath.c_str());
		assert(false);
	}
	string line ;
	int line_num = 0;
	while(getline(ifs, line))
	{
		_stopWords.insert(line);
		line_num++;
	}
	assert(_stopWords.size());
	INFO("StopWordDict::_loadStopWordDict, total line num: %d", line_num);
}

