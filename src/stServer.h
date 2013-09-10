#ifndef _ALMRUN_STSERVER_H_
#define _ALMRUN_STSERVER_H_
#include "MerryWx.h"
#include "wx/ipc.h"
#define IPC_SERVICE "36684"
// the hostname
#define IPC_HOST "localhost"

// the IPC topic
#define IPC_TOPIC "ALMRun"
#define IPC_CMD_SHOW 1
#define IPC_CMD_ADD_CMD 2
#define IPC_CMD_ADD_DIR 3

// Connection class, for use by both communicating instances

class stConnection : public wxConnection
{
public:
    stConnection() {cmd_mode = 0;}
    ~stConnection() {}

    virtual bool OnExecute(const wxString& topic,
                            const void *data,
                           size_t size,
                           wxIPCFormat format);
	virtual bool OnDisconnect();
private:
	int cmd_mode;
	wxArrayString cmds;
};

// Server class, for listening to connection requests

class stServer: public wxServer
{
public:
    stServer();
    virtual ~stServer();

    void Disconnect();
    bool IsConnected() { return m_connection != NULL; }
    stConnection *GetConnection() { return m_connection; }
    virtual wxConnectionBase *OnAcceptConnection(const wxString& topic);

protected:
    stConnection *m_connection;
};


// Client class, to be used by subsequent instances in OnInit

class stClient: public wxClient
{
public:
    stClient();
    ~stClient();
    bool Connect(const wxString& sHost, const wxString& sService, const wxString& sTopic);
    void Disconnect();
    wxConnectionBase *OnMakeConnection();
    bool IsConnected() { return m_connection != NULL; };
    stConnection *GetConnection() { return m_connection; };

protected:
    stConnection  *m_connection;
};



#endif