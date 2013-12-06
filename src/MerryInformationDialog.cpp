#include "MerryInformationDialog.h"

BEGIN_EVENT_TABLE(MerryInformationDialog, wxDialog)
	EVT_BUTTON(wxID_OK, MerryInformationDialog::OnButtonOKEvent)
	EVT_CLOSE(MerryInformationDialog::OnCloseEvent)
END_EVENT_TABLE()

MerryInformationDialog::MerryInformationDialog(const wxString& title, const wxString& information):
	wxDialog(NULL, wxID_ANY, title)
{
	wxBoxSizer* sizerTop = new wxBoxSizer(wxVERTICAL);

	sizerTop->Add(new wxStaticText(this, wxID_ANY, information), 0, wxEXPAND | wxALL, 25);
	sizerTop->Add(new wxButton(this, wxID_OK, wxT("OK")), 0, wxDOWN | wxRIGHT | wxUP | wxALIGN_RIGHT, 5);
	sizerTop->SetMinSize(200, 60);

	SetSizer(sizerTop);
	sizerTop->SetSizeHints(this);
	sizerTop->Fit(this);

	this->SetBackgroundColour(wxT("White"));
	this->Centre();
	this->Raise();
	this->Show();
}

MerryInformationDialog::MerryInformationDialog(const wxString& title, const wxString& information,const wxString& check):
	wxDialog(NULL,wxID_ANY,title)
{
	wxBoxSizer* sizerTop = new wxBoxSizer(wxVERTICAL);
	check_flag = new wxCheckBox(this,wxID_ANY,check.empty()?"不再显示此类提示":check);

	sizerTop->Add(new wxStaticText(this, wxID_ANY, information), 0, wxEXPAND | wxALL, 25);
	sizerTop->Add(check_flag,0,wxDOWN | wxRIGHT | wxUP | wxALIGN_RIGHT, 5);
	sizerTop->Add(new wxButton(this, wxID_OK, wxT("OK")), 0, wxDOWN | wxRIGHT | wxUP | wxALIGN_RIGHT, 5);
//	sizerTop->SetMinSize(200, 60);

	SetSizer(sizerTop);
	sizerTop->SetSizeHints(this);
	sizerTop->Fit(this);

	this->SetBackgroundColour(wxT("White"));
	this->Centre();
	this->Raise();
}

bool MerryInformationDialog::isChecked()
{
	if (check_flag && check_flag->IsChecked())
		return true;
	return false;
}

void MerryInformationDialog::OnButtonOKEvent(wxCommandEvent& e)
{
	if (this->IsModal())
		this->EndModal(wxID_OK);
	else
		this->Destroy();
}

void MerryInformationDialog::OnCloseEvent(wxCloseEvent& e)
{
	if (this->IsModal())
		this->EndModal(wxID_CLOSE);
	else
		this->Destroy();
	e.Skip();
}
