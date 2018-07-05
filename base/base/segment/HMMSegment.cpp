/*
 * CPPJIEBA 
 * modified by qinjian@sogou-inc.com
 */
#include "HMMSegment.h"

using namespace ba;

bool HMMSegment::init(const string& filePath)
{
	if(_getInitFlag())
	{
		INFO("HMMSegment::init, HMMSegment have already inited!");
		return false;
	}
	memset(_startProb, 0, sizeof(_startProb));
	memset(_transProb, 0, sizeof(_transProb));
	_statMap[0] = 'B';
	_statMap[1] = 'E';
	_statMap[2] = 'M';
	_statMap[3] = 'S';
	_emitProbVec.push_back(&_emitProbB);
	_emitProbVec.push_back(&_emitProbE);
	_emitProbVec.push_back(&_emitProbM);
	_emitProbVec.push_back(&_emitProbS);
	if(!_setInitFlag(_loadModel(filePath.c_str())))
	{
		INFO("HMMSegment::init, _loadModel failed, model file: %s", filePath.c_str());
		return false;
	}
	INFO("HMMSegment::init, HMMSegment init ok!");
	return true;
}

bool HMMSegment::cut(Unicode::const_iterator begin, Unicode::const_iterator end, vector<Unicode>& res)
{
	Unicode::const_iterator left = begin;
	Unicode::const_iterator right = begin;
	while(right != end)
	{
		if(*right < 0x80)//ascii 
		{
			if(left != right && !_cut(left, right, res))
			{
				return false;
			}
			left = right;
			while(*right < 0x80 && right != end)
			{
				right++;
			}
			res.push_back(Unicode(left, right));
			left = right;
		}
		else
		{
			right++;
		}
	}
	if(left != right && !_cut(left, right, res))
	{
		return false;
	}
	return true;
}

bool HMMSegment::cut(Unicode::const_iterator begin, Unicode::const_iterator end, vector<string>& res)
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
		if(!TransCode::encode(words[i], res[offset + i]))
		{
			INFO("HMMSegment::cut, encode error!");
			res[offset+i].clear();
		}
	}
	log_info << res;
	INFO("after HMMSegment: %s", log_info.c_str());
	return true;
}

bool HMMSegment::_cut(Unicode::const_iterator begin, Unicode::const_iterator end, vector<Unicode>& res)
{
	assert(_getInitFlag());
	vector<size_t> status;
	if(!_viterbi(begin, end, status))
	{
		INFO("HMMSegment::_cut, _viterbi failed!");
		return false;
	}

	Unicode::const_iterator left = begin;
	Unicode::const_iterator right;
	for(size_t i = 0; i < status.size(); i++)
	{
		if(status[i] & 0x01) //if(E == status[i] || S == status[i])
		{
			right = begin + i + 1;
			res.push_back(Unicode(left, right));
			left = right;
		}
	}
	return true;
}

bool HMMSegment::_viterbi(Unicode::const_iterator begin, Unicode::const_iterator end, vector<size_t>& status)const
{
	if(begin == end)
	{
		return false;
	}

	size_t Y = STATUS_SUM;
	size_t X = end - begin;

	size_t XYSize = X * Y;
	size_t now, old, stat;
	double tmp, endE, endS;

	vector<int> path(XYSize);
	vector<double> weight(XYSize);

	//start
	for(size_t y = 0; y < Y; y++)
	{
		weight[0 + y * X] = _startProb[y] + _getEmitProb(_emitProbVec[y], *begin, MIN_DOUBLE);
		path[0 + y * X] = -1;
	}


	double emitProb;

	for(size_t x = 1; x < X; x++)
	{
		for(size_t y = 0; y < Y; y++)
		{
			now = x + y*X;
			weight[now] = MIN_DOUBLE;
			path[now] = E; // warning
			emitProb = _getEmitProb(_emitProbVec[y], *(begin+x), MIN_DOUBLE);
			for(size_t preY = 0; preY < Y; preY++)
			{
				old = x - 1 + preY * X;
				tmp = weight[old] + _transProb[preY][y] + emitProb;
				if(tmp > weight[now])
				{
					weight[now] = tmp;
					path[now] = preY;
				}
			}
		}
	}

	endE = weight[X-1+E*X];
	endS = weight[X-1+S*X];
	stat = 0;
	if(endE >= endS)
	{
		stat = E;
	}
	else
	{
		stat = S;
	}

	status.resize(X);
	for(int x = X -1 ; x >= 0; x--)
	{
		status[x] = stat;
		stat = path[x + stat*X];
	}

	return true;
}

