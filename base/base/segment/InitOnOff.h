/*
 * CPPJIEBA 
 * modified by qinjian@sogou-inc.com
 */
#ifndef BA_INITONOFF_H
#define BA_INITONOFF_H

namespace ba
{
    class InitOnOff
    {
        public:
            InitOnOff(){
				_setInitFlag(false);
			};
            ~InitOnOff(){};
        protected:
            bool _getInitFlag()const{return _isInited;};
            bool _setInitFlag(bool flag){return _isInited = flag;};

            bool _isInited;
    };
}

#endif
