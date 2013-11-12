#include "DlgParam.h"
#include "ALMRunConfig.h"
#include <wx/wfstream.h>
#define PARAMHISTORY_FILE wxT("config/ParamHistory.txt")

BEGIN_EVENT_TABLE(DlgParam, wxDialog)
	EVT_BUTTON(wxID_OK, DlgParam::OnOKClick)
	EVT_SHOW(DlgParam::OnShow)
END_EVENT_TABLE()

#ifndef _DISABLE_DND_
	#include <wx/dnd.h>
	class FileDropDnd : public wxFileDropTarget
	{
	public:
		FileDropDnd(DlgParam *pOwner) { m_pOwner = pOwner; }
 
		virtual bool OnDropFiles(wxCoord x, wxCoord y,
								 const wxArrayString& filenames);
	private:
		//对话框类，成员TextCtrlPath保存文件路径
		DlgParam *m_pOwner;
 
	};
	bool FileDropDnd::OnDropFiles(wxCoord, wxCoord, const wxArrayString& filenames)
	{
		if (filenames.Count() >= 1)
		{
			m_pOwner->SetValue(filenames[0]);
			return true;
		}
		return false;
	}
#endif

DlgParam::DlgParam( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxDialog( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );
	
	wxBoxSizer* bSizer2;
	bSizer2 = new wxBoxSizer(wxHORIZONTAL);
	this->SetSizer(bSizer2 );

	wxArrayString m_array;
	if (g_config->get_u(ParamHistoryLimit))
	{
		Param_file = PARAMHISTORY_FILE;
		if (!wxFileExists(Param_file))
			tfile.Create(Param_file);

		if (tfile.Open(Param_file))
		{
			size_t i = tfile.GetLineCount();
			for(size_t j=0;j<i;++j)
			{
				m_array.Add(tfile[j].Trim());
			}
		}
	}
	comboBox = new wxComboBox(this, wxID_ANY,wxEmptyString, wxDefaultPosition,wxSize(this->GetSize().GetWidth()-80,this->GetSize().GetHeight()-5),m_array,wxCB_DROPDOWN ); 
	comboBox->AutoComplete(m_array);
	comboBox->SetSelection(0);
	bSizer2->Add(comboBox, 0, wxALL, 5 );

	wxButton* ButtonOk = new wxButton(this,wxID_OK,"确定(&O)",wxDefaultPosition,wxDefaultSize);
	wxButton* ButtonCancel = new wxButton(this,wxID_CANCEL,"取消(&O)",wxDefaultPosition,wxDefaultSize);
	ButtonOk->SetDefault();
	bSizer2->Add(ButtonOk,0,wxALL,5);
	bSizer2->Add(ButtonCancel,0,wxALL,5);
	this->Layout();
	this->Centre( wxBOTH );

#ifndef _DISABLE_DND_
	this->SetDropTarget(new FileDropDnd(this));
#endif
//	this->SetEscapeId(wxID_CLOSE);
}

void DlgParam::OnOKClick(wxCommandEvent& e)
{
	wxString m_value = comboBox->GetValue();
	if (m_value.empty() || !tfile.IsOpened())
		return this->EndModal(wxID_OK);
	int i = comboBox->GetSelection();
	if (i < 0 )
		i = comboBox->FindString(m_value);
	if (i)
	{
		if (i > 0)
			tfile.RemoveLine(i);
		else if (tfile.GetLineCount() >= g_config->get_u(ParamHistoryLimit))
			tfile.RemoveLine(tfile.GetLineCount()-1);
		tfile.InsertLine(comboBox->GetValue(),0);
		tfile.Write();
	}
	this->EndModal(wxID_OK);
}

void DlgParam::OnShow(wxShowEvent& e)
{
	if (e.IsShown())
	{
		this->SetFocus();
		comboBox->SetFocusFromKbd();
	}
	e.Skip();
}

void DlgParam::SetValue(const wxString& str)
{
	comboBox->SetValue(str);
}

wxString DlgParam::getvalue()
{
	return comboBox->GetValue().Trim(true);
}

DlgParam::~DlgParam()
{
	__DEBUG_BEGIN("")
	if (tfile.IsOpened())
		tfile.Close();

#ifndef _DISABLE_DND_
	this->SetDropTarget(NULL);
#endif
	__DEBUG_END("")
}
