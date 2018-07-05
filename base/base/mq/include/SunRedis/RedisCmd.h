#ifndef _Redis_CMD_H_
#define _Redis_CMD_H_

#include <string>
#include <vector>
#include <set>
#include <map>
#include <algorithm>
#include <hiredis/hiredis.h>

#include "RedisComm.h"
#include "RedisConn.h"
#include "RedisModule.h"
#include "RedisIndex.h"

typedef std::vector<Key>        Keys;
typedef Keys                    FILEDS;
typedef std::vector<Value>      Values;

typedef std::set<std::string>        SETDATA;

typedef struct _DATA_ITEM_{
    int         type;
    std::string str;

    _DATA_ITEM_ & operator=(const _DATA_ITEM_ &data) {
        type = data.type;
        str  = data.str;
        return *this;
    }
}DataItem;
typedef std::vector<DataItem>            ReplyData;
typedef ReplyData                        ArrayReply;
typedef std::map<std::string, double>    ZSETDATA;
//typedef std::vector<RedisIndex>          DBIArray;


typedef enum _BIT_OP_{
    AND = 0,
    OR  = 1,
    XOR = 2,
    NOT = 3
}BITOP;

typedef enum _LIST_MODEL_{
    BEFORE = 0,
    AFTER  = 1
}LMODEL;


typedef enum _SORT_ORDER_{
    ASC = 0,
    DESC = 1
}SORTODER;

typedef struct _SORT_LIMIT_
{
	int offset;
	int count;
}LIMIT;

namespace SunRedis
{
class SunRedisReply;
class RedisCmd : public RedisModule, public RedisIndex
{
public:
	RedisCmd();
	virtual ~RedisCmd();

	bool ConnectGroup(const std::vector<RedisNode>& arrayRedisNode);
protected:
    int command_bool(const char* cmd, ...);
    int command_status(const char* cmd, ...);
    int command_integer(int64_t &intval, const char* cmd, ...);
    int command_string(std::string &data, const char* cmd, ...);
    int command_list(Values &vValue, const char* cmd, ...);
    int command_array(ArrayReply& array, const char* cmd, ...);

protected:
    int commandargv_bool(const VDATA& vData);
    int commandargv_status(const VDATA& vData);
    int commandargv_array(const VDATA& vDataIn, ArrayReply& array);
    int commandargv_array(const VDATA& vDataIn, Values& array);
    int commandargv_integer(const VDATA& vDataIn, int64_t& retval);

protected:
    redisReply* command(const char *format, ...);
    SunRedisReply commandv(const char *format, va_list ap);
    SunRedisReply commandArgv(int argc, const char **argv, const size_t *argvlen);
};

}

#endif /* _Redis_CMD_H_ */
