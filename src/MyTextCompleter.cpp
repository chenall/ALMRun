#include "MyTextCompleter.h"

MyTextCompleter::MyTextCompleter(wxComboBox *textctrl)
{
	m_entry = textctrl;
}

bool MyTextCompleter::Start(const wxString& prefix)
{
	long from, to;
    m_entry->GetSelection(&from, &to);

	if ( to == from )
	{
        from = m_entry->GetLastPosition(); // Take all if no selection.
	}

	const wxString find(m_entry->GetRange(0, from).Upper());

	int n = m_entry->GetCount();

	m_index = 0;
	m_completions.clear();

	for(int i=0;i<n;++i)
	{
		wxString str(m_entry->GetString(i));
		size_t pos = str.Upper().find(find);
		if (pos != wxNOT_FOUND)
			m_completions.Add(str);
	}
	if (m_completions.empty())
		return false;

	wxString str(m_completions[0]);
	if (str.Upper().find(find) == 0)
	{
		m_entry->ChangeValue(str.Truncate(find.size() + 16));
		m_entry->SetSelection(find.size(),-1);
	}
	return true;
}

wxString MyTextCompleter::GetNext()
{
   if ( m_index == m_completions.size() )
        return wxString();
    return m_completions[m_index++];
}