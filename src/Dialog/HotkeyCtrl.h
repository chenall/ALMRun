#ifndef _HOTKEYCTRL_H_
#define _HOTKEYCTRL_H_

#include "MerryWx.h"

class HotkeyCtrl : public wxTextCtrl
{
public:
	HotkeyCtrl(wxWindow *parent,
               wxWindowID id,
               const wxString& value = wxEmptyString,
               const wxPoint& pos = wxDefaultPosition,
               const wxSize& size = wxSize(110,-1),
               long style = wxTE_CHARWRAP|wxWANTS_CHARS,
               const wxValidator& validator = wxDefaultValidator,
               const wxString& name = wxTextCtrlNameStr);
	~HotkeyCtrl();

private:
	void onContextMenu(wxContextMenuEvent& e);
	void OnKeyDownEvent(wxKeyEvent& e);
	void OnKeyUpEvent(wxKeyEvent& e);
	virtual void OnFocus( wxFocusEvent& event ) { WinDown = false;event.Skip(); }
	bool WinDown;
private:
	DECLARE_EVENT_TABLE()
};

#endif
