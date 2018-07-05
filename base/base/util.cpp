
#include "util.h"
#include "stdio.h"
#include "time.h"
#include <sstream>
#include "ConfigFileReader.h"
using namespace std;

CSLog g_imlog = CSLog(LOG_MODULE_IM);

CRefObject::CRefObject()
{
	m_lock = NULL;
	m_refCount = 1;
}

CRefObject::~CRefObject()
{

}

void CRefObject::AddRef()
{
	if (m_lock)
	{
		m_lock->lock();
		m_refCount++;
		m_lock->unlock();
	}
	else
	{
		m_refCount++;
	}
}

void CRefObject::ReleaseRef()
{
	if (m_lock)
	{
		m_lock->lock();
		m_refCount--;
		if (m_refCount == 0)
		{
			delete this;
			return;
		}
		m_lock->unlock();
	}
	else
	{
		m_refCount--;
		if (m_refCount == 0)
			delete this;
	}
}

uint64_t get_tick_count()
{
#ifdef _WIN32
	LARGE_INTEGER liCounter; 
	LARGE_INTEGER liCurrent;

	if (!QueryPerformanceFrequency(&liCounter))
		return GetTickCount();

	QueryPerformanceCounter(&liCurrent);
	return (uint64_t)(liCurrent.QuadPart * 1000 / liCounter.QuadPart);
#else
	struct timeval tval;
	uint64_t ret_tick;

	gettimeofday(&tval, NULL);

	ret_tick = tval.tv_sec * 1000L + tval.tv_usec / 1000L;
	return ret_tick;
#endif
}

void util_sleep(uint32_t millisecond)
{
#ifdef _WIN32
	Sleep(millisecond);
#else
	usleep(millisecond * 1000);
#endif
}

time_t get_modify_time(const char* file_path)
{
	FILE* fp; 
	int fd;
	struct stat buf;
    fp=fopen(file_path,"r");
    fd=fileno(fp);
    fstat(fd, &buf);
    time_t modify_time=buf.st_mtime;
    fclose(fp);
	return modify_time;
}

/*
void * logLevelUpdate(void* ptr)
{

	string confPath = LogLevel::getInstance()->getPath();
	time_t conf_last_modify = get_modify_time(confPath.c_str());
	
	uint32_t load_time = 900;
	CConfigFileReader config_file(confPath.c_str());
	char* update_time_cs = config_file.GetConfigName("levelUpdateTime");

	if (!update_time_cs ) {
		ERROR("missing update_time_cs, exit...");
		return NULL;
	}

	load_time = atoi(update_time_cs);

	
	
	while(true)
	{
		sleep(load_time);
		time_t conf_cur_modify = get_modify_time(confPath.c_str());
		if(difftime(conf_cur_modify, conf_last_modify))
		{
			conf_last_modify = conf_cur_modify;
			CConfigFileReader config_file(confPath.c_str());
			char* confLevel = config_file.GetConfigName("LogLevel");
			if (!confLevel) {
				ERROR("missing confLevel, exit...");
				return NULL;
			}
			uint32_t new_level = atoi(confLevel);
			LogLevel::getInstance()->update(new_level);
			INFO("The conf_file has been changed, the new log level is: %d\n!!!", new_level);
		}
	}
}*/


/*
string getLocalTime()
{
    time_t rawtime;
    time ( &rawtime );
    struct tm * ptm;
    ptm = localtime(&rawtime);
    string str;
    str=str
        .append(itos(ptm->tm_year + 1900))
        .append("-")
        .append(itos(ptm->tm_mon + 1)) 
        .append("-")
        .append(itos(ptm->tm_mday))
        .append(" ")
        .append(itos(ptm->tm_hour))
        .append(":")
        .append(itos(ptm->tm_min))
        .append(":")
        .append(itos(ptm->tm_sec));

    return str;
}*/