bool HMMSegment::_loadModel(const char* const filePath)
{
	INFO("HMMSegment::_loadModel, start load model!");
	string line;
	vector<string> tmp;

	ifstream ifile(filePath);
	if(!ifile){
		INFO("HMMSegment::_loadModel, open model file error, file: %s", filePath);
		return false;
	}

	//load _startProb
	if(!_getLine(ifile, line))
	{
		INFO("HMMSegment::_loadModel, read startProb error, file: %s", filePath);
		return false;
	}
	split(line, tmp, " ");
	if(tmp.size() != STATUS_SUM)
	{
		INFO("HMMSegment::_loadModel, startProb illegal!");
		return false;
	}
	for(size_t j = 0; j< tmp.size(); j++)
	{
		_startProb[j] = atof(tmp[j].c_str());
		//cout<<_startProb[j]<<endl;
	}

	//load _transProb
	for(size_t i = 0; i < STATUS_SUM; i++)
	{
		if(!_getLine(ifile, line))
		{
			return false;
		}
		split(line, tmp, " ");
		if(tmp.size() != STATUS_SUM)
		{
			INFO("HMMSegment::_loadModel, transProb illegal!");
			return false;
		}
		for(size_t j =0; j < STATUS_SUM; j++)
		{
			_transProb[i][j] = atof(tmp[j].c_str());
			//cout<<_transProb[i][j]<<endl;
		}
	}

	//load _emitProbB
	if(!_getLine(ifile, line) || !_loadEmitProb(line, _emitProbB))
	{
		INFO("HMMSegment::_loadModel, load _emitProbB error!");
		return false;
	}

	//load _emitProbE
	if(!_getLine(ifile, line) || !_loadEmitProb(line, _emitProbE))
	{
		INFO("HMMSegment::_loadModel, load _emitProbE error!");
		return false;
	}

	//load _emitProbM
	if(!_getLine(ifile, line) || !_loadEmitProb(line, _emitProbM))
	{
		INFO("HMMSegment::_loadModel, load _emitProbM error!");
		return false;
	}

	//load _emitProbS
	if(!_getLine(ifile, line) || !_loadEmitProb(line, _emitProbS))
	{
		INFO("HMMSegment::_loadModel, load _emitProbS error!");
		return false;
	}

	INFO("HMMSegment::_loadModel, load hmm model ok!");

	return true;
}

bool HMMSegment::_getLine(ifstream& ifile, string& line)
{
	while(getline(ifile, line))
	{
		trim(line);
		if(line.empty())
		{
			continue;
		}
		if(startsWith(line, "#"))
		{
			continue;
		}
		return true;
	}
	return false;
}

bool HMMSegment::_loadEmitProb(const string& line, EmitProbMap& mp)
{
	if(line.empty())
	{
		return false;
	}
	vector<string> tmp, tmp2;
	Unicode unicode;
	split(line, tmp, ",");
	for(size_t i = 0; i < tmp.size(); i++)
	{
		split(tmp[i], tmp2, ":");
		if(2 != tmp2.size())
		{
			INFO("HMMSegment::_loadEmitProb, _emitProb illegal!");
			return false;
		}
		if(!TransCode::decode(tmp2[0], unicode) || unicode.size() != 1)
		{
			INFO("HMMSegment::_loadEmitProb, TransCode failed!");
			return false;
		}
		mp[unicode[0]] = atof(tmp2[1].c_str());
	}
	return true;
}

double HMMSegment::_getEmitProb(const EmitProbMap* ptMp, uint16_t key, double defVal)const
{
	EmitProbMap::const_iterator cit = ptMp->find(key);
	if(cit == ptMp->end())
	{
		return defVal;
	}
	return cit->second;

}

