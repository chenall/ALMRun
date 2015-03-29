/////////////////////////////////////////////////////////////////////////////
// Name:        dlgaddnewcmd.cpp
// Purpose:     
// Author:      test
// Modified by: 
// Created:     09/06/2013 11:53:23
// RCS-ID:      
// Copyright:   test
// Licence:     
/////////////////////////////////////////////////////////////////////////////

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma implementation "DlgAddNewCmd.h"
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
#include "ALMRunCommon.h"
#include "DlgAddNewCmd.h"
#include "DlgAddNewDir.h"
#include "MerryLua.h"


#ifndef _DISABLE_DND_
	#include <wx/dnd.h>
	class NewCmdDnd : public wxFileDropTarget
	{
	public:
		NewCmdDnd(DlgAddNewCmd *pOwner) { m_pOwner = pOwner; }
 
		virtual bool OnDropFiles(wxCoord x, wxCoord y,
								 const wxArrayString& filenames);
	private:
		//对话框类，成员TextCtrlPath保存文件路径
		DlgAddNewCmd *m_pOwner;
 
	};
	bool NewCmdDnd::OnDropFiles(wxCoord, wxCoord, const wxArrayString& filenames)
	{
		//只有一个文件时弹出添加命令窗口
		if (filenames.Count() == 1)
		{
			wxString cmd = filenames[0];
			//cmd.Replace('\\','/');
			m_pOwner->cmdLine->SetValue(cmd);
			m_pOwner->cmdName->SetValue(wxFileNameFromPath(cmd));
			cmd.Clear();
			return true;
		}
		return false;
	}
#endif

#ifdef _ALMRUN_CONFIG_H_

////@begin XPM images
////@end XPM images


/*!
 * DlgAddNewCmd type definition
 */

IMPLEMENT_DYNAMIC_CLASS( DlgAddNewCmd, wxDialog )


/*!
 * DlgAddNewCmd event table definition
 */

BEGIN_EVENT_TABLE( DlgAddNewCmd, wxDialog )

////@begin DlgAddNewCmd event table entries
////@end DlgAddNewCmd event table entries
	EVT_BUTTON(wxID_OK, OnOkButtonClick)
	EVT_BUTTON(ID_CMD_BROWSE,OnBrowseClick)
	EVT_BUTTON(ID_CMD_ADD_DIR,OnBrowseClick)
	EVT_TOGGLEBUTTON(ID_TGLBUTTON, OnToggle)
	EVT_TEXT(ID_CMD_CMDLINE,OnCmdUpdate)
	EVT_SHOW(OnShow)
END_EVENT_TABLE()


/*!
 * DlgAddNewCmd constructors
 */

DlgAddNewCmd::DlgAddNewCmd()
{
    Init();
	Create(NULL);
}

DlgAddNewCmd::DlgAddNewCmd(const wxString& cmdID)
{
	long cid;
	if (cmdID.IsNumber() && cmdID.ToLong(&cid))
		this->cmdID = cid;
	Create(NULL);
}

DlgAddNewCmd::DlgAddNewCmd(const long cmdID)
{
	this->cmdID = cmdID;
	this->Create(NULL);
}

DlgAddNewCmd::DlgAddNewCmd( wxWindow* parent, wxWindowID id, const wxString& caption, const wxPoint& pos, const wxSize& size, long style )
{
    Init();
    Create(parent, id, caption, pos, size, style);
}


/*!
 * DlgAddNewCmd creator
 */

bool DlgAddNewCmd::Create( wxWindow* parent, wxWindowID id, const wxString& caption, const wxPoint& pos, const wxSize& size, long style )
{
////@begin DlgAddNewCmd creation
    SetExtraStyle(wxWS_EX_BLOCK_EVENTS);
    wxDialog::Create( parent, id, caption, pos, size, style );

    CreateControls();
    if (GetSizer())
    {
        GetSizer()->SetSizeHints(this);
    }
    Centre();
////@end DlgAddNewCmd creation
    return true;
}


/*!
 * DlgAddNewCmd destructor
 */

DlgAddNewCmd::~DlgAddNewCmd()
{
	__DEBUG_BEGIN("")
#ifndef _DISABLE_DND_
		this->SetDropTarget(NULL);
#endif
////@begin DlgAddNewCmd destruction
////@end DlgAddNewCmd destruction
	__DEBUG_END("")
}

