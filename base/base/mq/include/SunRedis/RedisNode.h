#ifndef _Redis_NODE_H_
#define _Redis_NODE_H_

#include <string>
#include <vector>
#include <list>
#include "RedisComm.h"

namespace SunRedis
{

class Node
{
public:
	Node();
	Node(const std::string& host, const unsigned int port);
	virtual ~Node();

	bool empty() const;
	void clear();

	bool operator==(const Node& node) const;
	bool operator<(const Node& node) const;
	bool operator>(const Node& node) const;

public:
    std::string  mHost;            // redis host
    unsigned int mPort;            // redis sever port
};

class RedisNode {
public:
	RedisNode();
	RedisNode(
			const Node         node,
			const std::string  passwd,
			const Role         role=SLAVE,
			const unsigned int gid=0);
	virtual ~RedisNode();

	operator const Node&();

	bool operator==(const RedisNode& redisNode) const;

	bool empty() const;
	void clear();

	void Print() const;

	bool decodeVal(const std::string &strVal);

	bool contain(int slot);
	
private:
	bool hostAndPort(const std::string &nodeString);
	std::pair<int, int> slots(const std::string& sSlot);

public:
	int 		 mGid;
	Node         mNode;
    std::string  mPasswd; // redis server password
    Role         mRole; // redis role
    std::string  mId; //自己id
	std::string  mMasterId; //如果是从，则为所属主id
	
	std::vector<std::pair<int, int>> mSlots;
};

typedef std::vector<RedisNode> RedisNodeVec;
typedef std::list<RedisNode> RedisNodeList;
}

#endif /* _Redis_NODE_H_ */
