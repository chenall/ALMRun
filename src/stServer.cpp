#include "stServer.h"
#ifdef _ALMRUN_STSERVER_H_
#include "MerryApp.h"
#include "DlgAddNewCmd.h"
#include "DlgAddNewDir.h"

// ----------------------------------------------------------------------------
// stServer
// ----------------------------------------------------------------------------

stServer::stServer() : wxServer()
{
	__DEBUG_BEGIN("")
    m_connection = NULL;
}

stServer::~stServer()
{
	__DEBUG_BEGIN("")
    Disconnect();

}

void stServer::Disconnect()
{
	__DEBUG_BEGIN("")
    if (m_connection)
        wxDELETE(m_connection);
}

// Accepts a connection from another instance
wxConnectionBase *stServer::OnAcceptConnection(const wxString& topic)
{
	__DEBUG_BEGIN("")
    //if (topic == IPC_TOPIC)
    //{
		m_connection = new stConnection();
        return m_connection;
    //}
    //else
    //    return NULL;
}

/*-----------------------------------------------------------------------
	stConnection
--------------------------------------------------------------------------*/
bool stConnection::OnExecute(const wxString& topic,
                          const void *data,
                           size_t size,
                           wxIPCFormat format)
{
	__DEBUG_BEGIN("")
	wxString cmd((const wchar_t *)data);
	if (topic == IPC_TOPIC)//程序通信
	{
		if (cmd.empty() ||  cmd.Upper() == "SHOW")//默认显示
		{
			::wxGetApp().GetFrame().Show();
			return true;
		}
		cmds.Add(cmd);
	}
//	wxMessageBox(cmd,topic);
    return true;
}

bool stConnection::OnDisconnect()
{
	__DEBUG_BEGIN("")
	if (cmds.Count() == 1)
	{
		wxString cmd = cmds[0];
		if (::wxDirExists(cmd))
		{
			DlgAddNewDir *dlg = new DlgAddNewDir(NULL,wxID_ANY);
			dlg->dirName->SetValue(cmd);
			dlg->dirExclude->SetValue(g_config->GetDefaultDirExclude());
			dlg->dirInclude->SetValue(g_config->GetDefaultDirInclude());
			dlg->dirSub->SetValue(g_config->GetDefaultDirSub());
			dlg->ShowModal();
			dlg->Destroy();
		}
		else
		{
			DlgAddNewCmd *dlg = new DlgAddNewCmd();
			dlg->cmdLine->SetValue(cmd);
			dlg->cmdName->SetValue(wxFileNameFromPath(cmd));
			dlg->ShowModal();
			dlg->Destroy();
		}
	}
	else if (cmds.Count() > 1)
	{
		for(int i=cmds.Count() - 1; i>=0 ;--i)
		{
			wxString cmd = cmds[i];
			//cmd.Replace("\\","/");
			if (::wxDirExists(cmd))
				g_config->AddDir(cmd);
			else if (g_config->AddCmd(cmd,wxFileNameFromPath(cmd)) <= 0)
				ShowErrinfo(ShowCMDErrInfo);;
		}
		wxMessageBox("批量添加完成!");
	}
	::wxGetApp().stServerDisconnect();
    return true;
}
// ----------------------------------------------------------------------------
// stClient
// ----------------------------------------------------------------------------

stClient::stClient() : wxClient()
{
	__DEBUG_BEGIN("")
    m_connection = NULL;
}

bool stClient::Connect(const wxString& sHost, const wxString& sService, const wxString& sTopic)
{
    // suppress the log messages from MakeConnection()
//    wxLogNull nolog;
	__DEBUG_BEGIN("")
    m_connection = (stConnection *)MakeConnection(sHost, sService, sTopic);
    return m_connection    != NULL;
}

wxConnectionBase *stClient::OnMakeConnection()
{
	__DEBUG_BEGIN("")
    return new stConnection;
}

void stClient::Disconnect()
{
	__DEBUG_BEGIN("")
    if (m_connection)
    {
        m_connection->Disconnect();
        wxDELETE(m_connection);
//        wxGetApp().GetFrame()->EnableControls();
//        wxLogMessage(wxT("Client disconnected from server"));
    }
}

stClient::~stClient()
{
	__DEBUG_BEGIN("")
    Disconnect();
}

#endif
