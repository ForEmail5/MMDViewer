#include "Event.h"

//------------------------------------------------------------------
// MouseEventListener
//------------------------------------------------------------------
MouseEventListener::MouseEventListener()
{
	this->AppendListener(EventType::ET_MOUSE, this);
}

MouseEventListener::~MouseEventListener()
{
	this->RemoveListener(EventType::ET_MOUSE, this);
}

void MouseEventListener::HandleEvent(const BaseEvent& event)
{
	if ( event.event_id != EventType::ET_MOUSE && event.user_data ) return;

	MouseEvent* mouseEvent = static_cast<MouseEvent*>(event.user_data);

	switch ( mouseEvent->event_action ) {
	case ACT_MOVE:			this->MouseMove(*mouseEvent);		  break;
	case ACT_PRESS:			this->MousePressed(*mouseEvent);	  break;
	case ACT_RELAESE:		this->MouseReleased(*mouseEvent);	  break;
	case ACT_SCROLL:		this->MouseWheel(*mouseEvent);		  break;
	case ACT_DUBBLE_CLICK:	this->MouseDoubleClick(*mouseEvent);  break;
	}
}


//------------------------------------------------------------------
// KeyEventListener
//------------------------------------------------------------------
KeyEventListener::KeyEventListener()
{
	this->AppendListener(EventType::ET_KEY, this);
}

KeyEventListener::~KeyEventListener()
{
	this->RemoveListener(EventType::ET_KEY, this);
}

void KeyEventListener::HandleEvent(const BaseEvent& event)
{
	if ( event.event_id != EventType::ET_KEY && event.user_data ) return;

	KeyEvent* keyEvent = static_cast<KeyEvent*>(event.user_data);

	switch ( keyEvent->event_action ) {
	case ACT_PRESS:		this->KeyPressed(*keyEvent);  break;
	case ACT_RELAESE:	this->KeyReleased(*keyEvent); break;
	}
}


//------------------------------------------------------------------
// WinMsgHandle
// 窗口信息处理
//------------------------------------------------------------------
WinMsgHandle::WinMsgHandle()
{
	memset(&base_event, 0, sizeof(BaseEvent));

	memset(&key_event, 0, sizeof(KeyEvent));
	memset(&mouse_move_event, 0, sizeof(MouseEvent));
	memset(&mouse_button_event, 0, sizeof(MouseEvent));
}

void WinMsgHandle::HandleMessage(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	base_event.event_id = ET_UNKNOWN;

	/* 鼠标事件信息  */
	if ( msg >= WM_MOUSEMOVE && msg <= WM_MBUTTONDBLCLK || msg == WM_MOUSEWHEEL ) {
		switch ( msg ) {
		case WM_LBUTTONDOWN:
			mouse_button_event.button_type  = ButtonType::LEFT_BUTTON;
			mouse_button_event.event_action = EventAction::ACT_PRESS;
			break;
		case WM_LBUTTONUP:
			mouse_button_event.button_type  = ButtonType::LEFT_BUTTON;
			mouse_button_event.event_action = EventAction::ACT_RELAESE;
			break;
		case WM_LBUTTONDBLCLK:
			mouse_button_event.button_type  = ButtonType::LEFT_BUTTON;
			mouse_button_event.event_action = EventAction::ACT_DUBBLE_CLICK;
			break;
		case WM_MBUTTONDOWN:
			mouse_button_event.button_type  = ButtonType::MIDDLE_BUTTON;
			mouse_button_event.event_action = EventAction::ACT_PRESS;
			break;
		case WM_MBUTTONUP:
			mouse_button_event.button_type  = ButtonType::MIDDLE_BUTTON;
			mouse_button_event.event_action = EventAction::ACT_RELAESE;
			break;
		case WM_MBUTTONDBLCLK:
			mouse_button_event.button_type  = ButtonType::MIDDLE_BUTTON;
			mouse_button_event.event_action = EventAction::ACT_DUBBLE_CLICK;
			break;
		case WM_RBUTTONDOWN:
			mouse_button_event.button_type  = ButtonType::RIGHT_BUTTON;
			mouse_button_event.event_action = EventAction::ACT_PRESS;
			break;
		case WM_RBUTTONUP:
			mouse_button_event.button_type  = ButtonType::RIGHT_BUTTON;
			mouse_button_event.event_action = EventAction::ACT_RELAESE;
			break;
		case WM_RBUTTONDBLCLK:
			mouse_button_event.button_type  = ButtonType::RIGHT_BUTTON;
			mouse_button_event.event_action = EventAction::ACT_DUBBLE_CLICK;
			break;
		case WM_MOUSEMOVE:
			mouse_move_event.button_type  = mouse_button_event.button_type;
			mouse_move_event.event_action = ACT_MOVE;
			break;
		case WM_MOUSEWHEEL:
			mouse_button_event.event_action = EventAction::ACT_SCROLL;
			mouse_button_event.delta = ( short ) HIWORD(wParam);
			break;
		}

		mouse_move_event.x = ( short ) LOWORD(lParam);
		mouse_move_event.y = window_height - ( short ) HIWORD(lParam);

		mouse_button_event.x = ( short ) LOWORD(lParam);
		mouse_button_event.y = window_height - ( short ) HIWORD(lParam);

		base_event.event_id = ET_MOUSE;
		base_event.user_data = msg == WM_MOUSEMOVE ? &mouse_move_event : &mouse_button_event;
		EventDispatcher::GetInstance()->DispatchEvent(base_event);
	}

	/* 键盘按键事件信息 */
	if ( msg == WM_KEYDOWN || msg == WM_KEYUP ) {
		key_event.event_action = (msg == WM_KEYDOWN) ? EventAction::ACT_PRESS : EventAction::ACT_RELAESE;
		key_event.key_type = keyMap(( UINT ) wParam);
		key_event.keys[( UINT ) wParam] = (msg == WM_KEYDOWN) ? true : false;

		base_event.event_id = ET_KEY;
		base_event.user_data = &key_event;
		EventDispatcher::GetInstance()->DispatchEvent(base_event);
	}
}