#ifndef _MERRY_FRAME_H_
#define _MERRY_FRAME_H_

#include "MerryWx.h"

class MerryMainPanel;
class MerryListBoxPanel;
class MerryTaskBarIcon;
class MerryFrame : public wxFrame
{
public:
	MerryFrame();
	~MerryFrame();
	void OnInit();
	void OnClose();
	void OpenConfigDir();
	void NewConfig();
	void setTitle(const wxString& title);
	void ShowTrayIcon(const bool show);

	MerryListBoxPanel* GetListBoxPanel();

private:
	void onContextMenu(wxContextMenuEvent& e);
	void OnCloseEvent(wxCloseEvent& e);
	void OnActivateEvent(wxActivateEvent& e);
	void OnShowEvent(wxShowEvent& e);

private:
	void CentreOnce();

private:
	MerryMainPanel* m_mainPanel;
	MerryListBoxPanel* m_listBoxPanel;
	MerryTaskBarIcon* m_taskBarIcon;

	bool m_isCentred;

private:
	DECLARE_EVENT_TABLE()
};

#endif