void DlgAddNewCmd::OnBrowseClick(wxCommandEvent& e)
{
	wxString cmd;
	if (e.GetId() == ID_CMD_ADD_DIR)
	{
		cmd = wxDirSelector("选择目录",wxEmptyString,0,wxDefaultPosition,this);
		if (cmd.empty())
			return;
		this->Hide();
		//cmd.Replace("\\","/");
		DlgAddNewDir *dlg = new DlgAddNewDir(this);
		dlg->dirName->SetValue(cmd);
		dlg->SetMode(ADDDIR_FLAG_CMDS);
		//默认参数设置
		dlg->dirExclude->SetValue(g_config->GetDefaultDirExclude());
		dlg->dirInclude->SetValue(g_config->GetDefaultDirInclude());
		dlg->dirSub->SetValue(g_config->GetDefaultDirSub());

		if (dlg->ShowModal() == wxID_OK)
		{
			wxArrayString files;
			wxString Exclude = dlg->dirExclude->GetValue();
			ListFiles(dlg->dirName->GetValue(),&files,dlg->dirInclude->GetValue(),dlg->dirExclude->GetValue(),dlg->dirSub->GetValue());
			if (!files.empty())
			{
				for(int i = files.Count() - 1 ;i>=0;--i)
				{
					//files[i].Replace("\\","/");
					if (g_config->AddCmd(files[i],wxFileNameFromPath(files[i]))<=0)
						ShowErrinfo(ShowCMDErrInfo);;
				}
			}
		}
		dlg->Destroy();
		this->EndModal(wxID_OK);
		return;
	}
	else
	{
		wxFileName fn(cmdLine->GetValue());
		cmd = wxFileSelector("请选择",fn.GetPath(),fn.GetFullName(),wxEmptyString,wxEmptyString,0,this);
	}
	if (!cmd.empty())
	{
		//cmd.Replace("\\","/");
		cmdLine->SetValue(cmd);
	}
}

void DlgAddNewCmd::OnToggle(wxCommandEvent& e)
{
	if (e.IsChecked())
	{
		m_toggleButton->SetLabel("切换=>基本");
		luaBoxSizer->Show(true);
		cmdBoxSizer->Show(false);
	}
	else
	{
		m_toggleButton->SetLabel("切换=>LUA");
		luaBoxSizer->Show(false);
		cmdBoxSizer->Show(true);
	}
	this->GetSizer()->RecalcSizes();
	e.Skip();
}

void DlgAddNewCmd::OnCmdUpdate(wxCommandEvent& e)
{
	e.Skip();
	if (!g_config->get(cmdReadShortcut))
		return;
	wxString cmd = cmdLine->GetValue();
	if (wxFileName(cmd).GetExt().IsSameAs("lnk",true))
	{
		ALMRunCMDBase Lcmd;
		if (ReadShortcut(cmd.c_str(),&Lcmd))
		{
			this->cmdLine->SetValue(Lcmd.cmdLine);
			this->cmdDesc->SetValue(Lcmd.Desc);
			this->cmdPath->SetValue(Lcmd.WorkDir);
			this->cmdName->SetValue(wxFileName(cmd).GetName());
		}
	}
}

void DlgAddNewCmd::OnShow(wxShowEvent& e)
{
	e.Skip();
	if (!e.GetShow())
		return;
	this->SetFocus();
	if (cmdID == -1 || !g_config)
		return;
	ALMRunCMDBase cmd = g_config->GetCmd(cmdID);

	if (cmd.cmdLine.empty())
		return;
	cmdDesc->SetValue(cmd.Desc);
	if (this->flags & CMD_FLAGS_COPY)
	{
		this->SetTitle(wxString::Format("添加新命令-->[%s]副本",cmd.Name));
		this->cmdID = -1;
	}
	else
		cmdKey->SetValue(cmd.Key);
	cmdName->SetValue(cmd.Name);

	wxString lua;
	if (cmd.cmdLine.StartsWith("--LUA",&lua))
	{
		luaCmd->SetValue(UnEscapeString(lua));
		luaBoxSizer->Show(true);
		cmdBoxSizer->Show(false);
		this->GetSizer()->RecalcSizes();
		m_toggleButton->SetLabel("切换=>基本");
		m_toggleButton->SetValue(true);
		return;
	}
	cmdPath->SetValue(cmd.WorkDir);
	size_t i;
	for(i = 0;;++i)
	{
		wxChar c = cmd.cmdLine[i];
		switch(c)
		{
			case '?'://前导'?'唯一进程
				this->isSingleProc->SetValue(true);
				continue;
			case '@'://前导'@'隐藏窗口运行
				this->HideRun->SetValue(true);
				continue;
			case '>'://根据系统判断,NT6以上以管理员权限运行.
			case '*'://以管理员权限运行
				this->RunAs->SetValue(true);
				continue;
			case '+'://强制参数标志
				this->RequiredArg->SetValue(true);
				continue;
			case '|'://命令标志结尾.
				++i;
				break;
			case ' ':
			case '\t':
				continue;
			default:
				break;
		}
		break;
	}
//	cmd.Replace('\\','/');
	cmdLine->SetValue(cmd.cmdLine.substr(i));
}

