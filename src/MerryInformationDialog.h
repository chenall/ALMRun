#ifndef _MERRY_INFORMATION_DIALOG_H_
#define _MERRY_INFORMATION_DIALOG_H_

#include "MerryWx.h"

class MerryInformationDialog : public wxDialog
{
public:
	MerryInformationDialog(const wxString& title, const wxString& information);

private:
	void OnButtonOKEvent(wxCommandEvent& e);
	void OnCloseEvent(wxCloseEvent& e);

private:
	DECLARE_EVENT_TABLE()
};

#endif
