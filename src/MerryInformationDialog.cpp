#include "MerryInformationDialog.h"

enum
{
	DIALOG_BUTTON_OK,
};

BEGIN_EVENT_TABLE(MerryInformationDialog, wxDialog)
	EVT_BUTTON(DIALOG_BUTTON_OK, MerryInformationDialog::OnButtonOKEvent)
	EVT_CLOSE(MerryInformationDialog::OnCloseEvent)
END_EVENT_TABLE()

MerryInformationDialog::MerryInformationDialog(const wxString& title, const wxString& information):
	wxDialog(NULL, wxID_ANY, title)
{
	wxBoxSizer* sizerTop = new wxBoxSizer(wxVERTICAL);

	sizerTop->Add(new wxStaticText(this, wxID_ANY, information), 0, wxEXPAND | wxALL, 25);
	sizerTop->Add(new wxButton(this, DIALOG_BUTTON_OK, wxT("OK")), 0, wxDOWN | wxRIGHT | wxUP | wxALIGN_RIGHT, 5);
	sizerTop->SetMinSize(200, 60);

	SetSizer(sizerTop);
	sizerTop->SetSizeHints(this);
	sizerTop->Fit(this);

	this->SetBackgroundColour(wxT("White"));
	this->Centre();
	this->Raise();
	this->Show();
}

void MerryInformationDialog::OnButtonOKEvent(wxCommandEvent& e)
{
	this->Destroy();
}

void MerryInformationDialog::OnCloseEvent(wxCloseEvent& e)
{
	this->Destroy();
	e.Skip();
}
