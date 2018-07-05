/*
 * CPPJIEBA 
 * modified by qinjian@sogou-inc.com
 */
#ifndef BA_HMMSEGMENT_H
#define BA_HMMSEGMENT_H

#include <iostream>
#include <fstream>
#include <memory.h>
#include <cassert>

#include "ISegment.h"
#include "SegmentBase.h"
#include "DictTrie.h"
#include "segment/StringUtil.h"
#include "util.h"

namespace ba
{
    using namespace ba;

    typedef unordered_map<uint16_t, double> EmitProbMap;

    class HMMSegment: public SegmentBase
    {
        public:
            /*
             * STATUS:
             * 0:B, 1:E, 2:M, 3:S
             * */
            enum {B = 0, E = 1, M = 2, S = 3, STATUS_SUM = 4};

            HMMSegment(){_setInitFlag(false);}
            explicit HMMSegment(const string& filePath)
            {
                _setInitFlag(init(filePath));
            }
            virtual ~HMMSegment(){}

            bool init(const string& filePath);

			using SegmentBase::cut;
            bool cut(Unicode::const_iterator begin, Unicode::const_iterator end, vector<Unicode>& res);
            bool cut(Unicode::const_iterator begin, Unicode::const_iterator end, vector<string>& res);

        private:
            char _statMap[STATUS_SUM];
            double _startProb[STATUS_SUM];
            double _transProb[STATUS_SUM][STATUS_SUM];
            EmitProbMap _emitProbB;
            EmitProbMap _emitProbE;
            EmitProbMap _emitProbM;
            EmitProbMap _emitProbS;
            vector<EmitProbMap* > _emitProbVec;

            bool _loadModel(const char* const filePath);
            bool _getLine(ifstream& ifile, string& line);
            bool _loadEmitProb(const string& line, EmitProbMap& mp);

            bool _cut(Unicode::const_iterator begin, Unicode::const_iterator end, vector<Unicode>& res);
            bool _viterbi(Unicode::const_iterator begin, Unicode::const_iterator end, vector<size_t>& status)const;
            double _getEmitProb(const EmitProbMap* ptMp, uint16_t key, double defVal)const ;
    };
}

#endif
