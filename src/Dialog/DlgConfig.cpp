/////////////////////////////////////////////////////////////////////////////
// Name:        DlgConfig.cpp
// Purpose:     
// Author:      test
// Modified by: 
// Created:     12/06/2013 09:14:07
// RCS-ID:      
// Copyright:   test
// Licence:     
/////////////////////////////////////////////////////////////////////////////

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma implementation "DlgConfig.h"
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
#include "DlgConfig.h"
////@begin XPM images
////@end XPM images


/*!
 * DlgConfig type definition
 */

IMPLEMENT_DYNAMIC_CLASS( DlgConfig, wxDialog )


/*!
 * DlgConfig event table definition
 */

BEGIN_EVENT_TABLE( DlgConfig, wxDialog )

////@begin DlgConfig event table entries
////@end DlgConfig event table entries
//EVT_CHECKLISTBOX(ID_CHECKLISTBOX, OnCheck)
END_EVENT_TABLE()


/*!
 * DlgConfig constructors
 */

DlgConfig::DlgConfig()
{
    Init();
}

DlgConfig::DlgConfig( wxWindow* parent, wxWindowID id, const wxString& caption, const wxPoint& pos, const wxSize& size, long style )
{
    Init();
    Create(parent, id, caption, pos, size, style);
}


/*!
 * DlgConfig creator
 */

bool DlgConfig::Create( wxWindow* parent, wxWindowID id, const wxString& caption, const wxPoint& pos, const wxSize& size, long style )
{
////@begin DlgConfig creation
    SetExtraStyle(wxWS_EX_BLOCK_EVENTS);
    wxDialog::Create( parent, id, caption, pos, size, style );

    CreateControls();
    if (GetSizer())
    {
        GetSizer()->SetSizeHints(this);
    }
    Centre();
////@end DlgConfig creation
    return true;
}


/*!
 * DlgConfig destructor
 */

DlgConfig::~DlgConfig()
{
	__DEBUG_BEGIN("")
////@begin DlgConfig destruction
	this->Disconnect(ID_CHECKLISTBOX,wxEVT_COMMAND_LISTBOX_SELECTED,wxObjectEventFunction(&DlgConfig::OnConfigCheck));
////@end DlgConfig destruction
	__DEBUG_END("")
}


/*!
 * Member initialisation
 */

void DlgConfig::Init()
{
////@begin DlgConfig member initialisation
    config = NULL;
////@end DlgConfig member initialisation
}

void DlgConfig::OnCheck(wxCommandEvent& e)
{
	e.Skip();
}

void DlgConfig::OnConfigCheck(wxCommandEvent& e)
{
	if (!g_config)
		return;
	config_tip->SetLabelText(g_config->config_tip[e.GetSelection()]);
	config_tip->SetToolTip(config_tip->GetLabelText());
}

void DlgConfig::OnMouseEvent(wxMouseEvent& e)
{
	e.Skip();
}
/*!
 * Control creation for DlgConfig
 */

