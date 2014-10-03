#include "DlgParam.h"
#include "ALMRunConfig.h"
#include "MyTextCompleter.h"
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
	comboBox = new wxComboBox(this, 1013,wxEmptyString, wxDefaultPosition,wxSize(this->GetSize().GetWidth()-80,this->GetSize().GetHeight()-5),m_array,wxCB_DROPDOWN );
	comboBox->AutoComplete(new MyTextCompleter(comboBox));
	comboBox->SetSelection(0);
	comboBox->Bind(wxEVT_KEY_DOWN,&DlgParam::OnKey,this);
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

void DlgParam::OnKey(wxKeyEvent& e)
{
	if (e.GetKeyCode() == WXK_DELETE && e.GetModifiers() == wxMOD_ALT)
	{
		int i = m_array.Index(comboBox->GetValue());
		if (i != wxNOT_FOUND)
		{
			if (wxMessageBox(wxString::Format("确定要删除参数历史记录:\n [ %s ]",comboBox->GetValue()),"提示",wxYES_NO|wxICON_WARNING) == wxYES)
			{
				m_array.RemoveAt(i);
				tfile.RemoveLine(i);
				tfile.Write();
				comboBox->Set(m_array);
				return;
			}
		}
	}
	e.Skip();
}

void DlgParam::OnOKClick(wxCommandEvent& e)
{
	wxString m_value = comboBox->GetValue();
	if (m_value.empty() || !tfile.IsOpened())
		return this->EndModal(wxID_OK);
	int i = comboBox->FindString(m_value);

	if (i > 0)
	{
		tfile.RemoveLine(i);
		if (tfile.GetLineCount() >= g_config->get_u(ParamHistoryLimit))
			tfile.RemoveLine(tfile.GetLineCount()-1);
	}

	if (i)
		tfile.InsertLine(comboBox->GetValue(),0);

	tfile.Write();
	this->EndModal(wxID_OK);
}

void DlgParam::OnShow(wxShowEvent& e)
{
	if (e.IsShown())
	{
	#ifdef __WXMSW__
		::SetWindowPos(this->GetHWND(),HWND_TOPMOST,0,0,0,0,SWP_NOMOVE|SWP_NOSIZE);
	#endif
		comboBox->SetFocus();
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
