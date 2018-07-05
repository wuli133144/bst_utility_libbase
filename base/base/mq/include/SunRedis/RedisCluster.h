#ifndef _REDIS_CLUSTER_H_
#define _REDIS_CLUSTER_H_
#include <string>
#include <vector>
#include "RedisConn.h"
#include "RedisNode.h"
#include <map>

namespace SunRedis {
class RedisCluster {
public:
	typedef std::map<std::string, RedisNodeVec> SliceMap;//主RedisID-->主从列表
	typedef std::pair<std::string, RedisNodeVec> SliceNode;//主RedisID-->主从列表
public:
	bool init(const RedisNode& redisNode);
	bool initSlice(const RedisNodeVec& nodes);
	std::string getSliceId(int slot);
	SliceMap getSlicesNodes();


private:
	bool clusterEnabled(RedisConn* conn);
	bool clusterStat(RedisConn* conn);
	bool slot2Slice();

	private:
		RedisNodeVec mMasterNodes; //保存主RedisNode
		SliceMap mSlicesNodes; 
};
}
#endif