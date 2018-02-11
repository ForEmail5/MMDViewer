#pragma once

#include <map>
#include <list>
#include <vector>

#define MAX_EVENT_PARAM 6
#define MASK(n) (1 << n)

struct BaseEvent
{
	int event_id;					/* 事件 ID */
	int params[MAX_EVENT_PARAM];	/* 自定义参数 */
	void* user_data;				/* 用户数据 */
};

//---------------------------------------------------------------------
// EventListener
// 事件监听器
//---------------------------------------------------------------------
class EventListener
{
	friend class EventDispatcher;

public:
	EventListener(unsigned int category_bit = 0x1 << 31);

	virtual ~EventListener() {}

	EventDispatcher* GetEventDispatcher();

	void SetCategoryBit(unsigned int category_bit);

	unsigned int CategoryBit() { return category_bit; }

protected:
	void AppendListener(int eventID, EventListener* listener);

	void RemoveListener(int eventID, EventListener* listener);

	virtual void HandleEvent(const BaseEvent& event) = 0;

private:
	static unsigned int nIDCounter;
	unsigned int id;

	/* 类别标志位 */
	unsigned int category_bit;
};


//---------------------------------------------------------------------
// EventDispatcher
// 事件分派器
//---------------------------------------------------------------------
class EventDispatcher
{
	friend class EventListener;
	typedef std::map<int, std::list<EventListener*>> ListenerGroup;

	struct RemoveListenerData
	{
		int ID;
		EventListener* listener;
	};
	typedef std::vector<RemoveListenerData> RemoveListenerList;

public:
	EventDispatcher();

	static EventDispatcher* GetInstance() { return &eventDispatcher; }

	void AppendMaskBit(unsigned int mask_bit);

	void RemoveMaskBit(unsigned int mask_bit);

	void SetMaskBits(unsigned int mask_bits);

	void ResetMaskBits();

	void DispatchEvent(const BaseEvent& event);

	void DispatchEventImmediate(const BaseEvent& event);

	void FlushEvent();

private:
	void RealDispatchEvent(const BaseEvent& event);

private:
	static EventDispatcher eventDispatcher;

	std::list<BaseEvent> event_pool;
	ListenerGroup listener_group;

	/* 遮罩标志位 */
	unsigned int mask_bits;
	unsigned int fixed_mask_bits;
};