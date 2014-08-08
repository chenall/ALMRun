#ifndef _MERRY_TEXT_CTRL_H_
#define _MERRY_TEXT_CTRL_H_

#include "MerryWx.h"
#include "MerryCommandManager.h"

class MerryTextCtrl : public wxTextCtrl
{
public:
	long EnterArgs;
	void ExecuteCmd();
	MerryTextCtrl(wxWindow* parent);
#ifdef __WXMSW__
	void SetEnInputMode(void);
#endif

private:
	void onContextMenu(wxContextMenuEvent& e);
	void OnKeyDownEvent(wxKeyEvent& e);
	void OnMouseEvent(wxMouseEvent& e);
#ifdef __WXGTK__
	void OnIdleEvent(wxIdleEvent& e);
#else
	void OnTextEvent(wxCommandEvent& e);
	void OnCharEvent(wxKeyEvent& e);
#endif

private:
	void AutoCompletion(int keyCode);
	void SetPluginMode(const MerryCommand* cmd);
private:
	int m_lastKeyDownCode;
#ifdef __WXGTK__
	wxString m_lastValue;
	bool m_needCompletion;
#endif

#ifdef __WXMSW__
	HKL hkl;
#endif

private:
	DECLARE_EVENT_TABLE()
};

#endif
