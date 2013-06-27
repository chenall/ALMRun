#include "cmdListCtrl.h"
#include "MerryCommandManager.h"
#include "cmdmgr.h"
#include "DlgAddNewCmd.h"

BEGIN_EVENT_TABLE(cmdListCtrl, wxListCtrl)
	EVT_MENU(MENU_CMD_DEL,cmdListCtrl::onPopMenu)
	EVT_MENU(MENU_CMD_EDIT,cmdListCtrl::onPopMenu)
END_EVENT_TABLE()
cmdListCtrl::cmdListCtrl(wxWindow *parent, wxWindowID id, const wxPoint& pos,const wxSize& size, long style):
wxListCtrl(parent,id,pos,size,style)
{
	this->InsertColumn(CMDLIST_COL_ID,_T("ÃüÁîID"),0,60);
	this->InsertColumn(CMDLIST_COL_NAME,_T("¿ì½ÝÃüÁî"),wxLIST_AUTOSIZE,150);
	this->InsertColumn(CMDLIST_COL_CMD,_T("ÃüÁîÐÐ"),wxLIST_AUTOSIZE,250);
	this->InsertColumn(CMDLIST_COL_KEY,_T("ÈÈ¼ü"),wxLIST_AUTOSIZE,100);
	this->InsertColumn(CMDLIST_COL_DESC,_T("±¸×¢"),wxLIST_AUTOSIZE,100);
	if (g_config && g_commands)
	{
		wxString cmds;
		long id = 0;
		long index = 0;
		const wxString oldPath = g_config->conf->GetPath();
		wxFileConfig *conf = g_config->conf;
		conf->SetPath("/cmds");
		for(bool test = conf->GetFirstGroup(cmds,id); test ; conf->SetPath("../"),test = conf->GetNextGroup(cmds,id))
		{
			if (!cmds.IsNumber())
				continue;
			conf->SetPath(cmds);
			cmds.ToLong(&index);
			index = this->InsertItem(index,cmds);
			this->SetItem(index, CMDLIST_COL_NAME ,conf->Read("name"));
			this->SetItem(index, CMDLIST_COL_CMD ,conf->Read("cmd"));
			this->SetItem(index, CMDLIST_COL_KEY ,conf->Read("key"));
			this->SetItem(index, CMDLIST_COL_DESC ,conf->Read("desc"));
		}
	}
//	this->Connect(wxEVT_COMMAND_LIST_ITEM_SELECTED,wxObjectEventFunction(&cmdListCtrl::OnSelected));
//	this->Connect(wxEVT_COMMAND_LIST_COL_CLICK,wxObjectEventFunction(&cmdListCtrl::OnColClick));
	this->Connect(wxEVT_COMMAND_LIST_ITEM_RIGHT_CLICK,wxObjectEventFunction(&cmdListCtrl::onRightClick));
//	this->Connect(wxEVT_COMMAND_LIST_DELETE_ITEM,wxObjectEventFunction(&cmdListCtrl::onDelete));
}

cmdListCtrl::~cmdListCtrl()
{
	__DEBUG_BEGIN("")
	this->Disconnect(wxEVT_COMMAND_LIST_ITEM_SELECTED);
	__DEBUG_END("")
}

//wxString cmdListCtrl::OnGetItemText(long item, long column) const
//{
//	if (!g_commands)
//		return wxEmptyString;
//	const MerryCommand* command = g_commands->GetCommand(item+1);
//	wxString text;
//	switch(column)
//	{
//		case 0:
//			return wxString::Format("%d",command->GetFlags()>>4);
//			break;
//		case 1:
//			return command->GetCommandName();
//			break;
//		case 2:
//			return command->GetCommandDesc();
//			break;
//		case 3:
//			return command->GetCommandLine();
//			break;
//		default:
//			return wxEmptyString;
//	}
//}

//wxListItemAttr* cmdListCtrl::OnGetItemAttr(long item) const
//{
//    ;
//}

//bool cmdListCtrl::OnSortColumn(int /*col*/, bool /*descend*/)
//{
// return false;
// 
// }
//
//void cmdListCtrl::OnColClick(wxListEvent& e)
//{
//	wxMessageBox(wxString::Format("1:%d,%d,%s",e.GetId(),e.GetIndex(),e.GetText()));
//	e.Skip();
//}

bool cmdListCtrl::onDelete(const wxString& item)
{
	if (g_config && item.IsNumber())
	{
		long id;
		item.ToLong(&id);
		if (id >= 0 && g_config->DeleteCmd(id))
			return true;
	}
	return false;
}

