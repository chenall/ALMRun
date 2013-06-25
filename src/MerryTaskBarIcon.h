#ifndef _MERRY_TASK_BAR_ICON_H_
#define _MERRY_TASK_BAR_ICON_H_

#include "MerryWx.h"
#include <wx/taskbar.h>

class MerryTaskBarIcon : public wxTaskBarIcon
{
public:
	~MerryTaskBarIcon();
	void MerryTaskBarIcon::onPopMenu(wxCommandEvent& e);
private:
	void OnLeftButtonDClickEvent(wxTaskBarIconEvent& e);

private:
	virtual wxMenu* CreatePopupMenu();

private:
	DECLARE_EVENT_TABLE()
};

#endif
