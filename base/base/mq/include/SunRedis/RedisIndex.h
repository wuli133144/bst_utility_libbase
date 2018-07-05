#ifndef _Redis_INDEX_H_
#define _Redis_INDEX_H_

#include <string>
#include <stdint.h>
#include <hiredis/hiredis.h>

#include "RedisSlot.h"
#include "RedisComm.h"

namespace SunRedis
{

class RedisIndex {
public:
	RedisIndex();
	RedisIndex(const std::string key);
	virtual ~RedisIndex();

    char* GetErrInfo() {return mStrerr;}
    void SetIOMaster();

protected:
    bool GetFlag() const;
    void SetFlag(const bool flag);
	void SetKey(const std::string key);
    void SetErrInfo(const redisReply* const reply);
    bool SetErrString(const char *info, int len);
    void SetErrMessage(const char* fmt, ...);
    // 当mFlag为false才能够设置成功
    bool SetRole(const Role role, const bool flag=false);
	void setMode(Mode mode);
	Mode mode();

protected:
    std::string  mKey;
    char         mStrerr[5120];
    Role         mRole;
    bool         mFlag;
	Mode         mMode;
};

}

#endif /* _Redis_INDEX_H_ */
