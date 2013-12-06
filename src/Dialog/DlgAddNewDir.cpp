/////////////////////////////////////////////////////////////////////////////
// Name:        DlgAddNewDir.cpp
// Purpose:     
// Author:      test
// Modified by: 
// Created:     09/06/2013 21:11:35
// RCS-ID:      
// Copyright:   test
// Licence:     
/////////////////////////////////////////////////////////////////////////////

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma implementation "DlgAddNewDir.h"
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

////@begin includes

////@end includes

#include "ALMRunConfig.h"
#ifdef _ALMRUN_CONFIG_H_
#include "DlgAddNewDir.h"

////@begin XPM images

#include "find.xpm"
////@end XPM images

#ifndef _DISABLE_DND_
	#include <wx/dnd.h>
	class NewDirDnd : public wxFileDropTarget
	{
	public:
		NewDirDnd(DlgAddNewDir *pOwner) { m_pOwner = pOwner; }
 
		virtual bool OnDropFiles(wxCoord x, wxCoord y,
								 const wxArrayString& filenames);
	private:
		//对话框类，成员TextCtrlPath保存文件路径
		DlgAddNewDir *m_pOwner;
 
	};
	bool NewDirDnd::OnDropFiles(wxCoord, wxCoord, const wxArrayString& filenames)
	{
		//只有一个文件时弹出添加命令窗口
		if (filenames.Count() == 1)
		{
			wxString cmd = filenames[0];
			if (wxDirExists(cmd))
			{
				cmd.Replace('\\','/');
				m_pOwner->dirName->SetValue(cmd);
				cmd.Clear();
				return true;
			}
		}
		return false;
	}
#endif

/*!
 * DlgAddNewDir type definition
 */

IMPLEMENT_DYNAMIC_CLASS( DlgAddNewDir, wxDialog )


/*!
 * DlgAddNewDir event table definition
 */

BEGIN_EVENT_TABLE( DlgAddNewDir, wxDialog )

////@begin DlgAddNewDir event table entries
////@end DlgAddNewDir event table entries
EVT_BUTTON(ID_BUTTON,onButtonClick)
EVT_BUTTON(wxID_OK,onButtonClick)
EVT_CHECKBOX(ID_ADD_AS_CMD,onCheckBoxClick)
END_EVENT_TABLE()


/*!
 * DlgAddNewDir constructors
 */

DlgAddNewDir::DlgAddNewDir()
{
    Init();
}

DlgAddNewDir::DlgAddNewDir( wxWindow* parent, wxWindowID id, const wxString& caption, const wxPoint& pos, const wxSize& size, long style )
{
    Init();
    Create(parent, id, caption, pos, size, style);
}


/*!
 * DlgAddNewDir creator
 */

bool DlgAddNewDir::Create( wxWindow* parent, wxWindowID id, const wxString& caption, const wxPoint& pos, const wxSize& size, long style )
{
////@begin DlgAddNewDir creation
    SetExtraStyle(wxWS_EX_BLOCK_EVENTS);
    wxDialog::Create( parent, id, caption, pos, size, style );

    CreateControls();
    SetIcon(GetIconResource(wxT("find.xpm")));
    if (GetSizer())
    {
        GetSizer()->SetSizeHints(this);
    }
    Centre();
////@end DlgAddNewDir creation
    return true;
}


/*!
 * DlgAddNewDir destructor
 */

DlgAddNewDir::~DlgAddNewDir()
{
	__DEBUG_BEGIN("")
////@begin DlgAddNewDir destruction
#ifndef _DISABLE_DND_
	this->SetDropTarget(NULL);
#endif
////@end DlgAddNewDir destruction
	__DEBUG_END("")
}


/*!
 * Member initialisation
 */

void DlgAddNewDir::Init()
{
////@begin DlgAddNewDir member initialisation
	flags = 0;
	DirId = -1;
////@end DlgAddNewDir member initialisation
}

/*!
 * Control creation for DlgAddNewDir
 */

