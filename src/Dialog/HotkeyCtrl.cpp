#include "HotkeyCtrl.h"
#include "MerryKey.h"

BEGIN_EVENT_TABLE(HotkeyCtrl, wxTextCtrl)
	EVT_KEY_DOWN(HotkeyCtrl::OnKeyDownEvent)
	EVT_KEY_UP(HotkeyCtrl::OnKeyUpEvent)
	EVT_SET_FOCUS(HotkeyCtrl::OnFocus)
END_EVENT_TABLE()

HotkeyCtrl::HotkeyCtrl(wxWindow *parent,
               wxWindowID id,
               const wxString& value,
               const wxPoint& pos,
               const wxSize& size,
               long style,
               const wxValidator& validator,
               const wxString& name):
	wxTextCtrl(parent,id,value,pos,size,style,validator,name)
{
	WinDown = false;
}

HotkeyCtrl::~HotkeyCtrl()
{
	__DEBUG_BEGIN("")
	__DEBUG_END("")
}

void HotkeyCtrl::onContextMenu(wxContextMenuEvent& e)
{
	e.Skip();
}

void HotkeyCtrl::OnKeyUpEvent(wxKeyEvent& e)
{
	switch(e.GetKeyCode())
	{
		case WXK_WINDOWS_LEFT:
		case WXK_WINDOWS_RIGHT:
			WinDown = false;
			e.StopPropagation();
			return;
	}
	e.Skip();
}

void HotkeyCtrl::OnKeyDownEvent(wxKeyEvent& e)
{
	e.StopPropagation();
	int code = e.GetKeyCode();
	if (code == WXK_WINDOWS_LEFT || code == WXK_WINDOWS_RIGHT)
	{
		WinDown = true;
		return;
	}
	if (code == WXK_NONE || code == WXK_SHIFT || code == WXK_ALT || code == WXK_CONTROL)
		return;
	if (code == WXK_DELETE)
	{
		this->Clear();
		return;
	}
	wxString key;
	if (code > 32 && code < 127)
		key.Append(e.GetUnicodeKey());
	else
		key = g_keys.GetKeyString(code);
	if (key.empty())
		return;
	if (e.HasModifiers())
	{
		int Modifers = e.GetModifiers();
		if (Modifers & wxMOD_ALT)
			key.insert(0,"Alt+");
		if (Modifers & wxMOD_CONTROL)
			key.insert(0,"Ctrl+");
		if (Modifers & wxMOD_SHIFT)
			key.insert(0,"Shift+");
	}
	if (WinDown)
		key.insert(0,"Win+");
	this->SetValue(key);
}