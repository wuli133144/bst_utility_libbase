

#ifndef    ____HTTPQUERY__H
#define     ____HTTPQUERY__H


#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <stddef.h>
#include <curl/curl.h>
#include <string>
#include <string.h>
#include <iostream>
#include <sstream>


/*
Copyright <2018-7-27> <COPYRIGHT JACKWUwuyujie@sunlands.com>
Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated 
documentation files (the "Software"), to deal in the Software without restriction, including without limitation
the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software,
and to permit persons to whom the Software is furnished to do so, subject to the following conditions:The above
copyright notice and this permission notice shall be included in all copies or substantial portions of the 
Software.THE SOFTWARE IS PROVIDED "AS IS" , WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT
NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT
SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF
CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
IN THE SOFTWARE.
*/

using namespace std;
class HttpQuery
{



public:
	
	HttpQuery(string &url);
	HttpQuery();

	~HttpQuery();

	
	string       get(string & url);
	char   *     get2(const char *);
	void         post(string&request,string &response);
	void         post2(const char *request, char *);

private:
	
	stringstream  m_headerdata;
	stringstream  m_response;
	stringstream  m_request;
	stringstream  m_version;
	stringstream  m_url;
	stringstream  m_cookies;

	stringstream  m_error;

	enum {BUFFERSIZE= 1024};

private:

	CURL *        m_handle;
	static void   init_curl();
	inline void   curlerror(int ret);
	inline void   getcurlversion();



public:

	static void getref();
	static bool isNull();

	static  int refcount;
	static  int refcount0;
	static  pthread_mutex_t m_mutex;
};







#endif 
