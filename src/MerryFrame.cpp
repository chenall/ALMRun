#include "MerryFrame.h"
#include "MerryConfig.h"
#include "MerryMainPanel.h"
#include "MerryListBoxPanel.h"
#include "MerryTaskBarIcon.h"
#include "MerryIcon.xpm"
#include "MerryLua.h"
#include "MerryCommandManager.h"
#include "MerryTimerManager.h"
#include "MerryHotkey.h"
#include "MerryController.h"
#include <wx/sound.h>

//#define DEBUG_ALWAYS_SHOW

BEGIN_EVENT_TABLE(MerryFrame, wxFrame)
//	EVT_CLOSE(MerryFrame::OnCloseEvent)
//	EVT_ACTIVATE(MerryFrame::OnActivateEvent)
	EVT_MENU_RANGE(MENU_ITEM_FIRST,MENU_ITEM_LAST,MerryTaskBarIcon::onPopMenu)
	EVT_SHOW(MerryFrame::OnShowEvent)
END_EVENT_TABLE()

MerryFrame::MerryFrame():
	wxFrame(NULL, wxID_ANY, MERRY_DEFAULT_TITLE, wxDefaultPosition, wxDefaultSize, wxFRAME_NO_TASKBAR | wxBORDER_NONE// | wxSTAY_ON_TOP
#ifdef __WXOSX__
		| wxSTAY_ON_TOP
#endif
	)
{
	this->SetClientSize(MERRY_DEFAULT_WIDTH, MERRY_DEFAULT_HEIGHT);
	m_mainPanel = new MerryMainPanel(this);
	m_listBoxPanel = new MerryListBoxPanel(this);
	m_taskBarIcon = NULL;
//	this->ShowTrayIcon(true);
	m_isCentred = false;
#ifdef DEBUG_ALWAYS_SHOW
	this->Show();
#endif
}

MerryFrame::~MerryFrame()
{
	__DEBUG_BEGIN("")
	if (g_lua)
		OnClose();
	if (m_taskBarIcon)
		wxDELETE(m_taskBarIcon);
	m_taskBarIcon = NULL;
	m_listBoxPanel = NULL;
	m_mainPanel = NULL;
	__DEBUG_END("")
}

void MerryFrame::NewConfig()
{
	__DEBUG_BEGIN("::刷新配置");
	bool show = m_listBoxPanel->IsShown();
	m_listBoxPanel->Dismiss();
	LastCmd = NULL;

	wxDELETE(g_lua);
	wxDELETE(g_config);

	wxDELETE(g_hotkey);
	g_hotkey = NewMerryHotkey();


	wxDELETE(g_timers);
	g_timers = new MerryTimerManager();


	wxDELETE(g_commands);
	g_commands = new MerryCommandManager();

	g_lua = new MerryLua();
#ifdef _ALMRUN_CONFIG_H_

	g_config = new ALMRunConfig();

#endif//#ifdef _ALMRUN_CONFIG_H_

	if (show)
		m_mainPanel->GetTextCtrl()->AppendText("");
#ifdef _ALMRUN_CONFIG_H_
	if (!g_config->get(ShowTip))
		m_listBoxPanel->SetToolTip(NULL);
#endif//#ifdef _ALMRUN_CONFIG_H_
	g_lua->DoConfig();
	__DEBUG_END("::刷新配置");
}

void MerryFrame::OnInit()
{
	assert(!g_controller);
	g_controller = new MerryController();

	assert(!g_timers);
	g_timers = new MerryTimerManager();

	assert(!g_hotkey);
	g_hotkey = NewMerryHotkey();

	assert(!g_commands);
	g_commands = new MerryCommandManager();

	assert(!g_lua);
	g_lua = new MerryLua();
#ifdef _ALMRUN_CONFIG_H_
	assert(!g_config);
	g_config = new ALMRunConfig();
#endif//#ifdef _ALMRUN_CONFIG_H_
	g_lua->DoConfig();
}

void MerryFrame::OpenConfigDir()
{
#ifdef __WXMSW__
#ifdef _ALMRUN_CONFIG_H_
	if (!g_config)
		return;
	wxString pathTmp = g_config->Home;
	pathTmp.Append("config");
	if (g_config->Explorer.empty())
		ShellExecute(NULL,_T("explore"),pathTmp.c_str(),NULL, NULL,SW_SHOW);
	else
		WinExec(wxString::Format("%s \"%s\"",g_config->Explorer,pathTmp),SW_SHOW);
#else//#ifdef _ALMRUN_CONFIG_H_
	::ShellExecute(NULL,_T("explore"),wxGetenv("ALMRUN_HOME"),NULL, NULL,true);
#endif//#ifdef _ALMRUN_CONFIG_H_
#endif
}

void MerryFrame::ShowTrayIcon(const bool show)
{
	if (!show)
	{
		wxDELETE(m_taskBarIcon);
//		if (m_taskBarIcon)
//			m_taskBarIcon->RemoveIcon();
		return;
	}
	if (!m_taskBarIcon)
		m_taskBarIcon = new MerryTaskBarIcon();
	if (m_taskBarIcon->IsIconInstalled())
		return;
	m_taskBarIcon->SetIcon(wxIcon(MerryIcon_xpm), TASKBARICON_TIP);
}

void MerryFrame::OnClose()
{
	__DEBUG_BEGIN("")
	this->Hide();

	wxDELETE(g_lua);
	wxDELETE(g_commands);

#ifdef _ALMRUN_CONFIG_H_
	wxDELETE(g_config);
#endif//#ifdef _ALMRUN_CONFIG_H_
	wxDELETE(g_hotkey);

	wxDELETE(g_timers);

	wxDELETE(g_controller);
	__DEBUG_END("")
}

MerryListBoxPanel* MerryFrame::GetListBoxPanel()
{
	return m_listBoxPanel;
}

//void MerryFrame::OnCloseEvent(wxCloseEvent& e)
//{
//	this->OnClose();
//	e.Skip();
//}
/*
void MerryFrame::OnActivateEvent(wxActivateEvent& e)
{
#ifndef DEBUG_ALWAYS_SHOW
	if (!e.GetActive() && !this->IsShownOnScreen())
		this->Hide();
#endif
}
*/
void MerryFrame::OnShowEvent(wxShowEvent& e)
{
	MerryTextCtrl* textCtrl = m_mainPanel->GetTextCtrl();
	assert(textCtrl);
	if (e.IsShown())
	{
		if (g_config->get(PlayPopupNotify))//是否播放提示音
			wxSound("Popup.wav").Play();
		this->CentreOnce();
		m_listBoxPanel->Dismiss();
		this->Raise();
		textCtrl->SetFocus();
#ifdef __WXMSW__
		textCtrl->SetEnInputMode();
#endif
		if (g_config->get(AutoPopup))
			textCtrl->AppendText(wxT(""));
	}
	else
		textCtrl->ChangeValue(wxT(""));

	textCtrl->EnterArgs = 0;
}

void MerryFrame::CentreOnce()
{
	if (m_isCentred)
		return;

	this->Centre();
	wxPoint position = this->GetPosition();
	position.y -= MERRY_DEFAULT_LIST_BOX_ITEM_HEIGHT * MERRY_DEFAULT_LIST_BOX_ITEM_MAX_NUM / 4;
	this->SetPosition(position);
	
	m_isCentred = true;
}