void DlgAddNewDir::CreateControls()
{    
////@begin DlgAddNewDir content construction
    DlgAddNewDir* itemDialog1 = this;
	
	wxString dirIncludeHelp = "包含文件,支持通配符*和?,多个使用'|'分隔,例子:*.exe|*.lnk";
	wxString dirExcludeHelp = "排除文件,支持通配符*和?,多个使用'|'分隔,例子:*.exe|*.lnk";
	wxString dirSubHelp = "要扫描子目录层数,-1不限,0不扫描子目录.其它数字,N级子目录";
	wxString dirNameHelp = "添加要扫描的目录,支持多个目录,一行一个";
	if (g_config)
	{
		dirIncludeHelp += wxString::Format("\n\n留空默认值: %s",g_config->GetDefaultDirInclude());
		dirExcludeHelp += wxString::Format("\n\n留空默认值: %s",g_config->GetDefaultDirExclude());
		dirSubHelp += wxString::Format("\n\n留空默认值: %d",g_config->GetDefaultDirSub());
	}

    wxBoxSizer* itemBoxSizer2 = new wxBoxSizer(wxVERTICAL);
    itemDialog1->SetSizer(itemBoxSizer2);

    wxBoxSizer* itemBoxSizer3 = new wxBoxSizer(wxHORIZONTAL);
    itemBoxSizer2->Add(itemBoxSizer3, 0, wxALIGN_CENTER_HORIZONTAL|wxLEFT|wxRIGHT|wxTOP, 5);

    wxStaticText* itemStaticText4 = new wxStaticText( itemDialog1, wxID_STATIC, wxT("目录:"), wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer3->Add(itemStaticText4, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    dirName = new wxTextCtrl( itemDialog1, ID_DLG_DIR_DIR, wxEmptyString, wxDefaultPosition, wxSize(220, -1), wxTE_MULTILINE);
    itemBoxSizer3->Add(dirName, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);
	dirName->SetHelpText(dirNameHelp);
    if (ShowToolTips())
        dirName->SetToolTip(dirNameHelp);

    wxButton* itemButton6 = new wxButton( itemDialog1, ID_BUTTON, _T("浏览(&B)"), wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT );
    itemBoxSizer3->Add(itemButton6, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxBoxSizer* itemBoxSizer7 = new wxBoxSizer(wxHORIZONTAL);
    itemBoxSizer2->Add(itemBoxSizer7, 0, wxGROW|wxLEFT|wxRIGHT, 5);

    wxStaticText* itemStaticText8 = new wxStaticText( itemDialog1, wxID_STATIC, wxT("过滤(包含):"), wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer7->Add(itemStaticText8, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);



	dirInclude = new wxTextCtrl( itemDialog1, ID_DIR_INCLUDE, wxEmptyString, wxDefaultPosition, wxSize(250, -1), 0 );
    itemBoxSizer7->Add(dirInclude, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);
	dirInclude->SetHelpText(dirIncludeHelp);
    if (ShowToolTips())
        dirInclude->SetToolTip(dirIncludeHelp);

    wxBoxSizer* itemBoxSizer10 = new wxBoxSizer(wxHORIZONTAL);
    itemBoxSizer2->Add(itemBoxSizer10, 0, wxGROW|wxLEFT|wxRIGHT, 5);

    wxStaticText* itemStaticText11 = new wxStaticText( itemDialog1, wxID_STATIC, wxT("过滤(排除):"), wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer10->Add(itemStaticText11, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    dirExclude = new wxTextCtrl( itemDialog1, ID_DIR_EXCLUDE, wxEmptyString, wxDefaultPosition, wxSize(250, -1), 0 );
    itemBoxSizer10->Add(dirExclude, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);
	dirExclude->SetHelpText(dirExcludeHelp);
    if (ShowToolTips())
        dirExclude->SetToolTip(dirExcludeHelp);

    wxBoxSizer* itemBoxSizer13 = new wxBoxSizer(wxHORIZONTAL);
    itemBoxSizer2->Add(itemBoxSizer13, 0, wxGROW|wxLEFT|wxRIGHT|wxBOTTOM, 5);

    wxStaticText* itemStaticText14 = new wxStaticText( itemDialog1, wxID_STATIC,wxT("子目录层数:"), wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer13->Add(itemStaticText14, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    dirSub = new wxSpinCtrl( itemDialog1, ID_DIR_SUB, _T("0"), wxDefaultPosition, wxSize(60,-1), wxSP_ARROW_KEYS, -1, 100, 0 );
    itemBoxSizer13->Add(dirSub, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);
	dirSub->SetHelpText(dirSubHelp);
    if (ShowToolTips())
        dirSub->SetToolTip(dirSubHelp);

	wxString AddAsCmdHelp("选中把该目录作为一个命令添加");
	AddAsCmd = new wxCheckBox(itemDialog1,ID_ADD_AS_CMD,wxT("命令"));
	itemBoxSizer13->Add(AddAsCmd,0,wxALIGN_CENTER_VERTICAL|wxALL,5);
	AddAsCmd->SetHelpText(AddAsCmdHelp);
    if (ShowToolTips())
        AddAsCmd->SetToolTip(AddAsCmdHelp);

    wxStdDialogButtonSizer* itemStdDialogButtonSizer16 = new wxStdDialogButtonSizer;

    itemBoxSizer13->Add(itemStdDialogButtonSizer16, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);
    wxButton* itemButton17 = new wxButton( itemDialog1, wxID_OK, wxT("确定(&O)"), wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT );
	itemButton17->SetDefault();
    itemStdDialogButtonSizer16->AddButton(itemButton17);

    wxButton* itemButton18 = new wxButton( itemDialog1, wxID_CANCEL, wxT("取消(&C)"), wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT );
    itemStdDialogButtonSizer16->AddButton(itemButton18);

    itemStdDialogButtonSizer16->Realize();
#ifndef _DISABLE_DND_
	this->SetDropTarget(new NewDirDnd(this));
#endif
////@end DlgAddNewDir content construction
}

void DlgAddNewDir::onCheckBoxClick(wxCommandEvent& e)
{
	switch(e.GetId())
	{
		case ID_ADD_AS_CMD:
			if (e.IsChecked())
			{
				this->dirExclude->Disable();
				this->dirInclude->Disable();
				this->dirSub->Disable();
			}
			else
			{
				this->dirExclude->Enable();
				this->dirInclude->Enable();
				this->dirSub->Enable();
			}
	}
}

void DlgAddNewDir::onButtonClick(wxCommandEvent& e)
{
	if (e.GetId() == ID_BUTTON)
	{
		wxString name = wxDirSelector("请选择一个目录",wxEmptyString,0,wxDefaultPosition,this);
		if (!name.empty())
		{
			//name.Replace("\\","/");
			if (AddAsCmd->IsChecked() || dirName->GetValue().empty())
				dirName->SetValue(name);
			else
				dirName->AppendText("\n"+name);
		}
		return;
	}
	if (!g_config)
		return;
	wxString dName = dirName->GetValue();
	dName.Trim(false);
	if (dName.empty())
		return;
	if (AddAsCmd->IsChecked())
	{
		if (g_config->AddCmd(dName) == -1)
			return ShowErrinfo();
		if (this->IsModal())
			this->EndModal(wxID_OK);
		else
			this->Destroy();
		return;
	}
	dName.Replace("\r","");
	dName.Replace("\n","|");
	if (flags < 2 && g_config->AddDir(dName,dirInclude->GetValue(),dirExclude->GetValue(),dirSub->GetValue(),this->DirId) == -1)
	{
		wxMessageBox("添加/修改目录失败,可能是参数错误","提示");
		return;
	}
	this->EndDialog(wxID_OK);
}

int DlgAddNewDir::SetMode(const int mode)
{
	flags = mode;
	switch(mode)
	{
		case ADDDIR_FLAG_EDIT:
			this->SetTitle("扫描参数修改");
			AddAsCmd->Disable();
			break;
		case ADDDIR_FLAG_CMDS:
			this->SetTitle("批量添加命令");
			AddAsCmd->Disable();
			break;
		default:
			this->SetTitle(SYMBOL_DLGADDNEWDIR_TITLE);
			flags = ADDDIR_FLAG_NORMAL;
	}
	return flags;
}
/*!
 * Should we show tooltips?
 */

bool DlgAddNewDir::ShowToolTips()
{
    return true;
}

/*!
 * Get bitmap resources
 */

wxBitmap DlgAddNewDir::GetBitmapResource( const wxString& name )
{
    // Bitmap retrieval
////@begin DlgAddNewDir bitmap retrieval
    wxUnusedVar(name);
    return wxNullBitmap;
////@end DlgAddNewDir bitmap retrieval
}

/*!
 * Get icon resources
 */

wxIcon DlgAddNewDir::GetIconResource( const wxString& name )
{
    // Icon retrieval
////@begin DlgAddNewDir icon retrieval
    wxUnusedVar(name);
    if (name == _T("find.xpm"))
    {
        wxIcon icon(find_xpm);
        return icon;
    }
    return wxNullIcon;
////@end DlgAddNewDir icon retrieval
}
#endif //#ifdef _ALMRUN_CONFIG_H_