#include "MerryKey.h"
#ifdef __WXMSW__
#include <Windows.h>
#endif

MerryKey g_keys;

MerryKey::MerryKey()
{
	m_wxKeys[wxT("BACK")] = WXK_BACK;
	m_wxKeys[wxT("TAB")] = WXK_TAB;
	m_wxKeys[wxT("RETURN")] = WXK_RETURN;
	m_wxKeys[wxT("ESCAPE")] = WXK_ESCAPE;
	m_wxKeys[wxT("SPACE")] = WXK_SPACE;
	m_wxKeys[wxT("DELETE")] = WXK_DELETE;
	m_wxKeys[wxT("LBUTTON")] = WXK_LBUTTON;
	m_wxKeys[wxT("RBUTTON")] = WXK_RBUTTON;
	m_wxKeys[wxT("CANCEL")] = WXK_CANCEL;
	m_wxKeys[wxT("MBUTTON")] = WXK_MBUTTON;
	m_wxKeys[wxT("CLEAR")] = WXK_CLEAR;
	m_wxKeys[wxT("SHIFT")] = WXK_SHIFT;
	m_wxKeys[wxT("ALT")] = WXK_ALT;
	m_wxKeys[wxT("CONTROL")] = WXK_CONTROL;
	m_wxKeys[wxT("MENU")] = WXK_MENU;
	m_wxKeys[wxT("PAUSE")] = WXK_PAUSE;
	m_wxKeys[wxT("CAPITAL")] = WXK_CAPITAL;
	m_wxKeys[wxT("END")] = WXK_END;
	m_wxKeys[wxT("HOME")] = WXK_HOME;
	m_wxKeys[wxT("LEFT")] = WXK_LEFT;
	m_wxKeys[wxT("UP")] = WXK_UP;
	m_wxKeys[wxT("RIGHT")] = WXK_RIGHT;
	m_wxKeys[wxT("DOWN")] = WXK_DOWN;
	m_wxKeys[wxT("SELECT")] = WXK_SELECT;
	m_wxKeys[wxT("PRINT")] = WXK_PRINT;
	m_wxKeys[wxT("EXECUTE")] = WXK_EXECUTE;
	m_wxKeys[wxT("SNAPSHOT")] = WXK_SNAPSHOT;
	m_wxKeys[wxT("INSERT")] = WXK_INSERT;
	m_wxKeys[wxT("HELP")] = WXK_HELP;
	m_wxKeys[wxT("NUMPAD0")] = WXK_NUMPAD0;
	m_wxKeys[wxT("NUMPAD1")] = WXK_NUMPAD1;
	m_wxKeys[wxT("NUMPAD2")] = WXK_NUMPAD2;
	m_wxKeys[wxT("NUMPAD3")] = WXK_NUMPAD3;
	m_wxKeys[wxT("NUMPAD4")] = WXK_NUMPAD4;
	m_wxKeys[wxT("NUMPAD5")] = WXK_NUMPAD5;
	m_wxKeys[wxT("NUMPAD6")] = WXK_NUMPAD6;
	m_wxKeys[wxT("NUMPAD7")] = WXK_NUMPAD7;
	m_wxKeys[wxT("NUMPAD8")] = WXK_NUMPAD8;
	m_wxKeys[wxT("NUMPAD9")] = WXK_NUMPAD9;
	m_wxKeys[wxT("MULTIPLY")] = WXK_MULTIPLY;
	m_wxKeys[wxT("ADD")] = WXK_ADD;
	m_wxKeys[wxT("SEPARATOR")] = WXK_SEPARATOR;
	m_wxKeys[wxT("SUBTRACT")] = WXK_SUBTRACT;
	m_wxKeys[wxT("DECIMAL")] = WXK_DECIMAL;
	m_wxKeys[wxT("DIVIDE")] = WXK_DIVIDE;
	m_wxKeys[wxT("F1")] = WXK_F1;
	m_wxKeys[wxT("F2")] = WXK_F2;
	m_wxKeys[wxT("F3")] = WXK_F3;
	m_wxKeys[wxT("F4")] = WXK_F4;
	m_wxKeys[wxT("F5")] = WXK_F5;
	m_wxKeys[wxT("F6")] = WXK_F6;
	m_wxKeys[wxT("F7")] = WXK_F7;
	m_wxKeys[wxT("F8")] = WXK_F8;
	m_wxKeys[wxT("F9")] = WXK_F9;
	m_wxKeys[wxT("F10")] = WXK_F10;
	m_wxKeys[wxT("F11")] = WXK_F11;
	m_wxKeys[wxT("F12")] = WXK_F12;
	m_wxKeys[wxT("F13")] = WXK_F13;
	m_wxKeys[wxT("F14")] = WXK_F14;
	m_wxKeys[wxT("F15")] = WXK_F15;
	m_wxKeys[wxT("F16")] = WXK_F16;
	m_wxKeys[wxT("F17")] = WXK_F17;
	m_wxKeys[wxT("F18")] = WXK_F18;
	m_wxKeys[wxT("F19")] = WXK_F19;
	m_wxKeys[wxT("F20")] = WXK_F20;
	m_wxKeys[wxT("F21")] = WXK_F21;
	m_wxKeys[wxT("F22")] = WXK_F22;
	m_wxKeys[wxT("F23")] = WXK_F23;
	m_wxKeys[wxT("F24")] = WXK_F24;
	m_wxKeys[wxT("NUMLOCK")] = WXK_NUMLOCK;
	m_wxKeys[wxT("SCROLL")] = WXK_SCROLL;
	m_wxKeys[wxT("PAGEUP")] = WXK_PAGEUP;
	m_wxKeys[wxT("PAGEDOWN")] = WXK_PAGEDOWN;
	m_wxKeys[wxT("NUMPAD_SPACE")] = WXK_NUMPAD_SPACE;
	m_wxKeys[wxT("NUMPAD_TAB")] = WXK_NUMPAD_TAB;
	m_wxKeys[wxT("NUMPAD_ENTER")] = WXK_NUMPAD_ENTER;
	m_wxKeys[wxT("NUMPAD_F1")] = WXK_NUMPAD_F1;
	m_wxKeys[wxT("NUMPAD_F2")] = WXK_NUMPAD_F2;
	m_wxKeys[wxT("NUMPAD_F3")] = WXK_NUMPAD_F3;
	m_wxKeys[wxT("NUMPAD_F4")] = WXK_NUMPAD_F4;
	m_wxKeys[wxT("NUMPAD_HOME")] = WXK_NUMPAD_HOME;
	m_wxKeys[wxT("NUMPAD_LEFT")] = WXK_NUMPAD_LEFT;
	m_wxKeys[wxT("NUMPAD_UP")] = WXK_NUMPAD_UP;
	m_wxKeys[wxT("NUMPAD_RIGHT")] = WXK_NUMPAD_RIGHT;
	m_wxKeys[wxT("NUMPAD_DOWN")] = WXK_NUMPAD_DOWN;
	m_wxKeys[wxT("NUMPAD_PAGEUP")] = WXK_NUMPAD_PAGEUP;
	m_wxKeys[wxT("NUMPAD_PAGEDOWN")] = WXK_NUMPAD_PAGEDOWN;
	m_wxKeys[wxT("NUMPAD_END")] = WXK_NUMPAD_END;
	m_wxKeys[wxT("NUMPAD_BEGIN")] = WXK_NUMPAD_BEGIN;
	m_wxKeys[wxT("NUMPAD_INSERT")] = WXK_NUMPAD_INSERT;
	m_wxKeys[wxT("NUMPAD_DELETE")] = WXK_NUMPAD_DELETE;
	m_wxKeys[wxT("NUMPAD_EQUAL")] = WXK_NUMPAD_EQUAL;
	m_wxKeys[wxT("NUMPAD_MULTIPLY")] = WXK_NUMPAD_MULTIPLY;
	m_wxKeys[wxT("NUMPAD_ADD")] = WXK_NUMPAD_ADD;
	m_wxKeys[wxT("NUMPAD_SEPARATOR")] = WXK_NUMPAD_SEPARATOR;
	m_wxKeys[wxT("NUMPAD_SUBTRACT")] = WXK_NUMPAD_SUBTRACT;
	m_wxKeys[wxT("NUMPAD_DECIMAL")] = WXK_NUMPAD_DECIMAL;
	m_wxKeys[wxT("NUMPAD_DIVIDE")] = WXK_NUMPAD_DIVIDE;

#ifdef __WXMSW__
	m_windowsKeys[wxT("BACK")] = VK_BACK;
	m_windowsKeys[wxT("TAB")] = VK_TAB;
	m_windowsKeys[wxT("RETURN")] = VK_RETURN;
	m_windowsKeys[wxT("ESCAPE")] = VK_ESCAPE;
	m_windowsKeys[wxT("SPACE")] = VK_SPACE;
	m_windowsKeys[wxT("DELETE")] = VK_DELETE;
	m_windowsKeys[wxT("LBUTTON")] = VK_LBUTTON;
	m_windowsKeys[wxT("RBUTTON")] = VK_RBUTTON;
	m_windowsKeys[wxT("CANCEL")] = VK_CANCEL;
	m_windowsKeys[wxT("MBUTTON")] = VK_MBUTTON;
	m_windowsKeys[wxT("CLEAR")] = VK_CLEAR;
	m_windowsKeys[wxT("SHIFT")] = VK_SHIFT;
	m_windowsKeys[wxT("ALT")] = VK_MENU;
	m_windowsKeys[wxT("CONTROL")] = VK_CONTROL;
	m_windowsKeys[wxT("MENU")] = VK_MENU;
	m_windowsKeys[wxT("PAUSE")] = VK_PAUSE;
	m_windowsKeys[wxT("CAPITAL")] = VK_CAPITAL;
	m_windowsKeys[wxT("END")] = VK_END;
	m_windowsKeys[wxT("HOME")] = VK_HOME;
	m_windowsKeys[wxT("LEFT")] = VK_LEFT;
	m_windowsKeys[wxT("UP")] = VK_UP;
	m_windowsKeys[wxT("RIGHT")] = VK_RIGHT;
	m_windowsKeys[wxT("DOWN")] = VK_DOWN;
	m_windowsKeys[wxT("SELECT")] = VK_SELECT;
	m_windowsKeys[wxT("PRINT")] = VK_PRINT;
	m_windowsKeys[wxT("EXECUTE")] = VK_EXECUTE;
	m_windowsKeys[wxT("SNAPSHOT")] = VK_SNAPSHOT;
	m_windowsKeys[wxT("INSERT")] = VK_INSERT;
	m_windowsKeys[wxT("HELP")] = VK_HELP;
	m_windowsKeys[wxT("NUMPAD0")] = VK_NUMPAD0;
	m_windowsKeys[wxT("NUMPAD1")] = VK_NUMPAD1;
	m_windowsKeys[wxT("NUMPAD2")] = VK_NUMPAD2;
	m_windowsKeys[wxT("NUMPAD3")] = VK_NUMPAD3;
	m_windowsKeys[wxT("NUMPAD4")] = VK_NUMPAD4;
	m_windowsKeys[wxT("NUMPAD5")] = VK_NUMPAD5;
	m_windowsKeys[wxT("NUMPAD6")] = VK_NUMPAD6;
	m_windowsKeys[wxT("NUMPAD7")] = VK_NUMPAD7;
	m_windowsKeys[wxT("NUMPAD8")] = VK_NUMPAD8;
	m_windowsKeys[wxT("NUMPAD9")] = VK_NUMPAD9;
	m_windowsKeys[wxT("MULTIPLY")] = VK_MULTIPLY;
	m_windowsKeys[wxT("ADD")] = VK_ADD;
	m_windowsKeys[wxT("SEPARATOR")] = VK_SEPARATOR;
	m_windowsKeys[wxT("SUBTRACT")] = VK_SUBTRACT;
	m_windowsKeys[wxT("DECIMAL")] = VK_DECIMAL;
	m_windowsKeys[wxT("DIVIDE")] = VK_DIVIDE;
	m_windowsKeys[wxT("F1")] = VK_F1;
	m_windowsKeys[wxT("F2")] = VK_F2;
	m_windowsKeys[wxT("F3")] = VK_F3;
	m_windowsKeys[wxT("F4")] = VK_F4;
	m_windowsKeys[wxT("F5")] = VK_F5;
	m_windowsKeys[wxT("F6")] = VK_F6;
	m_windowsKeys[wxT("F7")] = VK_F7;
	m_windowsKeys[wxT("F8")] = VK_F8;
	m_windowsKeys[wxT("F9")] = VK_F9;
	m_windowsKeys[wxT("F10")] = VK_F10;
	m_windowsKeys[wxT("F11")] = VK_F11;
	m_windowsKeys[wxT("F12")] = VK_F12;
	m_windowsKeys[wxT("F13")] = VK_F13;
	m_windowsKeys[wxT("F14")] = VK_F14;
	m_windowsKeys[wxT("F15")] = VK_F15;
	m_windowsKeys[wxT("F16")] = VK_F16;
	m_windowsKeys[wxT("F17")] = VK_F17;
	m_windowsKeys[wxT("F18")] = VK_F18;
	m_windowsKeys[wxT("F19")] = VK_F19;
	m_windowsKeys[wxT("F20")] = VK_F20;
	m_windowsKeys[wxT("F21")] = VK_F21;
	m_windowsKeys[wxT("F22")] = VK_F22;
	m_windowsKeys[wxT("F23")] = VK_F23;
	m_windowsKeys[wxT("F24")] = VK_F24;
	m_windowsKeys[wxT("NUMLOCK")] = VK_NUMLOCK;
	m_windowsKeys[wxT("SCROLL")] = VK_SCROLL;
	m_windowsKeys[wxT("PAGEUP")] = VK_PRIOR;
	m_windowsKeys[wxT("PAGEDOWN")] = VK_NEXT;

	m_windowsKeys[wxT("`")] = VK_OEM_3;
	m_windowsKeys[wxT("-")] = VK_OEM_MINUS;
	m_windowsKeys[wxT("=")] = VK_OEM_PLUS;
	m_windowsKeys[wxT("[")] = VK_OEM_4;
	m_windowsKeys[wxT("]")] = VK_OEM_6;
	m_windowsKeys[wxT(";")] = VK_OEM_1;
	m_windowsKeys[wxT("'")] = VK_OEM_7;
	m_windowsKeys[wxT("\\")] = VK_OEM_5;
	m_windowsKeys[wxT(",")] = VK_OEM_COMMA;
	m_windowsKeys[wxT(".")] = VK_OEM_PERIOD;
	m_windowsKeys[wxT("/")] = VK_OEM_2;
#endif
}

int MerryKey::GetWxKeyCode(const wxString& key) const
{
	std::map<wxString, int>::const_iterator it = m_wxKeys.find(key);
	if (it == m_wxKeys.end())
		return 0;
	return it->second;
}

#ifdef __WXMSW__
int MerryKey::GetWindowsKeyCode(const wxString& key) const
{
	std::map<wxString, int>::const_iterator it = m_windowsKeys.find(key);
	if (it == m_windowsKeys.end())
		return 0;
	return it->second;
}

wxString MerryKey::GetKeyString(int key) const
{
	if (key >= WXK_NUMPAD0 && key <= WXK_NUMPAD9)
		return wxString::Format("NUMPAD%d",key - WXK_NUMPAD0);
	if (key >= WXK_F1 && key <= WXK_F24)
		return wxString::Format("F%d",key - WXK_F1 + 1);
	std::map<wxString, int>::const_iterator it = m_wxKeys.end();
    it = std::find_if(m_wxKeys.begin(), m_wxKeys.end(), map_value_finder(key));
    if (it == m_wxKeys.end())
       return wxEmptyString;
	return it->first;
}

#endif
