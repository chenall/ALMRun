#include "cmdListCtrl.h"
#ifdef _ALMRUN_CONFIG_H_
#include "MerryCommandManager.h"
#include "cmdmgr.h"
#include "DlgAddNewCmd.h"
#include "DlgAddNewDir.h"

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
			//cmd.Replace('\\','/');
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
			//cmd.Replace("\\","/");
			if (g_config->AddCmd(cmd,wxFileNameFromPath(cmd)) <= 0)
				ShowErrinfo(ShowCMDErrInfo);
		}
		m_pOwner->ReLoadCmds();
		return true;
		//多个文件时自动批量添加;
	}
#endif

int wxCALLBACK
cmdListCompare(wxIntPtr item1, wxIntPtr item2, wxIntPtr sortData)
{
	ListSortInfo *SortInfo = (ListSortInfo *)sortData;
	wxListCtrl *ListCtrl= SortInfo->ListCtrl;
	int col = SortInfo->Column;
	long index1 = ListCtrl->FindItem(-1,item1);
	long index2 = ListCtrl->FindItem(-1,item2);
	long cmdid1,cmdid2;
	long cmp = 0;
	wxString str1;
	wxString str2;
	wxString filter(SortInfo->filter);

	if (!filter.empty())
	{
		size_t s1,s2;
		int _col = col;
		int _col_2;
		if (_col != CMDLIST_COL_NAME && _col != CMDLIST_COL_CMD)
			_col = CMDLIST_COL_NAME;

		_col_2 = (_col == CMDLIST_COL_NAME) ? CMDLIST_COL_CMD : CMDLIST_COL_NAME;

		str1 = ListCtrl->GetItemText(index1,_col);
		str2 = ListCtrl->GetItemText(index2,_col);
		if (str1.IsEmpty())
			str1 = ListCtrl->GetItemText(index1,_col_2);
		if (str2.IsEmpty())
			str2 = ListCtrl->GetItemText(index2,_col_2);
		s1 = str1.Upper().find(filter);
		s2 = str2.Upper().find(filter);
		if (s1 != wxNOT_FOUND)
		{
			if (s2 == wxNOT_FOUND)
				return -1;
			else
				return s1 - s2;
		}
		else if (s2 != wxNOT_FOUND)
			return 1;
	}

	str1 = ListCtrl->GetItemText(index1,CMDLIST_COL_ID);
	str2 = ListCtrl->GetItemText(index2,CMDLIST_COL_ID);
	str1.ToLong(&cmdid1);
	str2.ToLong(&cmdid2);

	if (col == CMDLIST_COL_ID)//ID比较
	{
		cmp = cmdid1 - cmdid2;
	}
	else
	{//字符串比较
		str1 = ListCtrl->GetItemText(index1,col);
		str2 = ListCtrl->GetItemText(index2,col);
		cmp = str1.CmpNoCase(str2);
	}

	if (cmp == 0)
		return 0;

	if (cmdid1 < 0 && cmdid2 >=0)
		return 1;
	if (cmdid2 < 0 && cmdid1 >=0)
		return -1;

	if (SortInfo->SortAscending)
		return cmp;

	return cmp>0?-1:1;
}

BEGIN_EVENT_TABLE(cmdListCtrl, wxListCtrl)
	EVT_LEFT_DCLICK(cmdListCtrl::onDclick)
	EVT_MENU(MENU_CMD_DEL,cmdListCtrl::onPopMenu)
	EVT_MENU(MENU_CMD_EDIT,cmdListCtrl::onPopMenu)
END_EVENT_TABLE()
cmdListCtrl::cmdListCtrl(wxWindow *parent, wxWindowID id, const wxPoint& pos,const wxSize& size, long style):
wxListCtrl(parent,id,pos,size,style)
{
	SortInfo.ListCtrl = this;
	m_imageListSmall = new wxImageList(16, 16, true);
	m_imageListSmall->Add( wxIcon(wxT("desc"), wxBITMAP_TYPE_ICO_RESOURCE) );
	m_imageListSmall->Add( wxIcon(wxT("asc"), wxBITMAP_TYPE_ICO_RESOURCE) );
	this->SetImageList(m_imageListSmall, wxIMAGE_LIST_SMALL);
	this->InsertColumn(CMDLIST_COL_NAME,_T("快捷命令"),wxLIST_AUTOSIZE,150);
	this->InsertColumn(CMDLIST_COL_CMD,_T("命令行"),wxLIST_AUTOSIZE,250);
	this->InsertColumn(CMDLIST_COL_KEY,_T("热键/包含"),wxLIST_AUTOSIZE,100);
	this->InsertColumn(CMDLIST_COL_DESC,_T("备注/排除"),wxLIST_AUTOSIZE,100);
	this->InsertColumn(CMDLIST_COL_ID,_T("ID"),0,30);
	this->InsertColumn(CMDLIST_COL_WORKDIR,_T("工作目录"),0,0);
	this->ReLoadCmds();
//	this->Connect(wxEVT_COMMAND_LIST_ITEM_SELECTED,wxObjectEventFunction(&cmdListCtrl::OnSelected));
//	this->Connect(wxEVT_COMMAND_LIST_COL_CLICK,wxObjectEventFunction(&cmdListCtrl::OnColClick));
//	this->Connect(wxEVT_COMMAND_LIST_ITEM_RIGHT_CLICK,wxObjectEventFunction(&cmdListCtrl::onRightClick));
	this->Bind(wxEVT_COMMAND_LIST_ITEM_RIGHT_CLICK,&cmdListCtrl::onRightClick,this);
	this->Bind(wxEVT_LIST_KEY_DOWN,&cmdListCtrl::onKeyDown,this);
	this->Bind(wxEVT_LIST_COL_CLICK,&cmdListCtrl::OnColClick,this);
#ifndef _DISABLE_DND_
	this->SetDropTarget(new DnDialogFile(this));
#endif
//	this->SetImageList(m_imageListSort, wxIMAGE_LIST_SMALL);
//	this->Connect(wxEVT_COMMAND_LIST_DELETE_ITEM,wxObjectEventFunction(&cmdListCtrl::onDelete));
}

