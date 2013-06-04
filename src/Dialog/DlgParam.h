#ifndef __ALMRUN_DlgParam__
#define __ALMRUN_DlgParam__
#include "MerryWx.h"

class DlgParam : public wxDialog 
{
	private:
		DECLARE_EVENT_TABLE();
	protected:
		wxComboBox* comboBox;
	public:
		
		DlgParam(wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxEmptyString, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize(300,60), long style = wxDEFAULT_DIALOG_STYLE );
		~DlgParam();
		void OnOKClick(wxCommandEvent& e);
		void OnCancelClick(wxCommandEvent& e);
		wxString getvalue();
};

#endif //__ALMRUN_DlgParam__
