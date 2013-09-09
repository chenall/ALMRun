#include "stServer.h"
#ifdef _ALMRUN_STSERVER_H_
#include "MerryApp.h"

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
	if (topic == IPC_TOPIC)//³ÌÐòÍ¨ÐÅ
	{
		if (cmd == "EXIT")
		{
			cmd_mode = 0;
			return true;
		}

		switch(cmd_mode)
		{
			case IPC_CMD_ADD_CMD:
			case IPC_CMD_ADD_DIR:
				break;
			default:
				cmd.UpperCase();
				if (cmd == "SHOW")
					::wxGetApp().GetFrame().Show();
				else if (cmd == "ADD_CMD")
					cmd_mode = IPC_CMD_ADD_CMD;
				else if (cmd == "ADD_DIR")
					cmd_mode = IPC_CMD_ADD_DIR;
				else
					break;
				return true;
		}
//		return true;
	}
	wxMessageBox(cmd,topic);
    return true;
}

bool stConnection::OnDisconnect()
{
	__DEBUG_BEGIN("")
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
