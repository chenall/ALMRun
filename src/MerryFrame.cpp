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


// #define DEBUG_ALWAYS_SHOW

BEGIN_EVENT_TABLE(MerryFrame, wxFrame)
	EVT_CLOSE(MerryFrame::OnCloseEvent)
	EVT_ACTIVATE(MerryFrame::OnActivateEvent)
	EVT_SHOW(MerryFrame::OnShowEvent)
END_EVENT_TABLE()

MerryFrame::MerryFrame():
	wxFrame(NULL, wxID_ANY, MERRY_DEFAULT_TITLE, wxDefaultPosition, wxDefaultSize, wxFRAME_NO_TASKBAR | wxBORDER_NONE
#ifdef __WXOSX__
		| wxSTAY_ON_TOP
#endif
	)
{
	this->SetClientSize(MERRY_DEFAULT_WIDTH, MERRY_DEFAULT_HEIGHT);
	#ifdef __WXMSW__
	wxStandardPaths std; //<wx/stdpaths.h>
	wxFileName fname = wxFileName(std.GetExecutablePath());
	wxString volume;
	wxString pathTmp = fname.GetPathWithSep(); //<wx/filename.h>
	::wxSetEnv(wxT("ALMRUN_HOME"),pathTmp.c_str());
	wxFileName::SplitVolume(pathTmp,&volume,NULL);
	if (!volume.empty())
	{
		volume.Append(':');
		::wxSetEnv(wxT("ALMRUN_DRIVE"),volume.c_str());
	}
	::wxSetEnv(wxT("ALMRUN_ROOT"),pathTmp.c_str());
	::wxSetWorkingDirectory(pathTmp);
	pathTmp.Clear();
	volume.Clear();
	#endif
	m_mainPanel = new MerryMainPanel(this);
	m_listBoxPanel = new MerryListBoxPanel(this);
	m_taskBarIcon = new MerryTaskBarIcon();
	m_taskBarIcon->SetIcon(wxIcon(MerryIcon_xpm), wxT("ALMRun ±ã½ÝÆô¶¯¹¤¾ß"));

	m_isCentred = false;

#ifdef DEBUG_ALWAYS_SHOW
	this->Show();
#endif
}

MerryFrame::~MerryFrame()
{
	delete m_taskBarIcon;
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
}

void MerryFrame::OpenConfigDir()
{
#ifdef __WXMSW__
	wxString pathTmp;
	::wxGetEnv("ALMRUN_HOME",&pathTmp);
	pathTmp.Append("config");
	::ShellExecute(NULL, NULL,pathTmp.c_str(),NULL, pathTmp.c_str(),true);
	pathTmp.clear();
#endif
}

void MerryFrame::OnClose()
{
	if (g_lua)
	{
		g_lua->OnClose();
		delete g_lua;
		g_lua = NULL;
	}

	if (g_commands)
	{
		delete g_commands;
		g_commands = NULL;
	}

	if (g_hotkey)
	{
		g_hotkey->OnDelete();
		delete g_hotkey;
		g_hotkey = NULL;
	}

	if (g_timers)
	{
		delete g_timers;
		g_timers = NULL;
	}

	if (g_controller)
	{
		delete g_controller;
		g_controller = NULL;
	}
}

MerryListBoxPanel* MerryFrame::GetListBoxPanel()
{
	return m_listBoxPanel;
}

void MerryFrame::OnCloseEvent(wxCloseEvent& e)
{
	this->OnClose();
	this->Destroy();
}

void MerryFrame::OnActivateEvent(wxActivateEvent& e)
{
#ifndef DEBUG_ALWAYS_SHOW
	if (!e.GetActive())
		this->Hide();
#endif
}

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
