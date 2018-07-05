/*
 * api of cutting word
 * written by qinjian@sogou-inc.com
 */
#include "seg_word_api.h"

using namespace ba;

WordCutter* WordCutter::wordCutterInsPtr = NULL;

WordCutter* WordCutter::getInstance()
{
    if (!wordCutterInsPtr)
    {
        wordCutterInsPtr = new WordCutter();
    }
    return wordCutterInsPtr;
}

#if 0
bool WordCutter::init(const string& dirPath, const string& mpDictPath, const string& hmmModelPath, const string& userWordDictPath, const string& userNameDictPath)
{ 
    _mpDictPath = dirPath + "/" + mpDictPath;
    _hmmModelPath = dirPath + "/" + hmmModelPath;
    _userWordDictPath = dirPath + "/" + userWordDictPath;
    _userNameDictPath = dirPath + "/" + userNameDictPath;

    _userWordDictMapUse = new UserDictMap();
    _userWordDictMapUpd = new UserDictMap();

    _userNameDictMapUse = new UserDictMap();
    _userNameDictMapUpd = new UserDictMap();    

    _segmentWordUse = new MixSegment();
    _segmentWordUpd = new MixSegment();

    _segmentNameUse = new MixSegment();
    _segmentNameUpd = new MixSegment();    

    _rwlock = new CRWLock();

	/*bool ret = _segmentUse->init(mpDictPath, 
                             hmmModelPath, 
                             userDictPath);
                             */
    
	if( !update(GROUP_WORD_SENSITIVE) || !update(GROUP_NAME_SENSITIVE) || !update(SINGLE_WORD_SENSITIVE)) 
    {
		INFO("WordCutter::init, segment init error!");
        return false;
	}

	return true;
}
#endif

bool WordCutter::init(const string & dirPath, const string & mpDictPath, const string & hmmModelPath, const string & userWordDictPath, const string & userNameDictPath, const string & singleWordDictPath, const string& liveWordDictPath)
{
	_mpDictPath = dirPath + "/" + mpDictPath;
    _hmmModelPath = dirPath + "/" + hmmModelPath;
    _userWordDictPath = dirPath + "/" + userWordDictPath;
    _userNameDictPath = dirPath + "/" + userNameDictPath;
	_singleWordDictPath = dirPath + "/" + singleWordDictPath;

	_liveWordDictPath = dirPath + "/" + liveWordDictPath;

    _userWordDictMapUse = new UserDictMap();
    _userWordDictMapUpd = new UserDictMap();

    _userNameDictMapUse = new UserDictMap();
    _userNameDictMapUpd = new UserDictMap();

	_singleWordDictMapUse = new UserDictMap();
	_singleWordDictMapUpd = new UserDictMap();

	_liveWordDictMapUse = new UserDictMap();
	_liveWordDictMapUpd = new UserDictMap();

    _segmentWordUse = new MixSegment();
    _segmentWordUpd = new MixSegment();

    _segmentNameUse = new MixSegment();
    _segmentNameUpd = new MixSegment(); 

	_segmentSingleWordUse = new MixSegment();
	_segmentSingleWordUpd = new MixSegment();


	_segmentLiveWordUse = new MixSegment();
	_segmentLiveWordUpd = new MixSegment();

    _rwlock = new CRWLock();

	/*bool ret = _segmentUse->init(mpDictPath, 
                             hmmModelPath, 
                             userDictPath);
                             */
    
	if( !update(GROUP_WORD_SENSITIVE) || !update(GROUP_NAME_SENSITIVE) || !update(SINGLE_WORD_SENSITIVE) || !update(LIVE_WORD_SENSITIVE)) 
    {
		INFO("WordCutter::init, segment init error!");
        return false;
	}

	return true;
}




unsigned int WordCutter::cutWord(const char * inputs, vector<string>& word_vec)
{

	string input_str(inputs);
	_segmentWordUse->cut(input_str, word_vec);

	return word_vec.size();
}

bool WordCutter::hasUserWord(const string& inputStr)
{
	vector<string> wordVec;

	_segmentWordUse->cut(inputStr, wordVec);

    for (vector<string>::iterator it = wordVec.begin(); it != wordVec.end(); ++it)
    {

        if (_userWordDictMapUse->find(*it) != _userWordDictMapUse->end())
        {
            INFO("message(\'%s\') hasSensitiveWord:%s", inputStr.c_str(), (*it).c_str());
            return true;
        }
    }

    return false;
}

