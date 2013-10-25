#include "DlgParam.h"
#include <wx/wfstream.h>

BEGIN_EVENT_TABLE(DlgParam, wxDialog)
//	EVT_BUTTON(wxID_OK, DlgParam::OnOKClick)
	EVT_SHOW(DlgParam::OnShow)
END_EVENT_TABLE()

DlgParam::DlgParam( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxDialog( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );
	
	wxBoxSizer* bSizer2;
	bSizer2 = new wxBoxSizer(wxHORIZONTAL);
	this->SetSizer(bSizer2 );
	comboBox = new wxComboBox( this, wxID_ANY,wxEmptyString, wxDefaultPosition,wxSize(this->GetSize().GetWidth()-80,this->GetSize().GetHeight()-5), 0, NULL, 0 ); 
	bSizer2->Add( comboBox, 0, wxALL, 5 );
	wxButton* ButtonOk = new wxButton(this,wxID_OK,"确定(&O)",wxDefaultPosition,wxDefaultSize);
	wxButton* ButtonCancel = new wxButton(this,wxID_CANCEL,"取消(&O)",wxDefaultPosition,wxDefaultSize);
	ButtonOk->SetDefault();
	bSizer2->Add(ButtonOk,0,wxALL,5);
	bSizer2->Add(ButtonCancel,0,wxALL,5);
	this->Layout();	
	this->Centre( wxBOTH );
//	this->SetEscapeId(wxID_CLOSE);
}

//void DlgParam::OnOKClick(wxCommandEvent& e)
//{
//	//this->EndModal(wxID_OK);dd
//}

void DlgParam::OnShow(wxShowEvent& e)
{
	if (e.IsShown())
		this->SetFocusFromKbd();
	e.Skip();
}

wxString DlgParam::getvalue()
{
	return comboBox->GetValue().Trim(true);
}

DlgParam::~DlgParam()
{
	__DEBUG_BEGIN("")
	__DEBUG_END("")
}
