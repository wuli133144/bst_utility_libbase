/*
 * CPPJIEBA 
 * modified by qinjian@sogou-inc.com
 */
#ifndef BA_MIXSEGMENT_H
#define BA_MIXSEGMENT_H

#include <cassert>

#include "MPSegment.h"
#include "HMMSegment.h"
#include "segment/StringUtil.h"
#include "util.h"

namespace ba
{
	using namespace ba;

    class MixSegment: public SegmentBase
    {
        public:
            MixSegment(){_setInitFlag(false);};
            MixSegment(const string& mpSegDict, const string& hmmSegDict, const string& userDict = "")
            {
                _setInitFlag(init(mpSegDict, hmmSegDict, userDict));
                assert(_getInitFlag());
            }
            virtual ~MixSegment(){}

            bool init(const string& mpSegDict, const string& hmmSegDict, const string& userDict = "");
            //update dict
            bool update(const string& mpSegDict, const string& hmmSegDict, const string& userDict = "");

			using SegmentBase::cut;
            bool cut(Unicode::const_iterator begin, Unicode::const_iterator end, vector<Unicode>& res);
            bool cut(Unicode::const_iterator begin, Unicode::const_iterator end, vector<string>& res);

        private:
            MPSegment _mpSeg;
            HMMSegment _hmmSeg;

    };
}

#endif