bool WordCutter::hasUserName(const string& inputStr)
{
	vector<string> NameVec;

	_segmentNameUse->cut(inputStr, NameVec);

    for (vector<string>::iterator it = NameVec.begin(); it != NameVec.end(); ++it)
    {

        if (_userNameDictMapUse->find(*it) != _userNameDictMapUse->end())
        {
            INFO("message(\'%s\') hasSensitiveName:%s", inputStr.c_str(), (*it).c_str());
            return true;
        }
    }

    return false;
}

bool WordCutter::hasSingleWord(const string & inputStr)
{
	vector<string> wordVec;

	_segmentSingleWordUse->cut(inputStr, wordVec);

    for (vector<string>::iterator it = wordVec.begin(); it != wordVec.end(); ++it)
    {

        if (_singleWordDictMapUse->find(*it) != _singleWordDictMapUse->end())
        {
            INFO("message(\'%s\') hasSensitiveWord:%s", inputStr.c_str(), (*it).c_str());
            return true;
        }
    }

    return false;
}

bool WordCutter::hasLiveWord(const string & inputStr)
{
	vector<string> wordVec;

	_segmentLiveWordUse->cut(inputStr, wordVec);

    for (vector<string>::iterator it = wordVec.begin(); it != wordVec.end(); ++it)
    {

        if (_liveWordDictMapUse->find(*it) != _liveWordDictMapUse->end())
        {
            INFO("message(\'%s\') hasSensitiveWord:%s", inputStr.c_str(), (*it).c_str());
            return true;
        }
    }

    return false;
}


bool WordCutter::update(uint32_t sensiType) 
{
    INFO("WordCutter::udpate.");
	bool ret = false;
    if(sensiType == GROUP_WORD_SENSITIVE)
	{
		ret = _segmentWordUpd->update(_mpDictPath, _hmmModelPath, _userWordDictPath);
	    if (!ret)
	    {
	        INFO("WordCutter::update, segment update failed.");
	        return false;
	    }

	    ret = _updateUserWordDict();
	    if (!ret)
	    {
	        INFO("WordCutter::update, update userDict failed.");
	        return false;
	    }

	    {
	        CAutoRWLock autolock(_rwlock, false); //wlock
	        MixSegment* temp = _segmentWordUse;
	        _segmentWordUse = _segmentWordUpd;
	        _segmentWordUpd = temp;

	        UserDictMap* userDictPtrTemp = _userWordDictMapUse;
	        _userWordDictMapUse = _userWordDictMapUpd;
	        _userWordDictMapUpd = userDictPtrTemp;
	    }
    }
	else if(sensiType == GROUP_NAME_SENSITIVE)
	{
	    ret = _segmentNameUpd->update(_mpDictPath, _hmmModelPath, _userNameDictPath);
	    if (!ret)
	    {
	        INFO("WordCutter::update, segment update failed.");
	        return false;
	    }

	    ret = _updateUserNameDict();
	    if (!ret)
	    {
	        INFO("WordCutter::update, update userDict failed.");
	        return false;
	    }

	    {
	        CAutoRWLock autolock(_rwlock, false); //wlock
	        MixSegment* temp = _segmentNameUse;
	        _segmentNameUse = _segmentNameUpd;
	        _segmentNameUpd = temp;

	        UserDictMap* userDictPtrTemp = _userNameDictMapUse;
	        _userNameDictMapUse = _userNameDictMapUpd;
	        _userNameDictMapUpd = userDictPtrTemp;
	    }
	}
	else if(sensiType == SINGLE_WORD_SENSITIVE)
	{
		ret = _segmentSingleWordUpd->update(_mpDictPath, _hmmModelPath, _singleWordDictPath);
	    if (!ret)
	    {
	        INFO("WordCutter::update, segment update failed.");
	        return false;
	    }

	    ret = _updateSingleWordDict();
	    if (!ret)
	    {
	        INFO("WordCutter::update, update singleDict failed.");
	        return false;
	    }

	    {
	        CAutoRWLock autolock(_rwlock, false); //wlock
	        MixSegment* temp = _segmentSingleWordUse;
	        _segmentSingleWordUse = _segmentSingleWordUpd;
	        _segmentSingleWordUpd = temp;

	        UserDictMap* singleDictPtrTemp = _singleWordDictMapUse;
	        _singleWordDictMapUse = _singleWordDictMapUpd;
	        _singleWordDictMapUpd = singleDictPtrTemp;
	    }
	}

	else if(sensiType == LIVE_WORD_SENSITIVE)
	{
		ret = _segmentLiveWordUpd->update(_mpDictPath, _hmmModelPath, _liveWordDictPath);
	    if (!ret)
	    {
	        INFO("WordCutter::update, segment update failed.");
	        return false;
	    }

		ret = _updateLiveWordDict();
		if (!ret)
		{
			INFO("WordCutter::update, update liveDict failed.");
			return false;
		}

		{
			CAutoRWLock autolock(_rwlock, false); //wlock
			MixSegment* temp = _segmentLiveWordUse;
			_segmentLiveWordUse = _segmentLiveWordUpd;
			_segmentLiveWordUpd = temp;
		
			UserDictMap* liveDictPtrTemp = _liveWordDictMapUse;
			_liveWordDictMapUse = _liveWordDictMapUpd;
			_liveWordDictMapUpd = liveDictPtrTemp;
		}


	}
	else
	{
		INFO("Invalid sensitive word type: %d\n", sensiType);
	}

    return true;
}

