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

//#define DEBUG_ALWAYS_SHOW

BEGIN_EVENT_TABLE(MerryFrame, wxFrame)
//	EVT_CLOSE(MerryFrame::OnCloseEvent)
//	EVT_ACTIVATE(MerryFrame::OnActivateEvent)
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
	this->ShowTrayIcon(true);
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
	__DEBUG_BEGIN("::Ë¢ÐÂÅäÖÃ");
	bool show = m_listBoxPanel->IsShown();
	m_listBoxPanel->Dismiss();
	if (g_lua)
		g_lua->OnClose();

	if (g_hotkey)
	{
//		g_hotkey->OnDelete();
		wxDELETE(g_hotkey);
		g_hotkey = NewMerryHotkey();
	}

	if (g_timers)
	{
		wxDELETE(g_timers);
		g_timers = new MerryTimerManager();
	}

	if (g_commands)
	{
		wxDELETE(g_commands);
		g_commands = new MerryCommandManager();
	}
#ifdef _ALMRUN_CONFIG_H_
	if (g_config)
	{
		wxDELETE(g_config);
		g_config = new ALMRunConfig();
	}
#endif//#ifdef _ALMRUN_CONFIG_H_
	if (lua_bak)
		wxDELETE(lua_bak);

	lua_bak = g_lua;

	g_lua = new MerryLua();
	if (show)
		m_mainPanel->GetTextCtrl()->AppendText("");
#ifdef _ALMRUN_CONFIG_H_
	if (!g_config->config[ShowTip])
		m_listBoxPanel->SetToolTip(NULL);
#endif//#ifdef _ALMRUN_CONFIG_H_
	__DEBUG_END("::Ë¢ÐÂÅäÖÃ");
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
#ifdef _ALMRUN_CONFIG_H_
	assert(!g_config);
	g_config = new ALMRunConfig();
#endif//#ifdef _ALMRUN_CONFIG_H_
	assert(!g_lua);
	g_lua = new MerryLua();
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
		if (m_taskBarIcon)
			m_taskBarIcon->RemoveIcon();
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
	if (lua_bak)
		wxDELETE(lua_bak);

	if (g_lua)
	{
		g_lua->OnClose();
		wxDELETE(g_lua);
	}

	if (g_commands)
		wxDELETE(g_commands);

#ifdef _ALMRUN_CONFIG_H_
	if (g_config)
		wxDELETE(g_config);
#endif//#ifdef _ALMRUN_CONFIG_H_
	if (g_hotkey)
	{
//		g_hotkey->OnDelete();
		wxDELETE(g_hotkey);
	}

	if (g_timers)
	{
		wxDELETE(g_timers);
		g_timers = NULL;
	}

	if (g_controller)
	{
		wxDELETE(g_controller);
		g_controller = NULL;
	}
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
		this->CentreOnce();
		m_listBoxPanel->Dismiss();
		this->Raise();
		textCtrl->SetFocus();
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
