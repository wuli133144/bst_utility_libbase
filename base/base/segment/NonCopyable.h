/*
 * CPPJIEBA 
 * modified by qinjian@sogou-inc.com
 */
#ifndef BA_NONCOPYABLE_H
#define BA_NONCOPYABLE_H

#include <iostream>
#include <string>

namespace ba
{
    class NonCopyable
    {
        protected:
            NonCopyable(){};
            ~NonCopyable(){};
        private:
            NonCopyable(const NonCopyable& );
            const NonCopyable& operator=(const NonCopyable& );
    };
}

#endif
