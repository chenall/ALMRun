#ifndef _MERRY_APP_H_
#define _MERRY_APP_H_

#include "MerryFrame.h"
#include "ALMRunConfig.h"
#include "stServer.h"
#include <wx/snglinst.h>
class MerryApp : public wxApp
{
public:
	virtual int OnExit();
    virtual bool OnInit();
	void NewFrame();
	void stServerDisconnect();
	void Execute_IPC_CMD(wxConnectionBase* conn);
	MerryFrame& GetFrame();
	void EvtActive(wxActivateEvent& e);
private:
	MerryFrame* m_frame;
	stServer *m_server;
	wxSingleInstanceChecker *m_checker;
#if _DEBUG_LOG
	FILE *m_pLogFile;
#endif
};

DECLARE_APP(MerryApp)

#endif