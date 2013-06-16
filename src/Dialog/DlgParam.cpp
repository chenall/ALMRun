#include "DlgParam.h"
#include <wx/wfstream.h>

BEGIN_EVENT_TABLE(DlgParam, wxDialog)
	EVT_BUTTON(wxID_OK, DlgParam::OnOKClick)
END_EVENT_TABLE()

DlgParam::DlgParam( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxDialog( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );
	
	wxBoxSizer* bSizer2;
	bSizer2 = new wxBoxSizer(wxHORIZONTAL);
	this->SetSizer(bSizer2 );
	comboBox = new wxComboBox( this, wxID_ANY,wxEmptyString, wxDefaultPosition,wxSize(this->GetSize().GetWidth()-80,this->GetSize().GetHeight()-5), 0, NULL, 0 ); 
	bSizer2->Add( comboBox, 0, wxALL, 5 );
	wxButton* ButtonOk = new wxButton(this,wxID_OK,"È·¶¨(&O)",wxDefaultPosition,wxDefaultSize);
	bSizer2->Add(ButtonOk,0,wxALL,5);
	this->Layout();	
	this->Centre( wxBOTH );
}

void DlgParam::OnOKClick(wxCommandEvent& e)
{
	this->EndModal(wxID_OK);
}

wxString DlgParam::getvalue()
{
	return comboBox->GetValue();
}

DlgParam::~DlgParam()
{

}
