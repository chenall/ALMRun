#include "MerryTaskBarIcon.h"
#include "MerryApp.h"
#include "MerryInformationDialog.h"

BEGIN_EVENT_TABLE(MerryTaskBarIcon, wxTaskBarIcon)
	EVT_MENU(MENU_ITEM_OPEN, MerryTaskBarIcon::onPopMenu)
	EVT_MENU(MENU_ITEM_OPEN_CONFIG, MerryTaskBarIcon::onPopMenu)
	EVT_MENU(MENU_ITEM_CONFIG, MerryTaskBarIcon::onPopMenu)
	EVT_MENU(MENU_ITEM_ABOUT, MerryTaskBarIcon::onPopMenu)
	EVT_MENU(MENU_ITEM_EXIT, MerryTaskBarIcon::onPopMenu)
	EVT_TASKBAR_LEFT_DCLICK(MerryTaskBarIcon::OnLeftButtonDClickEvent)
END_EVENT_TABLE()

wxMenu* MerryTaskBarIcon::CreatePopupMenu()
{
	wxMenu* menu = new wxMenu;

	menu->Append(MENU_ITEM_OPEN, wxT("显示界面(&O)"));
	menu->Append(MENU_ITEM_OPEN_CONFIG, wxT("显示配置(&S)"));
	menu->Append(MENU_ITEM_CONFIG, wxT("刷新配置(R)"));
	menu->Append(MENU_ITEM_ABOUT, wxT("关于ALMRun(&A)"));
	menu->Append(MENU_ITEM_EXIT, wxT("退出(&X)"));

	return menu;
}

void MerryTaskBarIcon::onPopMenu(wxCommandEvent& e)
{
	switch(e.GetId())
	{
		case MENU_ITEM_ABOUT:
			new MerryInformationDialog(wxT("关于 ALMRun"),wxString::Format(wxT("version 1.1.2 --- http://chenall.net\r\n修改自(merry SVN R:98 http://name5566.com)\r\n编译时间:%s %s"),__DATE__,__TIME__));
			break;
		case MENU_ITEM_OPEN:
			::wxGetApp().GetFrame().Show();
			break;
		case MENU_ITEM_OPEN_CONFIG:
			::wxGetApp().GetFrame().OpenConfigDir();
			break;
		case MENU_ITEM_CONFIG:
			::wxGetApp().GetFrame().NewConfig();
			break;
		case MENU_ITEM_EXIT:
			::wxGetApp().GetFrame().Close();
			break;
	}
}

void MerryTaskBarIcon::OnLeftButtonDClickEvent(wxTaskBarIconEvent& e)
{
	::wxGetApp().GetFrame().Show();
	e.Skip();
}
