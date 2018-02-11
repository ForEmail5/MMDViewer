#include "Event.h"

#define MASK_ALL 0xFFFFFFFF

EventDispatcher* pDispatcher = nullptr;

//---------------------------------------------------------------------
// EventListener
// �¼�������
//---------------------------------------------------------------------
unsigned int EventListener::nIDCounter = 0;

EventListener::EventListener(unsigned int category_bit)
{
	id = nIDCounter++;
	this->category_bit = category_bit;
}

EventDispatcher* EventListener::GetEventDispatcher()
{
	return pDispatcher;
}

void EventListener::SetCategoryBit(unsigned int category_bit)
{
	this->category_bit = category_bit;
}

void EventListener::AppendListener(int eventID, EventListener* new_listener)
{
	auto listenerList = pDispatcher->listener_group.find(eventID);

	/* �¼� ID û�м����б�Ϊ ID ���������б���� eListener */
	if ( listenerList == pDispatcher->listener_group.end() ) 
	{
		std::list<EventListener*> newListenerList;
		newListenerList.push_back(new_listener);
		pDispatcher->listener_group.insert(std::make_pair(eventID, newListenerList));
	}
	else 
	{
		/* ��������б���û�м���������Ӽ��������б��� */
		std::list<EventListener*>::iterator listener_it;
		for ( listener_it = listenerList->second.begin(); listener_it != listenerList->second.end(); ++listener_it ) 
		{
			if ( (*listener_it)->id == new_listener->id ) return;
		}
		if ( listener_it == listenerList->second.end() ) 
		{
			listenerList->second.push_back(new_listener);
		}
	}
}

void EventListener::RemoveListener(int eventID, EventListener* listener)
{
	auto listenerList = pDispatcher->listener_group.find(eventID);
	if ( listenerList == pDispatcher->listener_group.end() ) return;

	/* �Ӽ����б����Ƴ������� */
	for ( auto it = listenerList->second.begin(); it != listenerList->second.end(); ++it ) 
	{
		if ( (*it)->id == listener->id )
		{
			listenerList->second.erase(it);
			break;
		}
	}
	/* �Ƴ��ռ����б� */
	if ( listenerList->second.empty() )
	{
		pDispatcher->listener_group.erase(listenerList);
	}
}


//---------------------------------------------------------------------
// EventDispatcher
// �¼�������
//---------------------------------------------------------------------
EventDispatcher EventDispatcher::eventDispatcher;

EventDispatcher::EventDispatcher()
{
	pDispatcher = this;
	mask_bits = fixed_mask_bits = MASK_ALL;
}

void EventDispatcher::AppendMaskBit(unsigned int mask_bit)
{
	if ( mask_bits == fixed_mask_bits ) 
	{
		mask_bits = 0;
	}
	mask_bits |= mask_bit;
}

void EventDispatcher::RemoveMaskBit(unsigned int mask_bit)
{
	mask_bits &= (~mask_bit);
	if ( mask_bits == 0 ) 
	{
		mask_bits = fixed_mask_bits;
	}
}

void EventDispatcher::SetMaskBits(unsigned int mask_bits)
{
	fixed_mask_bits = mask_bits;
	mask_bits = mask_bits;
}

void EventDispatcher::ResetMaskBits()
{
	fixed_mask_bits = MASK_ALL;
	mask_bits = MASK_ALL;
}

void EventDispatcher::DispatchEvent(const BaseEvent& event)
{
	/* ֻ����ʱ����¼����¼����У���û�����������¼�������ݹ���ɴ��� */
	event_pool.push_back(event);
}

void EventDispatcher::DispatchEventImmediate(const BaseEvent& event)
{
	this->RealDispatchEvent(event);
}

void EventDispatcher::FlushEvent()
{
	if ( event_pool.empty() ) return;

	/* �����¼����е������¼� */
	for ( auto& event : event_pool )
	{
		this->RealDispatchEvent(event);
	}
	event_pool.resize(0);
}

void EventDispatcher::RealDispatchEvent(const BaseEvent& event)
{
	auto listenerList_it = listener_group.find(event.event_id);
	if ( listenerList_it == listener_group.end() || listenerList_it->second.empty() ) return;

	std::list<EventListener*> listenerList = listenerList_it->second;
	for ( auto listener_it : listenerList ) 
	{
		if ( listener_it->CategoryBit() & mask_bits )
		{
			listener_it->HandleEvent(event);
		}
	}
}