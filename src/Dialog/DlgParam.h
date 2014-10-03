#ifndef __ALMRUN_DlgParam__
#define __ALMRUN_DlgParam__
#include "MerryWx.h"
#include <wx/textfile.h>

#define PARAMHISTORY_FILE wxT("config/ParamHistory.txt")

class DlgParam : public wxDialog 
{
	private:
		DECLARE_EVENT_TABLE();

	protected:
		void OnKey(wxKeyEvent& event);
		wxComboBox* comboBox;
		wxTextFile tfile;
		wxString Param_file;
	private:
		wxArrayString m_array;
		wxArrayString m_completions;
	public:
		
		DlgParam(wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxEmptyString, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize(400,70), long style = wxDEFAULT_DIALOG_STYLE|wxSTAY_ON_TOP);
		~DlgParam();
		void OnOKClick(wxCommandEvent& e);
		//void OnCancelClick(wxCommandEvent& e);
		void OnShow(wxShowEvent& e);
		void SetValue(const wxString& str);
		wxString getvalue();
};

#endif //__ALMRUN_DlgParam__
