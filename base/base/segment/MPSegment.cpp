/*
 * CPPJIEBA 
 * modified by qinjian@sogou-inc.com
 */
#include "MPSegment.h"

using namespace ba;

bool MPSegment::init(const string& dictPath, const string& userDictPath)
{       
	if(_getInitFlag())
	{
		INFO("MPSegment::init, MPSegment have already inited!");
		return false; 
	}
	assert(_dictTrie.init(dictPath, userDictPath));
	INFO("MPSegment::init, MPSegment init ok!");
	return _setInitFlag(true);
}

bool MPSegment::isUserDictSingleChineseWord(const Unicode::value_type & value) const
{
	return _dictTrie.isUserDictSingleChineseWord(value);
}

bool MPSegment::cut(Unicode::const_iterator begin, Unicode::const_iterator end, vector<string>& res)
{
	string log_info;

	assert(_getInitFlag());
	if(begin == end)
	{
		return false;
	}

	vector<Unicode> words;
	words.reserve(end - begin);
	if(!cut(begin, end, words))
	{
		return false;
	}
	size_t offset = res.size();
	res.resize(res.size() + words.size());
	for(size_t i = 0; i < words.size(); i++)
	{
		if(!TransCode::encode(words[i], res[i + offset]))
		{
			INFO("MPSegment::cut, encode error!");
			res[i + offset].clear();
		}
	}
	log_info << res;
	INFO("after MPSegment: %s", log_info.c_str());
	return true;
}

bool MPSegment::cut(Unicode::const_iterator begin , Unicode::const_iterator end, vector<Unicode>& res)
{
	if(end == begin)
	{
		return false;
	}
	assert(_getInitFlag());
	vector<SegmentChar> segmentChars(end - begin);

	//calc DAG
	//O(n^2)
	for(size_t i = 0; i < segmentChars.size(); i ++)
	{
		segmentChars[i].uniCh = *(begin + i);
		segmentChars[i].dag.clear();
		_dictTrie.find(begin + i, end, segmentChars[i].dag, i);
		segmentChars[i].dag.insert(make_pair<DagType::key_type, DagType::mapped_type>(i, NULL));
	}

	_calcDP(segmentChars);

	if(!_cut(segmentChars, res))
	{
		INFO("MPSegment::cut, _cut error!");
		return false;
	}

	return true;
}

void MPSegment::_calcDP(vector<SegmentChar>& SegmentChars) const
{
	size_t nextPos;
	const DictUnit* p;
	double val;

	//O(n^2)
	for(int i = SegmentChars.size() - 1; i >= 0; i--)
	{
		SegmentChars[i].pInfo = NULL;
		SegmentChars[i].weight = MIN_DOUBLE;
		for(DagType::const_iterator it = SegmentChars[i].dag.begin(); it != SegmentChars[i].dag.end(); it++)
		{
			nextPos = it->first;
			p = it->second;
			val = 0.0;
			if(nextPos + 1 < SegmentChars.size())
			{
				val += SegmentChars[nextPos + 1].weight;
			}

			if(p)
			{
				val += p->weight;
			}
			else
			{
				val += _dictTrie.getMinWeight();
			}
			if(val > SegmentChars[i].weight)
			{
				SegmentChars[i].pInfo = p;
				SegmentChars[i].weight = val;
			}
		}
	}
}

bool MPSegment::_cut(const vector<SegmentChar>& segmentChars, vector<Unicode>& res)
{
	size_t i = 0;
	while(i < segmentChars.size())
	{
		const DictUnit* p = segmentChars[i].pInfo;
		if(p)
		{
			res.push_back(p->word);
			i += p->word.size();
		}
		else//single chinese word
		{
			res.push_back(Unicode(1, segmentChars[i].uniCh));
			i++;
		}
	}
	return true;
}