void DlgConfig::CreateControls()
{    
////@begin DlgConfig content construction
    DlgConfig* itemDialog1 = this;

    wxBoxSizer* itemBoxSizer2 = new wxBoxSizer(wxVERTICAL);
    itemDialog1->SetSizer(itemBoxSizer2);


	wxStaticBox* itemStaticBoxSizer1Static = new wxStaticBox(itemDialog1, wxID_ANY,"程序热键");
	wxStaticBoxSizer* itemStaticBoxSizer4 = new wxStaticBoxSizer(itemStaticBoxSizer1Static,wxVERTICAL);
	wxBoxSizer* itemBoxSizer3 = new wxBoxSizer(wxHORIZONTAL);
    itemStaticBoxSizer4->Add(itemBoxSizer3, 0, wxGROW|wxALL, 5);

    wxStaticText* itemStaticText4 = new wxStaticText( itemStaticBoxSizer4->GetStaticBox(), wxID_STATIC, _("显隐热键:"), wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer3->Add(itemStaticText4, 0, wxALIGN_TOP|wxALL, 5);

    config_hotkey = new HotkeyCtrl( itemStaticBoxSizer4->GetStaticBox(), ID_TEXTCTRL, wxEmptyString, wxDefaultPosition,wxSize(150, -1));
    itemBoxSizer3->Add(config_hotkey, 0, wxGROW|wxALL, 5);

    itemBoxSizer2->Add(itemStaticBoxSizer4, 0, wxGROW|wxALL, 5);

	wxStaticBox* itemStaticBoxSizer3Static = new wxStaticBox(itemDialog1, wxID_ANY, "基本配置");
    wxStaticBoxSizer* itemStaticBoxSizer3 = new wxStaticBoxSizer(itemStaticBoxSizer3Static, wxVERTICAL);
    itemBoxSizer2->Add(itemStaticBoxSizer3, 0, wxGROW|wxALL, 5);
    wxArrayString configStrings;
	configStrings.Add("随系统自动启动");
	configStrings.Add("保持窗口置顶");
    configStrings.Add("允许数字快捷键");
    configStrings.Add("显示托盘图标");
    configStrings.Add("仅显示前10项");
	configStrings.Add("显示命令行");
    configStrings.Add("仅剩一项时立即执行");
	configStrings.Add("保存最近一次匹配");
	configStrings.Add("从任意位置匹配关键字");
    configStrings.Add("数字序列顺序 = 0, 1, ..., 8, 9");
    configStrings.Add("前辍匹配优先");
	configStrings.Add("显示命令提示信息");
	configStrings.Add("禁用WOW64文件系统重定向");
	configStrings.Add("添加到<发送到>菜单");
	configStrings.Add("主窗体弹出时播放声音");
	configStrings.Add("按空格键执行");
	configStrings.Add("主窗体弹出时显示列表");
	configStrings.Add("按两次程序热键重复执行上一个命令");
	configStrings.Add("鼠标双击运行");
	configStrings.Add("允许命令名重复");
	configStrings.Add("禁止同一命令多次运行");
	configStrings.Add("快捷方式读取目标属性");
	configStrings.Add("保存窗口位置");
    config = new wxCheckListBox(itemStaticBoxSizer3->GetStaticBox(), ID_CHECKLISTBOX, wxDefaultPosition, wxDefaultSize, configStrings, 0 );
	config_tip = new wxStaticText( itemStaticBoxSizer3->GetStaticBox(), wxID_STATIC,wxEmptyString, wxDefaultPosition, wxSize(-1,50), wxALIGN_LEFT|wxST_NO_AUTORESIZE|wxNO_BORDER);
	config_tip->SetForegroundColour(wxColour(255, 0, 255));
    itemStaticBoxSizer3->Add(config, 0, wxGROW|wxALL, 5);
	itemStaticBoxSizer3->Add(config_tip, 0, wxGROW|wxALL, 5);
	wxStdDialogButtonSizer* itemStdDialogButtonSizer6 = new wxStdDialogButtonSizer;

   itemBoxSizer2->Add(itemStdDialogButtonSizer6, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 5);
    wxButton* itemButton7 = new wxButton( itemDialog1, wxID_OK, _("更新(&O)"), wxDefaultPosition, wxDefaultSize, 0 );
	itemButton7->SetDefault();
    itemStdDialogButtonSizer6->AddButton(itemButton7);

    wxButton* itemButton8 = new wxButton( itemDialog1, wxID_CANCEL, _("取消(&C)"), wxDefaultPosition, wxDefaultSize, 0 );
    itemStdDialogButtonSizer6->AddButton(itemButton8);
	itemButton8->SetFocus();

    itemStdDialogButtonSizer6->Realize();

	this->Connect(ID_CHECKLISTBOX,wxEVT_COMMAND_LISTBOX_SELECTED,wxObjectEventFunction(&DlgConfig::OnConfigCheck));

////@end DlgConfig content construction
}


/*!
 * Should we show tooltips?
 */

bool DlgConfig::ShowToolTips()
{
    return true;
}

/*!
 * Get bitmap resources
 */

wxBitmap DlgConfig::GetBitmapResource( const wxString& name )
{
    // Bitmap retrieval
////@begin DlgConfig bitmap retrieval
    wxUnusedVar(name);
    return wxNullBitmap;
////@end DlgConfig bitmap retrieval
}

/*!
 * Get icon resources
 */

wxIcon DlgConfig::GetIconResource( const wxString& name )
{
    // Icon retrieval
////@begin DlgConfig icon retrieval
    wxUnusedVar(name);
    return wxNullIcon;
////@end DlgConfig icon retrieval
}
#endif//ifdef _ALMRUN_CONFIG_H_