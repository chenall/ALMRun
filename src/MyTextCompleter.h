#ifndef _MYTEXTCOMPLETER_H_
#define _MYTEXTCOMPLETER_H_

#include "MerryWx.h"
#include <wx/textcompleter.h>

class MyTextCompleter : public wxTextCompleter
{
public:
	MyTextCompleter(wxComboBox *entry = NULL);
    virtual bool Start(const wxString& prefix);
    virtual wxString GetNext();
private:
	wxComboBox *m_entry;
	wxArrayString m_completions;
	unsigned m_index;
};

#endif