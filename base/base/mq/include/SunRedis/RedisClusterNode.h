#ifndef _REDIS_CLUSTER_NODE_H_
#define _REDIS_CLUSTER_NODE_H_
#include <string>
#include <vector>

namespace SunRedis {
class RedisClusterNode {
public:
	RedisClusterNode();
	bool decodeVal(const std::string &strVal);
	std::string toStr();
	
private:
	bool hostAndPort(const std::string &nodeString);
	std::pair<int, int> slots(const std::string& sSlot);
	
public:
	std::string mId;// The node ID, a 40 characters random string generated when a node is created and never changed again (unless CLUSTER RESET HARD is used)
	std::string mHost;
	int mPort;
	bool mIsMaster;
	std::string mFlag; // A list of comma separated flags: myself, master, slave, fail?, fail, handshake, noaddr, noflags
	std::vector<std::pair<int, int>> mSlots;
	std::string mMasterId;  // The replication master
};

}
#endif