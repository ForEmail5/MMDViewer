#pragma once
#include "EventDispatcher.h"

#include <map>
#include <list>

#include <Windows.h>


enum EventType 
{ 
	ET_UNKNOWN,			/* 未知事件 */
	ET_MOUSE,			/* 鼠标事件 */
	ET_KEY				/* 按键事件 */
};

enum ButtonType 
{ 
	LEFT_BUTTON,		/* 鼠标左键 */
	RIGHT_BUTTON,		/* 鼠标右键 */
	MIDDLE_BUTTON		/* 鼠标中键 */
};

enum EventAction
{
	ACT_MOVE,			/* 移动 */
	ACT_PRESS,			/* 按压 */
	ACT_RELAESE,		/* 释放 */
	ACT_DUBBLE_CLICK,	/* 双击 */
	ACT_SCROLL			/* 滚动 */
};


/*
* VK_0 - VK_9 are the same as ASCII '0' - '9' (0x30 - 0x39)
* 0x40 : unassigned
* VK_A - VK_Z are the same as ASCII 'A' - 'Z' (0x41 - 0x5A)
*/
enum KeyType
{
	Key_Unknown,

	Key_Space = 0x20,
	Key_Prior,
	Key_Next,
	Key_End,
	Key_Home,
	Key_Left,
	Key_Up,
	Key_Right,
	Key_Down,
	Key_Select,
	Key_Print,
	Key_Execute,
	Key_Snapshot,
	Key_Insert,
	Key_Delete,
	Key_Help,

	/* 主键盘上的数字键 */
	Key_0 = 0x30,
	Key_1,
	Key_2,
	Key_3,
	Key_4,
	Key_5,
	Key_6,
	Key_7,
	Key_8,
	Key_9,

	Key_A = 0x41,
	Key_B,
	Key_C,
	Key_D,
	Key_E,
	Key_F,
	Key_G,
	Key_H,
	Key_I,
	Key_J,
	Key_K,
	Key_L,
	Key_M,
	Key_N,
	Key_O,
	Key_P,
	Key_Q,
	Key_R,
	Key_S,
	Key_T,
	Key_U,
	Key_V,
	Key_W,
	Key_X,
	Key_Y,
	Key_Z,

	/* 小键盘上的数字 */
	Key_NumPad_0 = 0x60,
	Key_NumPad_1,
	Key_NumPad_2,
	Key_NumPad_3,
	Key_NumPad_4,
	Key_NumPad_5,
	Key_NumPad_6,
	Key_NumPad_7,
	Key_NumPad_8,
	Key_NumPad_9,

	Key_F1 = 0x70,
	Key_F2,
	Key_F3,
	Key_F4,
	Key_F5,
	Key_F6,
	Key_F7,
	Key_F8,
	Key_F9,
	Key_F10,
	Key_F11,
	Key_F12,
	Key_F13,
	Key_F14,
	Key_F15,
	Key_F16,
	Key_F17,
	Key_F18,
	Key_F19,
	Key_F20,
	Key_F21,
	Key_F22,
	Key_F23,
	Key_F24,
};

static KeyType keyMap(unsigned int key)
{
	return KeyType(key);
}

/* 鼠标事件  */
struct MouseEvent
{
	EventAction event_action;
	ButtonType button_type;
	int delta;
	int x, y;
};

/* 按键事件 */
struct KeyEvent
{
	EventAction event_action;
	bool keys[256];
	KeyType key_type;
};

//------------------------------------------------------------------
// MouseEventListener
//------------------------------------------------------------------
class MouseEventListener : public EventListener
{
public:
	MouseEventListener();
	virtual ~MouseEventListener();

	virtual void MouseMove(const MouseEvent& event) {}
	virtual void MousePressed(const MouseEvent& event) {}
	virtual void MouseReleased(const MouseEvent& event) {}
	virtual void MouseDoubleClick(const MouseEvent& event) {}
	virtual void MouseWheel(const MouseEvent& event) {}

	void HandleEvent(const BaseEvent& event);
};


//------------------------------------------------------------------
// KeyEventListener
//------------------------------------------------------------------
class KeyEventListener : public EventListener
{
public:
	KeyEventListener();
	virtual ~KeyEventListener();

	virtual void KeyPressed(const KeyEvent& event) {}
	virtual void KeyReleased(const KeyEvent& event) {}

	void HandleEvent(const BaseEvent& event);
};


//------------------------------------------------------------------
// WinMsgHandle
// 窗口信息处理
//------------------------------------------------------------------
class WinMsgHandle
{
public:
	WinMsgHandle();

	void HandleMessage(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

	void SetWindowHeight(int height) { window_height = height; }

private:
	BaseEvent base_event;

	KeyEvent key_event;
	MouseEvent mouse_button_event;
	MouseEvent mouse_move_event;

	int window_height;
};