void DlgAddNewCmd::OnOkButtonClick(wxCommandEvent& e)
{
	if (!g_config)
		return;
	wxString cmd = wxEmptyString;

	if (m_toggleButton->GetValue())
	{
		cmd << "--LUA" << luaCmd->GetValue();
	}
	else
	{
		cmd = cmdLine->GetValue().Trim().Trim(false);
		if (cmd.empty())
		{
			wxMessageBox("命令行不能为空,请重新输入!","提示");
			return;
		}

		if (importCMD(cmd)>0)
		{
			if (this->IsModal())
				this->EndModal(wxID_OK);
			else
				this->Destroy();
			return;
		}
		cmdLine->SetValue(cmd);
		wxString cmdFlag = wxEmptyString;

		if (RequiredArg->IsChecked())
			cmdFlag += '+';
		if (isSingleProc->IsChecked())
			cmdFlag += '?';
		if (RunAs->IsChecked())
			cmdFlag += '>';
		if (HideRun->IsChecked())
			cmdFlag += '@';
		if (!cmdFlag.empty())
		{
			cmdFlag += '|';
			cmd.insert(0,cmdFlag);
		}
	}
	if (wxGetKeyState(WXK_CONTROL))
		cmdID = -1;
	if ((cmdID != -1 && g_config->ModifyCmd(cmdID,cmd,cmdName->GetValue(),cmdKey->GetValue(),cmdDesc->GetValue(),cmdPath->GetValue()))
		|| (cmdID = g_config->AddCmd(cmd,cmdName->GetValue(),cmdKey->GetValue(),cmdDesc->GetValue(),cmdPath->GetValue()))>0 )
	{
		if (this->IsModal())
			this->EndModal(wxID_OK);
		else
			this->Destroy();
	}
	else
		ShowErrinfo();
}

/*!
 * Member initialisation
 */

void DlgAddNewCmd::Init()
{
////@begin DlgAddNewCmd member initialisation
	cmdID = -1;
	this->flags = 0;
////@end DlgAddNewCmd member initialisation
}


/*!
 * Control creation for DlgAddNewCmd
 */