CStrExplode::CStrExplode(char* str, char seperator)
{
    m_item_cnt = 1;
    char* pos = str;
    while (*pos) {
        if (*pos == seperator) {
            m_item_cnt++;
        }

        pos++;
    }

    m_item_list = new char* [m_item_cnt];

    int idx = 0;
    char* start = pos = str;
    while (*pos) {
        if ( pos != start && *pos == seperator) {
            uint32_t len = pos - start;
            m_item_list[idx] = new char [len + 1];
            strncpy(m_item_list[idx], start, len);
            m_item_list[idx][len]  = '\0';
            idx++;

            start = pos + 1;
        }

        pos++;
    }

    uint32_t len = pos - start;
    if(len != 0)
    {
        m_item_list[idx] = new char [len + 1];
        strncpy(m_item_list[idx], start, len);
        m_item_list[idx][len]  = '\0';
    }
}

CStrExplode::~CStrExplode()
{
    for (uint32_t i = 0; i < m_item_cnt; i++) {
        delete [] m_item_list[i];
    }

    delete [] m_item_list;
}

char* replaceStr(char* pSrc, char oldChar, char newChar)
{
    if(NULL == pSrc)
    {
        return NULL;
    }

    char *pHead = pSrc;
    while (*pHead != '\0') {
        if(*pHead == oldChar)
        {
            *pHead = newChar;
        }
        ++pHead;
    }
    return pSrc;
}

string int2string(uint32_t user_id)
{
    stringstream ss;
    ss << user_id;
    return ss.str();
}

uint32_t string2int(const string& value)
{
    return (uint32_t)atoi(value.c_str());
}

//! judge str is number
bool isnum(string &str)
{
       uint32_t sz=str.size();
	   const char *pbuf=str.c_str();
	   if(sz<1){return false;}
	   for(int i=0;i<sz;i++)
	   { 
	        if(!isdigit(pbuf[i]))
	        {
	           return false;
	        }
	   }
	   return true;

}


// 由于被替换的内容可能包含?号，所以需要更新开始搜寻的位置信息来避免替换刚刚插入的?号
void replace_mark(string& str, string& new_value, uint32_t& begin_pos)
{
    string::size_type pos = str.find('?', begin_pos);
    if (pos == string::npos) {
        return;
    }

    string prime_new_value = "'"+ new_value + "'";
    str.replace(pos, 1, prime_new_value);

    begin_pos = pos + prime_new_value.size();
}

void replace_mark(string& str, uint32_t new_value, uint32_t& begin_pos)
{
    stringstream ss;
    ss << new_value;

    string str_value = ss.str();
    string::size_type pos = str.find('?', begin_pos);
    if (pos == string::npos) {
        return;
    }

    str.replace(pos, 1, str_value);
    begin_pos = pos + str_value.size();
}


void writePid()
{
    uint32_t curPid;
#ifdef _WIN32
    curPid = (uint32_t) GetCurrentProcess();
#else
    curPid = (uint32_t) getpid();
#endif
    FILE* f = fopen("server.pid", "w");
    assert(f);
    char szPid[32];
    snprintf(szPid, sizeof(szPid), "%d", curPid);
    fwrite(szPid, strlen(szPid), 1, f);
    fclose(f);
}

inline unsigned char toHex(const unsigned char &x)
{
    return x > 9 ? x -10 + 'A': x + '0';
}

inline unsigned char fromHex(const unsigned char &x)
{
    return isdigit(x) ? x-'0' : x-'A'+10;
}

string URLEncode(const string &sIn)
{
    string sOut;
    for( size_t ix = 0; ix < sIn.size(); ix++ )
    {
        unsigned char buf[4];
        memset( buf, 0, 4 );
        if( isalnum( (unsigned char)sIn[ix] ) || (0 == sIn[ix]) || (10 == sIn[ix]) || (13 == sIn[ix]) || (34 == sIn[ix]) || (92 == sIn[ix]) )
        {
            buf[0] = sIn[ix];
        }
        //else if ( isspace( (unsigned char)sIn[ix] ) ) //貌似把空格编码成%20或者+都可以
        //{
        //    buf[0] = '+';
        //}
        else
        {
            buf[0] = '%';
            buf[1] = toHex( (unsigned char)sIn[ix] >> 4 );
            buf[2] = toHex( (unsigned char)sIn[ix] % 16);
        }
        sOut += (char *)buf;
    }
    return sOut;
}

