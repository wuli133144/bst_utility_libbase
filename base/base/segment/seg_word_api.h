/*
 * api of cutting word
 * written by qinjian@sogou-inc.com
 */
#ifndef _BASE_SEG_WORD_API_H_
#define _BASE_SEG_WORD_API_H_

#include <vector>
#include <fstream>
#include <string>
#include <cassert>

#include "MixSegment.h"
#include "util.h"
#include "Lock.h"

using namespace std;
using namespace ba;


namespace ba {

	#define GROUP_WORD_SENSITIVE 1
	#define GROUP_NAME_SENSITIVE 2
	#define SINGLE_WORD_SENSITIVE 3
	#define LIVE_WORD_SENSITIVE 4
    typedef map<string, bool> UserDictMap;
    class WordCutter
    {
        public:
            WordCutter(){}
            ~WordCutter();

            static WordCutter* getInstance();
            //bool init(const string& dictPath, const string& mpDictPath, const string& hmmModelPath, const string& userWordDictPath, const string& userNameDictPath);
			bool init(const string& dictPath, const string& mpDictPath, const string& hmmModelPath, const string& userWordDictPath, const string& userNameDictPath, const string& singleWordDictPath, const string& liveWordDictPath);
            bool update(uint32_t sensiType);
            unsigned int cutWord(const char* inputs, vector<string>& word_vec);
            bool hasUserWord(const string& inputStr);
            bool hasUserName(const string& inputStr);
			bool hasSingleWord(const string& inputStr);
			bool hasLiveWord(const string& inputStr);

        private:

            bool _updateUserWordDict();
            bool _updateUserNameDict();
			bool _updateSingleWordDict();
            bool _updateLiveWordDict();

            string _mpDictPath;
            string _hmmModelPath;
            string _userWordDictPath;
            string _userNameDictPath;

			string _singleWordDictPath;
			string _liveWordDictPath;

            UserDictMap* _userWordDictMapUse;
            UserDictMap* _userWordDictMapUpd;

            UserDictMap* _userNameDictMapUse;
            UserDictMap* _userNameDictMapUpd;

			UserDictMap* _singleWordDictMapUse;
			UserDictMap* _singleWordDictMapUpd;

			UserDictMap* _liveWordDictMapUse;
			UserDictMap* _liveWordDictMapUpd;
            
            MixSegment* _segmentWordUse;
            MixSegment* _segmentWordUpd;

            MixSegment* _segmentNameUse;
            MixSegment* _segmentNameUpd;

			MixSegment* _segmentSingleWordUse;
			MixSegment* _segmentSingleWordUpd;

			MixSegment* _segmentLiveWordUse;
			MixSegment* _segmentLiveWordUpd;


            CRWLock* _rwlock;

        private:
            static WordCutter* wordCutterInsPtr;

    };
}

#endif