void cmdListCtrl::ReLoadCmds()
{
	if (g_config && g_commands)
	{
		wxString cmds;
		long id = 0;
		long index = -1;
		wxFileConfig *conf = g_config->conf;
		const wxString oldPath = conf->GetPath();
		conf->SetPath("/cmds");
		this->DeleteAllItems();
		for(bool test = conf->GetFirstGroup(cmds,id); test ; conf->SetPath("../"),test = conf->GetNextGroup(cmds,id))
		{
			if (!cmds.IsNumber())
				continue;
			conf->SetPath(cmds);
			index = this->InsertItem(++index,cmds,-1);
			this->SetItemData(index,index);
			this->SetItem(index, CMDLIST_COL_ID,cmds);
			this->SetItem(index, CMDLIST_COL_NAME ,conf->Read("name"));
			this->SetItem(index, CMDLIST_COL_CMD ,conf->Read("cmd"));
			this->SetItem(index, CMDLIST_COL_KEY ,conf->Read("key"));
			this->SetItem(index, CMDLIST_COL_DESC ,conf->Read("desc"));
			this->SetItem(index, CMDLIST_COL_WORKDIR,conf->Read("workdir"));
		}
		conf->SetPath("/dirs");
		id = 0;
		for(bool test = conf->GetFirstGroup(cmds,id); test ; conf->SetPath("../"),test = conf->GetNextGroup(cmds,id))
		{
			if (!cmds.IsNumber())
				continue;
			conf->SetPath(cmds);
			long cid;
			cmds.ToLong(&cid);
			cmds.sprintf("%d",~cid);
			index = this->InsertItem(++index,"@自动扫描目录@",-1);
			this->SetItemData(index,index);
			this->SetItem(index, DIRLIST_COL_ID,cmds);
			this->SetItem(index, DIRLIST_COL_PATH ,conf->Read("path"));
			this->SetItem(index, DIRLIST_COL_INCLUDE ,conf->Read("include"));
			this->SetItem(index, DIRLIST_COL_EXCLUDE ,conf->Read("exclude"));
			this->SetItem(index, DIRLIST_COL_SUB ,conf->Read("sub"));
			this->SetItemTextColour(index,wxColour(0xA20730));
		}
		conf->SetPath(oldPath);
	}
}

void cmdListCtrl::SortFilter(const wxString& s)
{
	SortInfo.filter = s;
	this->SortItems(cmdListCompare,(long)&SortInfo);
}

void cmdListCtrl::OnColClick(wxListEvent& e)
{
	int col = e.GetColumn();

    // set or unset image
    static bool x[COL_MAX];
	wxListItem itemCol;
	itemCol.SetMask(wxLIST_MASK_IMAGE);
	itemCol.SetImage(-1);
	SetColumn(SortInfo.Column,itemCol);

    x[col] = !x[col];

	SortInfo.Column = col;
	SortInfo.SortAscending = x[col];
	itemCol.SetImage(SortInfo.SortAscending);
	SetColumn(SortInfo.Column,itemCol);
	this->SortItems(cmdListCompare,(long)&SortInfo);
}

void cmdListCtrl::onKeyDown(wxListEvent& e)
{
	int keyCode = e.GetKeyCode();
	int menuid = 0;
	switch(keyCode)
	{
		case WXK_INSERT:
			menuid = ID_TOOL_ADD;
			break;
		case WXK_DELETE:
			menuid = ID_TOOL_DELETE;
			break;
		case WXK_F2:
			menuid = ID_TOOL_EDIT;
			break;
		case WXK_ESCAPE:
			return;
		default:
			e.Skip();
			return;
	}
	if (menuid)
		this->RunMenu(menuid,this);
//	wxMessageBox(wxString::Format("%d,%d,%s",e.GetKeyCode(),e.GetIndex(),e.GetItem().GetText()));
}

void cmdListCtrl::onDclick(wxMouseEvent& e)
{
	this->RunMenu(ID_TOOL_EDIT,this);
}

