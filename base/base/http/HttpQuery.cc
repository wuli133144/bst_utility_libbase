#include "HttpQuery.h"


int HttpQuery::refcount=0;

int HttpQuery::refcount0=1;

pthread_mutex_t HttpQuery::m_mutex=PTHREAD_MUTEX_INITIALIZER;

HttpQuery::HttpQuery(string &url):
m_handle(NULL),
m_url(url),
m_request(url),
m_response("")
{
	init_curl();
	m_handle=curl_easy_init();
}

HttpQuery::HttpQuery():
m_handle(NULL),
m_url(""),
m_request(""),
m_response("")
{
	init_curl();
	m_handle=curl_easy_init();
}




HttpQuery::~HttpQuery(){

	curl_easy_cleanup(m_handle);
	pthread_mutex_lock(&m_mutex);
	if(--refcount ==0){
		curl_global_cleanup();
	}
	pthread_mutex_unlock(&m_mutex);
}


void HttpQuery::init_curl()
{

	pthread_mutex_lock(&m_mutex);
	if(refcount0){
		 curl_global_init(CURL_GLOBAL_ALL);
		 refcount0--;
	}
	refcount++;
	pthread_mutex_unlock(&m_mutex);
  
}



int  HttpQuery::getref()
{
  return refcount;
}

bool HttpQuery::isNull()
{
   return refcount==0?true:false;
}


size_t writedatacallback(void *ptr, size_t size, size_t nmemb, void *stream) {
	string data((const char*) ptr, (size_t) size * nmemb);
	*((stringstream*)stream)<<data;
	//(*((string *)stream)).append(data);
	return size * nmemb;

}

size_t writeheadcallback(void *ptr, size_t size, size_t nmemb, void *stream) {
	string data((const char*) ptr, (size_t) size * nmemb);
	*((stringstream*) stream)<<data;
	return size * nmemb;

}

string  HttpQuery::get(string & url)
{

	m_url.str(url);
	m_request.str(url);
	curl_easy_setopt(m_handle, CURLOPT_URL,url.c_str());
	//set timeout 
	curl_easy_setopt(m_handle, CURLOPT_TIMEOUT, 20L);
	curl_easy_setopt(m_handle, CURLOPT_NOSIGNAL, 1L);
	curl_easy_setopt(m_handle, CURLOPT_CUSTOMREQUEST, "GET");
	//set header data
	
	curl_easy_setopt(m_handle,CURLOPT_READFUNCTION ,writeheadcallback);
	curl_easy_setopt(m_handle,CURLOPT_READDATA,(void *)&m_headerdata);
	
	
	//set response data
	curl_easy_setopt(m_handle, CURLOPT_WRITEFUNCTION,writedatacallback);
	curl_easy_setopt(m_handle, CURLOPT_WRITEDATA, (void *)&m_response);
	int ret=curl_easy_perform(m_handle);


	
	if(ret !=CURLE_OK){
		curlerror(ret);
	}
	
	return m_response.str();
}





void  HttpQuery::curlerror(int ret)
{
	switch(ret){

	case CURLE_OK:
		m_error.str("http request success");
		break;
	case CURLE_UNSUPPORTED_PROTOCOL:
		m_error.str("http request unsupported protocol");
		break;
	case CURLE_COULDNT_CONNECT:
		m_error.str("http request unable to connect remote server");
		break;
    case CURLE_REMOTE_ACCESS_DENIED:
		m_error.str("http request access denied");
		break;
	case CURLE_HTTP_RETURNED_ERROR:
		m_error.str("http request return error");
		break;
	case CURLE_READ_ERROR:
	    m_error.str("http request read error");
		break;
	default:
	    break;

	}
		 
}


void HttpQuery::getcurlversion()
{
      m_version.str(curl_version());
}

char   * HttpQuery::get2(const char *url)
{
     if(url == NULL){
		return NULL;
	}

	curl_easy_setopt(m_handle, CURLOPT_TIMEOUT, 20L);
	curl_easy_setopt(m_handle, CURLOPT_NOSIGNAL, 1L);
	curl_easy_setopt(m_handle, CURLOPT_URL,url); 
	
	curl_easy_setopt(m_handle, CURLOPT_CUSTOMREQUEST, "GET");
	curl_easy_setopt(m_handle ,CURLOPT_WRITEFUNCTION,writedatacallback);
	curl_easy_setopt(m_handle, CURLOPT_WRITEDATA, (void *)&m_response);

	curl_easy_perform(m_handle);

	
	return ( char *)m_response.str().c_str();
}


void HttpQuery::post(string & request, string & response)
{
   
   	if(request.empty()){
		return;
	}
	stringstream data;
	int pos=request.find("?");
	if(pos == std::string::npos){
		response="";
		return;
	}
	
	string keyvalues=request.substr(pos+1);
	string request_url=request.substr(0,pos);

	
	curl_easy_setopt(m_handle, CURLOPT_POST, 1);
	curl_easy_setopt(m_handle, CURLOPT_POSTFIELDS, keyvalues.c_str());    

	curl_easy_setopt(m_handle, CURLOPT_URL,request_url.c_str());  
	curl_easy_setopt(m_handle,CURLOPT_WRITEFUNCTION,writedatacallback);
	curl_easy_setopt(m_handle, CURLOPT_WRITEDATA, (void *)&data);

	
	int ret=curl_easy_perform(m_handle);
	response=data.str();
    

}


void HttpQuery::post2(const char *request, char *response)
{
	    if(request == NULL){
			return;
		}
		string request_str(request);
		string response_str;
        post(request_str,response_str);

		if(response && (strlen(response) >=strlen(response_str.c_str())))
		   strncpy(response,response_str.c_str(),strlen(response_str.c_str()));
		
}

