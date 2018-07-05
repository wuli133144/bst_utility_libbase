/*
 * CPPJIEBA 
 * modified by qinjian@sogou-inc.com
 */
#include "MixSegment.h"

using namespace ba;

bool MixSegment::init(const string& mpSegDict, const string& hmmSegDict, const string& userDict)
{
	assert(!_getInitFlag());
	if(!_mpSeg.init(mpSegDict, userDict))
	{
		INFO("MixSegment::init, MPSegment init error!");
		return false;
	}
	if(!_hmmSeg.init(hmmSegDict))
	{
		INFO("MixSegment::init, HMMSegment init error!");
		return false;
	}
	INFO("MixSegment::init, MixSegment init ok!");
    
	return _setInitFlag(true);
}

//added by SunWei
bool MixSegment::update(const string& mpSegDict, const string& hmmSegDict, const string& userDict)
{
    _setInitFlag(false);
    _mpSeg = MPSegment();
    _hmmSeg = HMMSegment();

	if(!_mpSeg.init(mpSegDict, userDict))
	{
		INFO("MixSegment::update, MPSegment init error!");
		return false;
	}
	if(!_hmmSeg.init(hmmSegDict))
	{
		INFO("MixSegment::update, HMMSegment init error!");
		return false;
	}
	INFO("MixSegment::update, MixSegment update ok!");
	return _setInitFlag(true);
}


bool MixSegment::cut(Unicode::const_iterator begin, Unicode::const_iterator end, vector<Unicode>& res)
{
	assert(_getInitFlag());
	vector<Unicode> words;
	words.reserve(end - begin);
	if(!_mpSeg.cut(begin, end, words))
	{
		INFO("MixSegment::cut, _mpSeg error!");
		return false;
	}

	vector<Unicode> hmmRes;
	hmmRes.reserve(end - begin);
	Unicode piece;
	piece.reserve(end - begin);
	for (size_t i = 0, j = 0; i < words.size(); i++)
	{
		//if mp get a word, it's ok, put it into result
		if (1 != words[i].size() || (words[i].size() == 1 && _mpSeg.isUserDictSingleChineseWord(words[i][0])))
		{
			res.push_back(words[i]);
			continue;
		}

		// if mp get a single one and it is not in userdict, collect it in sequence
		j = i;
		while (j < words.size() && 1 == words[j].size() && !_mpSeg.isUserDictSingleChineseWord(words[j][0]))
		{
			piece.push_back(words[j][0]);
			j++;
		}

		// cut the sequence with hmm
		if (!_hmmSeg.cut(piece.begin(), piece.end(), hmmRes))
		{
			INFO("MixSegment::cut, _hmmSeg error!");
			piece.clear();
			hmmRes.clear();
			i = j - 1;
			continue;
		}

		//put hmm result to result
		for (size_t k = 0; k < hmmRes.size(); k++)
		{
			res.push_back(hmmRes[k]);
		}

		//clear tmp vars
		piece.clear();
		hmmRes.clear();

		//let i jump over this piece
		i = j - 1;
	}
	return true;
}

bool MixSegment::cut(Unicode::const_iterator begin, Unicode::const_iterator end, vector<string>& res)
{
	assert(_getInitFlag());
	if(begin == end)
	{
		return false;
	}

	vector<Unicode> uRes;
	uRes.reserve(end - begin);
	if (!cut(begin, end, uRes))
	{
		return false;
	}

	size_t offset = res.size();
	res.resize(res.size() + uRes.size());
	for(size_t i = 0; i < uRes.size(); i ++, offset++)
	{
		if(!TransCode::encode(uRes[i], res[offset]))
		{
			INFO("MixSegment::cut, encode error!");
			res[offset].clear();
		}
	}
	return true;
}
