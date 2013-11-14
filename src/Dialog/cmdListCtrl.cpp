#include "cmdListCtrl.h"
#ifdef _ALMRUN_CONFIG_H_
#include "MerryCommandManager.h"
#include "cmdmgr.h"
#include "DlgAddNewCmd.h"

#ifndef _DISABLE_DND_
	#include <wx/dnd.h>
	class DnDialogFile : public wxFileDropTarget
	{
	public:
		DnDialogFile(cmdListCtrl *pOwner) { m_pOwner = pOwner; }
 
		virtual bool OnDropFiles(wxCoord x, wxCoord y,
								 const wxArrayString& filenames);
	private:
		//对话框类，成员TextCtrlPath保存文件路径
		cmdListCtrl *m_pOwner;
 
	};
	bool DnDialogFile::OnDropFiles(wxCoord, wxCoord, const wxArrayString& filenames)
	{
		//只有一个文件时弹出添加命令窗口
		if (filenames.Count() == 1)
		{
			wxString cmd = filenames[0];
			cmd.Replace('\\','/');
			DlgAddNewCmd* dlg=new DlgAddNewCmd(m_pOwner->GetParent());
			dlg->cmdLine->SetValue(cmd);
			dlg->cmdName->SetValue(wxFileNameFromPath(cmd));
			cmd.Clear();
			if (dlg->ShowModal() == wxID_OK)
				m_pOwner->ReLoadCmds();
			dlg->Destroy();
			return true;
		}
		//多个文件时自动批量添加;
		for(int i = filenames.Count() - 1 ;i>=0;--i)
		{
			wxString cmd = filenames[i];
			cmd.Replace("\\","/");
			g_config->AddCmd(cmd,wxFileNameFromPath(cmd));
		}
		m_pOwner->ReLoadCmds();
		return true;
		//多个文件时自动批量添加;
	}
#endif

BEGIN_EVENT_TABLE(cmdListCtrl, wxListCtrl)
	EVT_LEFT_DCLICK(cmdListCtrl::onDclick)
	EVT_MENU(MENU_CMD_DEL,cmdListCtrl::onPopMenu)
	EVT_MENU(MENU_CMD_EDIT,cmdListCtrl::onPopMenu)
END_EVENT_TABLE()
cmdListCtrl::cmdListCtrl(wxWindow *parent, wxWindowID id, const wxPoint& pos,const wxSize& size, long style):
wxListCtrl(parent,id,pos,size,style)
{
	this->InsertColumn(CMDLIST_COL_NAME,_T("快捷命令"),wxLIST_AUTOSIZE,150);
	this->InsertColumn(CMDLIST_COL_CMD,_T("命令行"),wxLIST_AUTOSIZE,250);
	this->InsertColumn(CMDLIST_COL_KEY,_T("热键"),wxLIST_AUTOSIZE,100);
	this->InsertColumn(CMDLIST_COL_DESC,_T("备注"),wxLIST_AUTOSIZE,100);
	this->InsertColumn(CMDLIST_COL_ID,_T("ID"),0,30);
	this->ReLoadCmds();
//	this->Connect(wxEVT_COMMAND_LIST_ITEM_SELECTED,wxObjectEventFunction(&cmdListCtrl::OnSelected));
//	this->Connect(wxEVT_COMMAND_LIST_COL_CLICK,wxObjectEventFunction(&cmdListCtrl::OnColClick));
	this->Connect(wxEVT_COMMAND_LIST_ITEM_RIGHT_CLICK,wxObjectEventFunction(&cmdListCtrl::onRightClick));
#ifndef _DISABLE_DND_
	this->SetDropTarget(new DnDialogFile(this));
#endif
//	this->Connect(wxEVT_COMMAND_LIST_DELETE_ITEM,wxObjectEventFunction(&cmdListCtrl::onDelete));
}

void cmdListCtrl::ReLoadCmds()
{
	if (g_config && g_commands)
	{
		wxString cmds;
		long id = 0;
		long index = 0;
		const wxString oldPath = g_config->conf->GetPath();
		wxFileConfig *conf = g_config->conf;
		conf->SetPath("/cmds");
		this->DeleteAllItems();
		for(bool test = conf->GetFirstGroup(cmds,id); test ; conf->SetPath("../"),test = conf->GetNextGroup(cmds,id))
		{
			if (!cmds.IsNumber())
				continue;
			conf->SetPath(cmds);
			cmds.ToLong(&index);
			index = this->InsertItem(index,cmds);
			this->SetItem(index, CMDLIST_COL_ID,cmds);
			this->SetItem(index, CMDLIST_COL_NAME ,conf->Read("name"));
			this->SetItem(index, CMDLIST_COL_CMD ,conf->Read("cmd"));
			this->SetItem(index, CMDLIST_COL_KEY ,conf->Read("key"));
			this->SetItem(index, CMDLIST_COL_DESC ,conf->Read("desc"));
		}
	}
}

void cmdListCtrl::onDclick(wxMouseEvent& e)
{
	this->RunMenu(ID_TOOL_EDIT,this);
}

cmdListCtrl::~cmdListCtrl()
{
	__DEBUG_BEGIN("")
	this->Disconnect(wxEVT_COMMAND_LIST_ITEM_SELECTED);
#ifndef _DISABLE_DND_
	this->SetDropTarget(NULL);
#endif
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
//			return command->GetCmd();
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
	menu->Append(ID_TOOL_EDIT,"编辑(&E)");
	menu->Append(ID_TOOL_DELETE,"删除(&D)");
	PopupMenu(menu);
	delete menu;
}

void cmdListCtrl::RunMenu(const int id,cmdListCtrl* ctrl)
{
	int item = ctrl->GetNextItem(-1,wxLIST_NEXT_ALL,wxLIST_STATE_FOCUSED);
	switch(id)
	{
		case ID_TOOL_ADD:
			{
				DlgAddNewCmd* dlg=new DlgAddNewCmd(NULL);
				if (dlg->ShowModal() == wxID_OK)
					ctrl->ReLoadCmds();
				dlg->Destroy();
			}
			break;
		case ID_TOOL_EDIT:
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
					ctrl->ReLoadCmds();
				dlg->Destroy();
			}
			break;
		case ID_TOOL_DELETE:
			if (item == -1)
				break;
			if (ctrl->GetSelectedItemCount() == 1)
			{
				if (wxMessageBox(wxString::Format("真的要删除该命令吗?\n名称:%s\n命令行:%s",ctrl->GetItemText(item,CMDLIST_COL_NAME),ctrl->GetItemText(item,CMDLIST_COL_CMD))	,"删除确认",wxYES_NO | wxICON_WARNING) != wxYES)
					break;
				if (cmdListCtrl::onDelete(ctrl->GetItemText(item,CMDLIST_COL_ID)))
					ctrl->DeleteItem(item);
				break;
			}
			if (wxMessageBox(wxString::Format("真的要删除所选择的[%d]个命令吗?",ctrl->GetSelectedItemCount()),"删除确认",wxYES_NO | wxICON_WARNING) == wxYES)
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
					if (cmdListCtrl::onDelete(ctrl->GetItemText(sel[item],CMDLIST_COL_ID)))
						ctrl->DeleteItem(sel[item]);
				}
				sel.Clear();
			}
			break;
		case ID_TOOL_CHECK:
			wxMessageBox("命令检测,尚未完成");
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
#endif