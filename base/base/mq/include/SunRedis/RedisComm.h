#ifndef _Redis_COMM_H_
#define _Redis_COMM_H_

#include <vector>
#include <sstream>
#include <stdio.h>
#include <limits.h>

#define MAX_GROUPID (UINT_MAX-1)
#define ERROR_GROUPID (UINT_MAX)

typedef unsigned int GroupID;

typedef std::string             Key;
typedef std::string             Value;

typedef std::vector<std::string>     VDATA;

#define MAX_REDIS_CONNPOOL_SIZE     128      // 每个DB最大连接数

#define GET_CONNECT_ERROR       "get connection error"
#define CONNECT_CLOSED_ERROR    "redis connection be closed"

#ifdef WIN32
    #define   strcasecmp   stricmp
    #define   strncasecmp  strnicmp
#endif

enum Redis_Error_Code {
    EC_REDIS_SUCCEED = 0,
    EC_REDIS_CONNECT_FAIL = 1,
    EC_REDIS_ERROR = 2,
};
/**
 * 主备角色定义
 */
enum Role {
    MASTER = 0,
    SLAVE
};

/**
*
*/
enum Mode {
	CLUSTER = 0,
	SLICE   = 1,
};

/**
 * 内存回收
 */
#define DELETE(point)      { if(NULL!=point){delete point; point=NULL;} }
#define DELETEARRAY(point) { if(NULL!=point){delete[] point; point=NULL;} }

/**
 * 通用对象转字符串
 */
template<class Type>
std::string toString(const Type& val) {
    std::ostringstream oss;
    oss << val;
    return oss.str();
}

#define MY_ERROR_OUT std::cout << "ERROR, File:" << __FILE__ <<",Function:" << __FUNCTION__ <<", Line:"<< __LINE__
#define MY_INFO_OUT std::cout << "INFO, File:" << __FILE__ <<",Function:" << __FUNCTION__ <<", Line:"<< __LINE__


namespace SunRedis
{

class RedisComm {
public:
	RedisComm();
	virtual ~RedisComm();

    static void addParam(VDATA& vDes, const VDATA& vSrc);
    static bool formatCommand(std::string& target, const char *format, ...);
    static bool formatCommandv(std::string& target, const char *format, va_list ap);
	static bool str2Vect(const std::string& srcStr, std::vector<std::string>& vDest, const std::string& sepStr);
	static int getRandWithLeft(int min, int max);
};

}

#endif /* _Redis_COMM_H_ */
