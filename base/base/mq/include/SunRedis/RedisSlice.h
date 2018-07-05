
#ifndef _Redis_CACHE_H_
#define _Redis_CACHE_H_

#include <map>
#include <vector>

#include "RedisPool.h"

namespace SunRedis
{

class RedisSlice {
public:
	RedisSlice();
	virtual ~RedisSlice();

	bool init(const std::vector<RedisNode>&);

    RedisConn* conn(Mode mode, Role role);
	RedisNode getMasterNode();

private:
	RedisNode mMasterNode;
	RedisNodeVec mNodes;
	RedisPool* mMasterPool;
	std::vector<RedisPool*> mSlavePools;
};

}

#endif /* _Redis_CACHE_H_ */