void DlgAddNewCmd::CreateControls()
{    
////@begin DlgAddNewCmd content construction
    DlgAddNewCmd* itemDialog1 = this;
	bool ShowToolTip = DlgAddNewCmd::ShowToolTips();

    itemBoxSizer2 = new wxBoxSizer(wxVERTICAL);
    itemDialog1->SetSizer(itemBoxSizer2);

   	wxStaticBox* MainBoxSizerStatic = new wxStaticBox(itemDialog1, wxID_ANY, "命令参数");

	wxStaticBoxSizer* MainBoxSizer = new wxStaticBoxSizer(MainBoxSizerStatic, wxVERTICAL);
//    MainBoxSizerStatic->SetFont(wxFont(9, wxSWISS, wxNORMAL, wxNORMAL, false, wxT("Tahoma")));
    itemBoxSizer2->Add(MainBoxSizer, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 5);

    wxBoxSizer* itemBoxSizer4 = new wxBoxSizer(wxHORIZONTAL);
	 wxBoxSizer* itemBoxSizer_hotkey = new wxBoxSizer(wxHORIZONTAL);
    MainBoxSizer->Add(itemBoxSizer4, 0, wxGROW|wxLEFT, 5);
	MainBoxSizer->Add(itemBoxSizer_hotkey, 0, wxGROW|wxLEFT, 5);

    wxStaticText* itemStaticText5 = new wxStaticText( MainBoxSizer->GetStaticBox(), wxID_STATIC, "名称:", wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer4->Add(itemStaticText5, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    cmdName = new wxTextCtrl( MainBoxSizer->GetStaticBox(), ID_CMD_NAME, wxEmptyString, wxDefaultPosition, wxSize(300, -1), 0 );
    cmdName->SetMaxLength(30);
	setWinHelpText(cmdName,"快捷命令,列表显示的内容.",ShowToolTip);
    itemBoxSizer4->Add(cmdName, 0, wxGROW|wxALL, 5);

    wxStaticText* itemStaticText7 = new wxStaticText( MainBoxSizer->GetStaticBox(), wxID_STATIC, "热键:", wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer_hotkey->Add(itemStaticText7, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    //cmdKey = new wxTextCtrl( MainBoxSizer->GetStaticBox(), ID_CMD_KEY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );

	//    itemBoxSizer4->Add(cmdKey, 0, wxALIGN_CENTER_VERTICAL|wxLEFT|wxTOP|wxBOTTOM, 5);
	cmdKey = new HotkeyCtrl(MainBoxSizer->GetStaticBox(),ID_CMD_KEY,wxEmptyString,wxDefaultPosition,wxSize(300, -1));
	setWinHelpText(cmdKey,"可选参数,命令对应全局热键",ShowToolTip);
	itemBoxSizer_hotkey->Add(cmdKey, 0, wxGROW|wxALL, 5);

    wxBoxSizer* itemBoxSizer9 = new wxBoxSizer(wxHORIZONTAL);
    MainBoxSizer->Add(itemBoxSizer9, 0, wxGROW|wxLEFT, 5);

    wxStaticText* itemStaticText10 = new wxStaticText( MainBoxSizer->GetStaticBox(), wxID_STATIC, "备注:", wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer9->Add(itemStaticText10, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    cmdDesc = new wxTextCtrl( MainBoxSizer->GetStaticBox(), ID_CMD_DESC, wxEmptyString, wxDefaultPosition, wxSize(300, -1), 0 );
	setWinHelpText(cmdDesc,"可选参数,命令备注或提示信息",ShowToolTip);
    itemBoxSizer9->Add(cmdDesc, 0, wxGROW|wxALL, 5);

	//命令相关信息
	itemBoxSizerToggle = new wxBoxSizer(wxVERTICAL);
	MainBoxSizer->Add(itemBoxSizerToggle, 0, wxGROW, 5);

	//LUA脚本模式
	luaBoxSizer = new wxBoxSizer(wxHORIZONTAL);
	itemBoxSizerToggle->Add(luaBoxSizer,0,wxGROW|wxALL);

	luaCmd = new wxTextCtrl(MainBoxSizer->GetStaticBox(), wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize(350,170), wxTE_MULTILINE);
	luaBoxSizer->Add(luaCmd,0,wxGROW|wxALL,1);
	setWinHelpText(luaCmd,"高级模式,使用LUA脚本,请输入LUA脚本内容\n注:在脚本中 args:用户输入的参数,cmdID: 命令对应内部ID(非固定)",ShowToolTip);
	luaCmd->Show(false);
	cmdBoxSizer = new wxBoxSizer(wxVERTICAL);
	itemBoxSizerToggle->Add(cmdBoxSizer, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 5);

    wxBoxSizer* itemBoxSizer13 = new wxBoxSizer(wxHORIZONTAL);
    cmdBoxSizer->Add(itemBoxSizer13, 0, wxGROW|wxLEFT, 5);

    wxStaticText* itemStaticText14 = new wxStaticText( MainBoxSizer->GetStaticBox(), wxID_STATIC, "命令:", wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer13->Add(itemStaticText14, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    cmdLine = new wxTextCtrl( MainBoxSizer->GetStaticBox(), ID_CMD_CMDLINE, wxEmptyString, wxDefaultPosition, wxSize(220, -1), 0 );
	cmdLine->AutoCompleteFileNames();
	setWinHelpText(cmdLine,"必要参数,要执行的命令行",ShowToolTip);
    itemBoxSizer13->Add(cmdLine, 0, wxGROW|wxALL, 5);

    wxButton* itemButton16 = new wxButton( MainBoxSizer->GetStaticBox(), ID_CMD_BROWSE, "浏览(&B)", wxDefaultPosition, wxSize(60, -1), 0 );
    itemBoxSizer13->Add(itemButton16, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

	wxBoxSizer* cmdPathBoxSizer = new wxBoxSizer(wxHORIZONTAL);
    cmdBoxSizer->Add(cmdPathBoxSizer, 0, wxGROW|wxLEFT, 5);

    wxStaticText* cmdPathText = new wxStaticText( MainBoxSizer->GetStaticBox(), wxID_STATIC, "工作目录:", wxDefaultPosition, wxDefaultSize, 0 );
    cmdPathBoxSizer->Add(cmdPathText, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    cmdPath = new wxTextCtrl( MainBoxSizer->GetStaticBox(), ID_CMD_WORKDIR, wxEmptyString, wxDefaultPosition, wxSize(260, -1), 0 );
	cmdPath->AutoCompleteDirectories();
	setWinHelpText(cmdPath,"可选参数,程序运行目录,工作目录,留空默认程序所在目录",ShowToolTip);
    cmdPathBoxSizer->Add(cmdPath, 0, wxGROW|wxALL, 5);


//	itemBoxSizerToggle->Hide(luaBoxSizer);//默认隐藏不显示

	//高级选项
	wxGridSizer* itemBoxSizer_checkbox = new wxGridSizer(3);
	wxStaticBoxSizer* advConfig = new wxStaticBoxSizer(new wxStaticBox(MainBoxSizer->GetStaticBox(), wxID_ANY, "命令运行选项"),wxHORIZONTAL);
    cmdBoxSizer->Add(advConfig, 0, wxGROW|wxALL, 5);
	advConfig->Add(itemBoxSizer_checkbox,0,wxGROW|wxALL,5);

	HideRun = new wxCheckBox( MainBoxSizer->GetStaticBox(),wxID_ANY, _T("隐藏执行"));
    HideRun->SetValue(false);
	itemBoxSizer_checkbox->Add(HideRun,0,wxALIGN_CENTER_VERTICAL|wxALL,5);
	RunAs = new wxCheckBox( MainBoxSizer->GetStaticBox(),wxID_ANY, _T("管理员权限"));
	RunAs->SetValue(false);
	itemBoxSizer_checkbox->Add(RunAs,0,wxALIGN_CENTER_VERTICAL|wxALL,5);
	
	isSingleProc = new wxCheckBox( MainBoxSizer->GetStaticBox(),wxID_ANY,_T("唯一进程"));
	isSingleProc->SetValue(false);
	itemBoxSizer_checkbox->Add(isSingleProc,0,wxALIGN_CENTER_VERTICAL|wxALL,5);

	RequiredArg = new wxCheckBox( MainBoxSizer->GetStaticBox(),wxID_ANY, _T("强制要求参数"));
	RunAs->SetValue(false);
	itemBoxSizer_checkbox->Add(RequiredArg,0,wxALIGN_CENTER_VERTICAL|wxALL,5);

    wxBoxSizer* itemBoxSizer17 = new wxBoxSizer(wxHORIZONTAL);
    itemBoxSizer2->Add(itemBoxSizer17, 0, wxGROW|wxALL, 5);
	if (cmdID != -1)
	{
		this->SetTitle("编辑命令参数");
	}
	else
	{
		wxButton* itemButton20 = new wxButton( itemDialog1, ID_CMD_ADD_DIR,"批量添加" , wxDefaultPosition, wxDefaultSize,wxBU_EXACTFIT);
		setWinHelpText(itemButton20,"自动扫描目录批量添加",this->ShowToolTips());
		itemBoxSizer17->Add(itemButton20, 0, wxALIGN_TOP|wxALL, 5);
	}
	
	m_toggleButton = new wxToggleButton(itemDialog1,ID_TGLBUTTON,"切换=>LUA");
	itemBoxSizer17->Add(m_toggleButton, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);
	//确定取消按钮
    wxStdDialogButtonSizer* itemStdDialogButtonSizer = new wxStdDialogButtonSizer;

    itemBoxSizer17->Add(itemStdDialogButtonSizer, 1, wxALIGN_CENTER_VERTICAL|wxALL, 5);
    wxButton* itemButton18 = new wxButton( itemDialog1, wxID_OK, "确定(&O)", wxDefaultPosition, wxDefaultSize, 0 );
    itemButton18->SetDefault();
    itemStdDialogButtonSizer->AddButton(itemButton18);

    wxButton* itemButton19 = new wxButton( itemDialog1, wxID_CANCEL,"取消(&C)", wxDefaultPosition, wxDefaultSize, 0 );
    itemStdDialogButtonSizer->AddButton(itemButton19);
    itemStdDialogButtonSizer->Realize();
	cmdName->SetFocus();
	cmdKey->MoveAfterInTabOrder(RequiredArg);
#ifndef _DISABLE_DND_
	this->SetDropTarget(new NewCmdDnd(this));
#endif
	this->flags = 0;
////@end DlgAddNewCmd content construction
}


/*!
 * Should we show tooltips?
 */

bool DlgAddNewCmd::ShowToolTips()
{
    return true;
}

/*!
 * Get bitmap resources
 */

wxBitmap DlgAddNewCmd::GetBitmapResource( const wxString& name )
{
    // Bitmap retrieval
////@begin DlgAddNewCmd bitmap retrieval
    wxUnusedVar(name);
    return wxNullBitmap;
////@end DlgAddNewCmd bitmap retrieval
}

/*!
 * Get icon resources
 */

wxIcon DlgAddNewCmd::GetIconResource( const wxString& name )
{
    // Icon retrieval
////@begin DlgAddNewCmd icon retrieval
    wxUnusedVar(name);
    return wxNullIcon;
////@end DlgAddNewCmd icon retrieval
}
#endif