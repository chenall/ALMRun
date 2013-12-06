#ifndef _MERRY_INFORMATION_DIALOG_H_
#define _MERRY_INFORMATION_DIALOG_H_

#include "MerryWx.h"

class MerryInformationDialog : public wxDialog
{
public:
	MerryInformationDialog(const wxString& title, const wxString& information);
	MerryInformationDialog(const wxString& title, const wxString& information,const wxString& check);
	bool isChecked();

private:
	void OnButtonOKEvent(wxCommandEvent& e);
	void OnCloseEvent(wxCloseEvent& e);
	wxCheckBox *check_flag;

private:
	DECLARE_EVENT_TABLE()
};

#endif
