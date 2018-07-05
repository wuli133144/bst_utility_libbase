#ifndef _Redis_SLOT_H_
#define _Redis_SLOT_H_

#include <stdint.h>

#include "RedisComm.h"

#define SLOT_MAX (16384)

namespace SunRedis
{

/**
 * 该计算部分代码完全拷贝自redis3.2.1代码
 */
class RedisSlot {
public:
	RedisSlot();
	virtual ~RedisSlot();

	void Clear();
	void SetSlotGroup(const unsigned int uiSlot, const unsigned int uiGroup);
	unsigned int GetSlotGroup(const unsigned int uiSlot);

	/**
	 * CRC16校验
	 */
	static uint16_t crc16(const char *buf, const int len);

	/**
	 * AP Hash Function
	 */
	static unsigned int APHash(const char *buf, const int len);

	/**
	 * 计算key对应的slot
	 */
	static unsigned int keyHashSlot(const char* const key, const int keylen);
	//static unsigned int keyHashSlot(const std::string& key);

private:
	static const uint16_t crc16tab[];
	GroupID mSlot2Group[SLOT_MAX];
};

}

#endif /* _Redis_SLOT_H_ */
