#ifndef _MYTEXTCOMPLETER_H_
#define _MYTEXTCOMPLETER_H_

#include "MerryWx.h"
#include <wx/textcompleter.h>
/*
为了实现任意位置匹配,需要修改wxWidget源码
src\msw\textentry.cpp
查找 pAutoComplete2->SetOptions
添加 ACO_NOPREFIXFILTERING 实现任意位置匹配

另一个改动上面同一个文件中
查找 wxBIND_OR_CONNECT_HACK(m_win, wxEVT_AFTER_CHAR,
修改为 wxBIND_OR_CONNECT_HACK(m_win, wxEVT_CHAR,
*/
class MyTextCompleter : public wxTextCompleter
{
public:
	MyTextCompleter(wxComboBox *entry = NULL);
    virtual bool Start(const wxString& prefix);
    virtual wxString GetNext();
private:
	void OnCharHook(wxKeyEvent& event);
	wxString m_last_str;
	wxComboBox *m_entry;
	wxWindow *m_win;
	wxArrayString m_completions;
	unsigned m_index;
	size_t m_last_pos;
};

#endif