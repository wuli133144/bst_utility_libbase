#ifndef _Redis_POOL_H_
#define _Redis_POOL_H_

#include <hiredis/hiredis.h>
#include <string.h>
#include <string>
#include <list>
#include <map>

#include "RedisSlice.h"
#include "RedisLock.h"
#include "RedisNode.h"
#include "RedisSlot.h"
#include "RedisComm.h"
#include "RedisCluster.h"
#include "RedisIndex.h"

namespace SunRedis
{

class RedisModule : protected RedisSlot {
public:
    RedisModule();
    virtual ~RedisModule();

	bool init(Mode mode, const std::vector<RedisNode>& arrayRedisNode);
    RedisConn* conn(const Key& key, const Role role=MASTER);

	RedisConn* clusterConn(const std::string&     key, Role role=MASTER);

private:
	bool initCluster(const std::vector<RedisNode>& arrayRedisNode);
	bool initSlice(const std::vector<RedisNode>& arrayRedisNode);

private:
	Mode mMode;
	RedisCluster mCluster;
	std::map<std::string, RedisSlice*> mIdSlice;
};

}

#endif
