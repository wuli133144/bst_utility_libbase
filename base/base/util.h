#ifndef __UTIL_H__
#define __UTIL_H__

#define _CRT_SECURE_NO_DEPRECATE	// remove warning C4996, 

#include "ostype.h"
#include "UtilPdu.h"
#include "Lock.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include "slog/slog_api.h"
#ifndef _WIN32
#include <strings.h>
#endif

#include <sys/stat.h>
#include <assert.h>

#include <time.h>
#include <sys/types.h>

#include <unistd.h>
#include <ctype.h>

#ifdef _WIN32
#define	snprintf	sprintf_s
#else
#include <stdarg.h>
#include <pthread.h>
#include <time.h>
#include <sys/time.h>
#endif

#define NOTUSED_ARG(v) ((void)v)		// used this to remove warning C4100, unreferenced parameter

/// yunfan modify end 
class CRefObject
{
public:
	CRefObject();
	virtual ~CRefObject();

	void SetLock(CLock* lock) { m_lock = lock; }
	void AddRef();
	void ReleaseRef();
private:
	int				m_refCount;
	CLock*	m_lock;
};

//add by hfy
class LogLevel
{
public:
	LogLevel(){}
	~LogLevel();
	static LogLevel* getInstance();
	bool init(uint32_t level, string confPath);
	bool update(uint32_t level);
	uint32_t getLevel();
	string getPath();
	
private:
	uint32_t _level;
	string _confPath;
private:
	static LogLevel* logLevelPtr;
};

//日志级别
#define TraceLevel (1)
#define DebugLevel (2)
#define InfoLevel (3)
#define WarnLevel (4)
#define ErrorLevel (5)
#define FatalLevel (6)

#define LOG_MODULE_IM         "IM"


extern CSLog g_imlog;

// Add By ZhangYuanhao 2015-01-14 For log show the file name not the full path + filename
#define __FILENAME__ (strrchr(__FILE__, '/') ? (strrchr(__FILE__, '/') + 1):__FILE__)
#if defined(_WIN32) || defined(_WIN64)
#define logInfo(fmt, ...)  g_imlog.Info("<%s>\t<%d>\t<%s>,"fmt, __FILENAME__, __LINE__, __FUNCTION__, ##__VA_ARGS__)
#else

#define logInfo(fmt, args...)  g_imlog.Info("<%s>|<%d>|<%s>," fmt, __FILENAME__, __LINE__, __FUNCTION__, ##args)
/*
#define _TRACE(fmt, args...)  g_imlog.Trace("<%s>|<%d>|<%s>," fmt, __FILENAME__, __LINE__, __FUNCTION__, ##args)
#define _INFO(fmt, args...)  g_imlog.Info("<%s>|<%d>|<%s>," fmt, __FILENAME__, __LINE__, __FUNCTION__, ##args)
#define _DEBUG(fmt, args...)  g_imlog.Debug("<%s>|<%d>|<%s>," fmt, __FILENAME__, __LINE__, __FUNCTION__, ##args)
#define _WARN(fmt, args...)  g_imlog.Warn("<%s>|<%d>|<%s>," fmt, __FILENAME__, __LINE__, __FUNCTION__, ##args)
#define _ERROR(fmt, args...)  g_imlog.Error("<%s>|<%d>|<%s>," fmt, __FILENAME__, __LINE__, __FUNCTION__, ##args)
#define _FATAL(fmt, args...)  g_imlog.Fatal("<%s>|<%d>|<%s>," fmt, __FILENAME__, __LINE__, __FUNCTION__, ##args)
*/

#define TRACE(fmt, args...) \
	do{\
		if(LogLevel::getInstance()->getLevel()<=TraceLevel) \
			g_imlog.Trace("<%s>|<%d>|<%s>," fmt, __FILENAME__, __LINE__, __FUNCTION__, ##args);\
	}while(0)

#define DEBUG(fmt, args...) \
	do{\
		if(LogLevel::getInstance()->getLevel()<=DebugLevel) \
			g_imlog.Debug("<%s>|<%d>|<%s>," fmt, __FILENAME__, __LINE__, __FUNCTION__, ##args);\
	}while(0)
	
#define INFO(fmt, args...) \
	do{\
		if(LogLevel::getInstance()->getLevel()<=InfoLevel) \
			g_imlog.Info("<%s>|<%d>|<%s>," fmt, __FILENAME__, __LINE__, __FUNCTION__, ##args);\
	}while(0)

#define WARN(fmt, args...) \
	do{\
		if(LogLevel::getInstance()->getLevel()<=WarnLevel) \
			g_imlog.Warn("<%s>|<%d>|<%s>," fmt, __FILENAME__, __LINE__, __FUNCTION__, ##args);\
	}while(0)

#define ERROR(fmt, args...) \
	do{\
		if(LogLevel::getInstance()->getLevel()<=ErrorLevel) \
			g_imlog.Error("<%s>|<%d>|<%s>," fmt, __FILENAME__, __LINE__, __FUNCTION__, ##args);\
	}while(0)

#define FATAL(fmt, args...) \
	do{\
		if(LogLevel::getInstance()->getLevel()<=FatalLevel) \
			g_imlog.Fatal("<%s>|<%d>|<%s>," fmt, __FILENAME__, __LINE__, __FUNCTION__, ##args);\
	}while(0)


#endif
//#define logInfo(fmt, ...)  g_imlog.Info("<%s>\t<%d>\t<%s>,"+fmt, __FILENAME__, __LINE__, __FUNCTION__, ##__VA_ARGS__)

uint64_t get_tick_count();
void util_sleep(uint32_t millisecond);

time_t get_modify_time(const char* file_path);
//void * logLevelUpdate(void* ptr);



//added by SunWei
//string getLocalTime();

class CStrExplode
{
public:
	CStrExplode(char* str, char seperator);
	virtual ~CStrExplode();

	uint32_t GetItemCnt() { return m_item_cnt; }
	char* GetItem(uint32_t idx) { return m_item_list[idx]; }
private:
	uint32_t	m_item_cnt;
	char** 		m_item_list;
};

char* replaceStr(char* pSrc, char oldChar, char newChar);
string int2string(uint32_t user_id);
uint32_t string2int(const string& value);
void replace_mark(string& str, string& new_value, uint32_t& begin_pos);
void replace_mark(string& str, uint32_t new_value, uint32_t& begin_pos);

bool isnum(string &str);

void writePid();
inline unsigned char toHex(const unsigned char &x);
inline unsigned char fromHex(const unsigned char &x);
string URLEncode(const string &sIn);
string URLDecode(const string &sIn);


int64_t get_file_size(const char *path);
const char*  memfind(const char *src_str,size_t src_len, const char *sub_str, size_t sub_len, bool flag = true);

//add by liuyuejie
void getTimestampForMysql(char * ts, int len);
string getLocalTime();

#endif