//更新用户自定义词典
bool WordCutter::_updateUserWordDict()
{
    _userWordDictMapUpd->clear();

    ifstream ifs(_userWordDictPath.c_str());
	if(!ifs){
		return false;
	}

	string line;

	while( getline(ifs, line) )
	{
		if(line.size() == 0)
			continue;

        vector<string> words;
        ba::split(line, words, " ");

        if(words.size() == 0)
            continue;

        _userWordDictMapUpd->insert(make_pair<string, bool>(words.at(0), true));

	}

    return true;
}

//更新用户自定义词典
bool WordCutter::_updateUserNameDict()
{
    _userNameDictMapUpd->clear();

    ifstream ifs(_userNameDictPath.c_str());
	if(!ifs){
		return false;
	}

	string line;

	while( getline(ifs, line) )
	{
		if(line.size() == 0)
			continue;

        vector<string> words;
        ba::split(line, words, " ");

        if(words.size() == 0)
            continue;

        _userNameDictMapUpd->insert(make_pair<string, bool>(words.at(0), true));

	}

    return true;
}

bool WordCutter::_updateSingleWordDict()
{
	_singleWordDictMapUpd->clear();

    ifstream ifs(_singleWordDictPath.c_str());
	if(!ifs){
		return false;
	}

	string line;

	while( getline(ifs, line) )
	{
		if(line.size() == 0)
			continue;

        vector<string> words;
        ba::split(line, words, " ");

        if(words.size() == 0)
            continue;

        _singleWordDictMapUpd->insert(make_pair<string, bool>(words.at(0), true));

	}

    return true;
}

bool WordCutter::_updateLiveWordDict()
{
	_liveWordDictMapUpd->clear();

    ifstream ifs(_liveWordDictPath.c_str());
	if(!ifs){
		return false;
	}

	string line;

	while( getline(ifs, line) )
	{
		if(line.size() == 0)
			continue;

        vector<string> words;
        ba::split(line, words, " ");

        if(words.size() == 0)
            continue;

        _liveWordDictMapUpd->insert(make_pair<string, bool>(words.at(0), true));

	}

    return true;
}


WordCutter::~WordCutter() 
{

    delete _userWordDictMapUse;
    delete _userWordDictMapUpd;

    delete _userNameDictMapUse;
    delete _userNameDictMapUpd;

	delete _singleWordDictMapUse;
	delete _singleWordDictMapUpd;

    delete _segmentWordUse;
    delete _segmentWordUpd;

    delete _segmentNameUse;
    delete _segmentNameUpd; 

	delete _segmentSingleWordUse;
	delete _segmentSingleWordUpd;

    delete _rwlock;

}