string URLDecode(const string &sIn)
{
    string sOut;
    for( size_t ix = 0; ix < sIn.size(); ix++ )
    {
        unsigned char ch = 0;
        if(sIn[ix]=='%')
        {
            ch = (fromHex(sIn[ix+1])<<4);
            ch |= fromHex(sIn[ix+2]);
            ix += 2;
        }
        else if(sIn[ix] == '+')
        {
            ch = ' ';
        }
        else
        {
            ch = sIn[ix];
        }
        sOut += (char)ch;
    }
    return sOut;
}


int64_t get_file_size(const char *path)
{
    int64_t filesize = -1;
    struct stat statbuff;
    if(stat(path, &statbuff) < 0){
        return filesize;
    }else{
        filesize = statbuff.st_size;
    }
    return filesize;
}

const char*  memfind(const char *src_str,size_t src_len, const char *sub_str, size_t sub_len, bool flag)
{
    if(NULL == src_str || NULL == sub_str || src_len <= 0)
    {
        return NULL;
    }
    if(src_len < sub_len)
    {
        return NULL;
    }
    const char *p;
    if (sub_len == 0)
        sub_len = strlen(sub_str);
    if(src_len == sub_len)
    {
        if(0 == (memcmp(src_str, sub_str, src_len)))
        {
            return src_str;
        }
        else
        {
            return NULL;
        }
    }
    if(flag)
    {
        for (int i = 0; i < src_len - sub_len; i++)
        {
            p = src_str + i;
            if(0 == memcmp(p, sub_str, sub_len))
                return p;
        }
    }
    else
    {
        for ( int i = (src_len - sub_len) ; i >= 0;i--  )
        {
            p = src_str + i;
            if ( 0 == memcmp(  p,sub_str,sub_len ) )
                return p;

        }
    }
    return NULL;
}

void getTimestampForMysql(char * ts, int len)
{
    struct tm *newtime; 
    time_t lt1; 
    time(&lt1);  
    newtime = localtime(&lt1); 
    strftime(ts, len-1, "%Y-%m-%d %H:%M:%S\n", newtime); 

    return;    
}

string itos(int a)
{
    stringstream ss;
    ss << a;
    return ss.str();
}

string getLocalTime()
{
    time_t rawtime;
    time ( &rawtime );
    struct tm * ptm;
    ptm = localtime(&rawtime);
    string str;
    str=str
      .append(itos(ptm->tm_year + 1900))
      .append("-")
      .append(itos(ptm->tm_mon + 1))
      .append("-")
      .append(itos(ptm->tm_mday))
      .append(" ")
      .append(itos(ptm->tm_hour))
      .append(":")
      .append(itos(ptm->tm_min))
      .append(":")
      .append(itos(ptm->tm_sec));

    return str;
}


//add by hfy
LogLevel* LogLevel::logLevelPtr = NULL;
LogLevel* LogLevel::getInstance()
{
	if(!logLevelPtr)
	{
		logLevelPtr = new LogLevel();
	}
	return logLevelPtr;
}

bool LogLevel::init(uint32_t level, string confPath)
{
	if(level<TraceLevel)
	{
		_level = 0;
		return false;
	}
	_level = level;
	_confPath = confPath;
	return true;
}

bool LogLevel::update(uint32_t level)
{
	if(level<TraceLevel)
	{
	    _level = 0;
		return false;
	}
	_level = level;
	return true;
}

LogLevel::~LogLevel()
{
}

uint32_t LogLevel::getLevel()
{
	return _level;
}

string LogLevel::getPath()
{
	return _confPath;
}
