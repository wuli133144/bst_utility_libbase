/*
 * CPPJIEBA 
 * modified by qinjian@sogou-inc.com
 */
#ifndef BA_SEGMENTBASE_H
#define BA_SEGMENTBASE_H

#include <cassert>

#include "ISegment.h"
#include "TransCode.h"
#include "segment/InitOnOff.h"
#include "segment/NonCopyable.h"

namespace ba
{
    using namespace ba;

    //const char* const SPECIAL_CHARS = " \t\n";
    const UnicodeValueType SPECIAL_SYMBOL[] = {32u, 9u, 10u};  

	/*  every class inherit from SegmentBase must implement as follow:
		1. rewrite "virtual bool cut(Unicode::const_iterator begin, Unicode::const_iterator end, vector<string>& res) const = 0"
		2. _setInitFlag(true)
	*/
	class SegmentBase: public ISegment, public InitOnOff //, public NonCopyable
	{
        public:
            SegmentBase(){_loadSpecialSymbols();};
            virtual ~SegmentBase(){};

            virtual bool cut(Unicode::const_iterator begin, Unicode::const_iterator end, vector<string>& res) = 0;
            bool cut(const string& str, vector<string>& res)
            {
                assert(_getInitFlag());
                res.clear();

                Unicode unicode;
                unicode.reserve(str.size());

                TransCode::decode(str, unicode);
                
                Unicode::const_iterator left = unicode.begin();
                Unicode::const_iterator right;
                
                for(right = unicode.begin(); right != unicode.end(); right++)
                {
                    if(isIn(_specialSymbols, *right))
                    {
                        if(left != right)
                        {
                            cut(left, right, res);
                        }
                        res.resize(res.size() + 1);
                        TransCode::encode(right, right + 1, res.back());
                        left = right + 1;
                    }
                }
                if(left != right)
                {
                    cut(left, right, res);
                }
                
                return true;
            }

        private:
            unordered_set<UnicodeValueType> _specialSymbols;

            void _loadSpecialSymbols()
            {
                size_t size = sizeof(SPECIAL_SYMBOL)/sizeof(*SPECIAL_SYMBOL);
                for(size_t i = 0; i < size; i++)
                {
                    _specialSymbols.insert(SPECIAL_SYMBOL[i]);
                }
                assert(_specialSymbols.size());
            }
    };
}

#endif