cmdListCtrl::~cmdListCtrl()
{
	__DEBUG_BEGIN("")
/*
	this->Disconnect(wxEVT_COMMAND_LIST_ITEM_RIGHT_CLICK);
	this->Disconnect(wxEVT_COMMAND_LIST_ITEM_SELECTED);
	this->Disconnect(wxEVT_LIST_KEY_DOWN);
*/
	delete m_imageListSmall;
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
		if (id >= 0)
			return g_config->DeleteCmd(id);
		return g_config->DeleteDir(~id);		
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
	long cmdID = 0;
	if (item >= 0)
		ctrl->GetItemText(item,CMDLIST_COL_ID).ToLong(&cmdID);

	switch(id)
	{
		case ID_TOOL_ADD:
			{
				DlgAddNewCmd* dlg=new DlgAddNewCmd();
				if (dlg->ShowModal() == wxID_OK)
					ctrl->ReLoadCmds();
				dlg->Destroy();
			}
			break;
		case ID_TOOL_ADD_FOLDER:
			{
				DlgAddNewDir* dlg=new DlgAddNewDir(NULL);
				if (dlg->ShowModal() == wxID_OK)
					ctrl->ReLoadCmds();
				dlg->Destroy();
			}
			break;
		case ID_TOOL_EDIT:
			if (item == -1)//没有当前激活条目
				break;
			if (cmdID >= 0)//cmdID >=0 命令
			{
				DlgAddNewCmd* dlg = new DlgAddNewCmd(ctrl->GetItemText(item,CMDLIST_COL_ID));
	//			dlg->flags = MENU_CMD_EDIT;
				//dlg->SetCmdID(ctrl->GetItemText(item,CMDLIST_COL_ID));
				//dlg->cmdName->SetValue(ctrl->GetItemText(item,CMDLIST_COL_NAME));
				//dlg->cmdDesc->SetValue(ctrl->GetItemText(item,CMDLIST_COL_DESC));
				//dlg->cmdKey->SetValue(ctrl->GetItemText(item,CMDLIST_COL_KEY));
				//dlg->cmdLine->SetValue(ctrl->GetItemText(item,CMDLIST_COL_CMD));
				//dlg->cmdPath->SetValue(ctrl->GetItemText(item,CMDLIST_COL_WORKDIR));
				if (dlg->ShowModal() == wxID_OK)
					ctrl->ReLoadCmds();
				dlg->Destroy();
			}
			else
			{
				DlgAddNewDir* dlg = new DlgAddNewDir(NULL);
				dlg->DirId = ~cmdID;
				dlg->SetMode(ADDDIR_FLAG_EDIT);
				dlg->dirInclude->SetValue(ctrl->GetItemText(item,DIRLIST_COL_INCLUDE));
				dlg->dirExclude->SetValue(ctrl->GetItemText(item,DIRLIST_COL_EXCLUDE));
				wxString name = ctrl->GetItemText(item,DIRLIST_COL_PATH);
				name.Replace("|","\n");
				dlg->dirName->SetValue(name);
				dlg->dirSub->SetValue(ctrl->GetItemText(item,DIRLIST_COL_SUB));
				if (dlg->ShowModal() == wxID_OK)
					ctrl->ReLoadCmds();
				dlg->Destroy();
			}
			break;
		case ID_TOOL_DELETE:
			if (ctrl->GetSelectedItemCount() == 0)
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
		case ID_TOOL_IMPORT:
			{
				wxString selFile = wxFileSelector("从ALTRun配置文件ShortCutList.txt导入命令",wxEmptyString,"ShortCutList.txt",wxEmptyString,"ShortCutList.txt",0,NULL);
				if (selFile.empty())
					return;
				if (importCMD(selFile)>0)
					ctrl->ReLoadCmds();
			}
			break;
		case ID_TOOL_CHECK:
			{
				if (MessageBox(NULL,_T("校验会检测无效命令（有可能没有办法全部检测出来），并用标记这些条目，你可以直接删除或修改，如果命令比较多可能需要比较长的时间，继续吗？"),_T("提示"),MB_YESNO|MB_TOPMOST|MB_ICONQUESTION) != IDYES)
					break;
				size_t count = ctrl->GetItemCount();
				size_t n = 0;
				for(size_t i = 0;i<count;++i)
				{
					long id;
					ctrl->GetItemText(i,CMDLIST_COL_ID).ToLong(&id);
					if (id < 0 )//ID < 0 是自动扫描目录不检测
						break;
					wxString test(ParseCmd(ctrl->GetItemText(i,CMDLIST_COL_CMD)));
					long state = test.empty()?wxLIST_STATE_SELECTED|wxLIST_STATE_FOCUSED:0;
					ctrl->SetItemState(i,state,wxLIST_STATE_SELECTED|wxLIST_STATE_FOCUSED);
					if (state)
					{
						ctrl->SetItemBackgroundColour(i,wxColour(0x6300f8));
						++n;
					}
				}
				if (n)
					wxMessageBox(wxString::Format("列表中有%d个条目[已选定]的命令找不到，可能是无效命令，请检查或直接删除",n));
				else
					wxMessageBox("校验完成，没有发现无效命令");
			}
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