void cmdListCtrl::onRightClick(wxListEvent& e)
{
	wxMenu *menu = new wxMenu;
	menu->Append(MENU_CMD_EDIT,"±à¼­(&E)");
	menu->Append(MENU_CMD_DEL,"É¾³ý(&D)");
	PopupMenu(menu);
	delete menu;
}

void cmdListCtrl::RunMenu(const int id,cmdListCtrl* ctrl)
{
	int item = ctrl->GetNextItem(-1,wxLIST_NEXT_ALL,wxLIST_STATE_FOCUSED);
	switch(id)
	{
		case MENU_CMD_ADD:
			{
				DlgAddNewCmd* dlg=new DlgAddNewCmd(NULL);
				if (dlg->ShowModal() == wxID_OK)
				{
					long index = ctrl->InsertItem(dlg->cmdID,dlg->GetCmdID());
					ctrl->SetItem(index,CMDLIST_COL_NAME,dlg->cmdName->GetValue());
					ctrl->SetItem(index,CMDLIST_COL_KEY,dlg->cmdKey->GetValue());
					ctrl->SetItem(index,CMDLIST_COL_DESC,dlg->cmdDesc->GetValue());
					ctrl->SetItem(index,CMDLIST_COL_CMD,dlg->cmdLine->GetValue());
				}
				dlg->Destroy();
			}
			break;
		case MENU_CMD_EDIT:
			if (item == -1)
				break;
			{
				DlgAddNewCmd* dlg = new DlgAddNewCmd(MENU_CMD_EDIT);
//				dlg->flags = MENU_CMD_EDIT;
				dlg->SetCmdID(ctrl->GetItemText(item,CMDLIST_COL_ID));
				dlg->cmdName->SetValue(ctrl->GetItemText(item,CMDLIST_COL_NAME));
				dlg->cmdDesc->SetValue(ctrl->GetItemText(item,CMDLIST_COL_DESC));
				dlg->cmdKey->SetValue(ctrl->GetItemText(item,CMDLIST_COL_KEY));
				dlg->cmdLine->SetValue(ctrl->GetItemText(item,CMDLIST_COL_CMD));
				if (dlg->ShowModal() == wxID_OK)
				{
					ctrl->SetItem(item,CMDLIST_COL_NAME,dlg->cmdName->GetValue());
					ctrl->SetItem(item,CMDLIST_COL_KEY,dlg->cmdKey->GetValue());
					ctrl->SetItem(item,CMDLIST_COL_DESC,dlg->cmdDesc->GetValue());
					ctrl->SetItem(item,CMDLIST_COL_CMD,dlg->cmdLine->GetValue());
				}	
				dlg->Destroy();
			}
			break;
		case MENU_CMD_DEL:
			if (item == -1)
				break;
			if (ctrl->GetSelectedItemCount() == 1)
			{
				if (wxMessageBox(wxString::Format("ÕæµÄÒªÉ¾³ý¸ÃÃüÁîÂð?\nÃû³Æ:%s\nÃüÁîÐÐ:%s",ctrl->GetItemText(item,1),ctrl->GetItemText(item,3))	,"É¾³ýÈ·ÈÏ",wxYES_NO | wxICON_WARNING) != wxYES)
					break;
				if (cmdListCtrl::onDelete(ctrl->GetItemText(item)))
					ctrl->DeleteItem(item);
				break;
			}
			if (wxMessageBox(wxString::Format("ÕæµÄÒªÉ¾³ýËùÑ¡ÔñµÄ[%d]¸öÃüÁîÂð?",ctrl->GetSelectedItemCount()),"É¾³ýÈ·ÈÏ",wxYES_NO | wxICON_WARNING) == wxYES)
			{
				wxArrayLong sel;
				for(item = -1;;)
				{
					item = ctrl->GetNextItem(item,wxLIST_NEXT_ALL,wxLIST_STATE_SELECTED);
					if (item == -1)
						break;
					sel.Add(item);
				}
				for(item = sel.size() - 1;item >= 0;--item)
				{
					if (cmdListCtrl::onDelete(ctrl->GetItemText(sel[item])))
						ctrl->DeleteItem(sel[item]);
				}
				sel.Clear();
			}
			break;
		case ID_TOOL_CHECK:
			wxMessageBox("ÃüÁî¼ì²â,ÉÐÎ´Íê³É");
			break;
	}
//	ctrl->Refresh();
}

void cmdListCtrl::onPopMenu(wxCommandEvent& e)
{
	cmdListCtrl::RunMenu(e.GetId(),this);
}


void cmdListCtrl::OnSelected(wxListEvent& e)
{
	wxMessageBox(wxString::Format("%d,%d,%s",e.GetId(),e.GetIndex(),e.GetText()));
	e.Skip();
}
