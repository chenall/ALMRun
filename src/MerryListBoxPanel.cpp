#include "MerryListBoxPanel.h"
#include "DlgAddNewCmd.h"

BEGIN_EVENT_TABLE(MerryListBoxPanel, wxPanel)
	EVT_MOUSE_EVENTS(MerryListBoxPanel::OnMouseEvent)
	EVT_PAINT(MerryListBoxPanel::OnPaintEvent)
	EVT_CONTEXT_MENU(MerryListBoxPanel::onContextMenu)
	EVT_MENU_RANGE(MENU_CMD_FIRST,MENU_CMD_LAST,onPopMenu)
END_EVENT_TABLE()

MerryListBoxPanel::MerryListBoxPanel(wxWindow* parent):
	wxPanel(parent, wxID_ANY,
		wxPoint(0, parent->GetClientSize().y),
		wxSize(parent->GetClientSize().x, 0))
{
	wxString listpicture(skin->get(LISTPICTURE));
	wxFont font = this->GetFont();
	show_title = (skin->get(TITLE_ENABLED) == 2);

	m_topCommandIndex = -1;
	m_selectionCommandIndex = -1;
	m_isPopup = false;

//重新设置列表框位置信息
	wxPoint pos(skin->get(LIST_BOX_LEFT),skin->get(LIST_BOX_TOP));
	if (pos.y == 0)
		pos.y = parent->GetClientSize().y;
	this->SetPosition(pos);

//设置列表框宽度
	if (skin->get(LIST_BOX_WIDTH))
		this->SetSize(skin->get(LIST_BOX_WIDTH),0);

	if (!listpicture.empty())
	{
		bool isOk = m_listBackground.LoadFile(skin->get(LISTPICTURE), wxBITMAP_TYPE_ANY);
		assert(isOk);
	}

	this->SetOwnForegroundColour(skin->get(LIST_TEXT_COLOR));
	this->SetOwnBackgroundColour(skin->get(TEXT_BACKCOLOR));

	if (skin->get(LIST_FONT_SIZE))
	{
		font.SetPointSize(skin->get(LIST_FONT_SIZE));
		this->SetOwnFont(font);
	}

	item_height = skin->get(LIST_ITEM_HEIGHT);
	item_width = this->GetClientSize().x - 4;

	for (int i=0; i<=MERRY_DEFAULT_LIST_BOX_ITEM_MAX_NUM; ++i)
	{
		m_items[i].x = 2;
		m_items[i].y = item_height * i + 2;
		m_items[i].width = item_width;
		m_items[i].height = item_height;
	}

	menu = new wxMenu;
	menu->Append(MENU_CMD_ADD, wxT("添加(&I)"));
	menu->Append(MENU_CMD_EDIT, wxT("编辑(&E)"));
	menu->Append(MENU_CMD_DEL, wxT("删除(&D)"));
	menu->Append(MENU_CMD_OPENDIR, wxT("定位(&L)"));
	menu->Append(MENU_CMD_INFO, wxT("详情(&M)"));
    return;
}

MerryListBoxPanel::~MerryListBoxPanel()
{
	__DEBUG_BEGIN("")
	if (menu)
		delete menu;
	menu = NULL;
	__DEBUG_END("")
}

void MerryListBoxPanel::SetCommandArray(const MerryCommandArray& commands)
{
//	const MerryCommand* cmd = this->GetSelectionCommand();

	m_commands = commands;

	commands_size = commands.size();

	if (commands.size() == 0)
	{
		m_topCommandIndex = -1;
		m_selectionCommandIndex = -1;
		return;
	}
	m_topCommandIndex = 0;
	m_selectionCommandIndex = 0;
	/* 自动选中前一个匹配的命令,感觉有点多余,新的版本取消了.
	if (!cmd)
		return;
	int	PreCmdID = cmd->GetCommandID();
	int ITEM_NUM = GetVisibleItemNum();

	for (int i=0; i<ITEM_NUM; ++i)
	{
		MerryCommand* command = m_commands[i];
		if (command->GetCommandID() == PreCmdID)
		{
			m_selectionCommandIndex = i;
			break;
		}
	}
	*/
}

void MerryListBoxPanel::SelectNext()
{
	this->SetSelection(m_selectionCommandIndex + 1);
}

void MerryListBoxPanel::SelectPrev()
{
	this->SetSelection(m_selectionCommandIndex - 1);
}

int MerryListBoxPanel::SetSelection(int index,int top)
{
	if (index == -1 && top >= 0)
	{
	#ifdef _ALMRUN_CONFIG_H_
		if (!g_config->get(IndexFrom0to9))
		{
			if (top == 0)
				top = 9;
			else
				--top;
		}
	#endif//#ifdef _ALMRUN_CONFIG_H_
		index = m_topCommandIndex + top;
		if (index>=(int)m_commands.size())
			return 0;
	}
	else if (index < 0 )
		index = m_commands.size() - 1;
	else if (index >= (int)m_commands.size())
		index = 0;
	if (index == m_selectionCommandIndex)
		return 1;

	m_selectionCommandIndex = index;
	if (m_selectionCommandIndex < m_topCommandIndex)
		m_topCommandIndex = m_selectionCommandIndex;
	else if (m_selectionCommandIndex - m_topCommandIndex >= MERRY_DEFAULT_LIST_BOX_ITEM_MAX_NUM)
		m_topCommandIndex = m_selectionCommandIndex - MERRY_DEFAULT_LIST_BOX_ITEM_MAX_NUM + 1;
	this->Refresh();
	return 1;
}


bool MerryListBoxPanel::DelSelectedItem()
{
	const MerryCommand* cmd = this->GetSelectionCommand();
	if (!cmd)
		return false;
//	assert(cmd);
	int flags = cmd->GetFlags();
	if ((flags & CMDS_FLAG_CMDS))
	{
		if (wxMessageBox(wxString::Format("删除 ID:[%d] %s\n命令:%s?",flags >> 4,cmd->GetCommandName(),cmd->GetCmd()),"提示",wxYES_NO|wxICON_WARNING) != wxYES)
			return false;
	#ifdef _ALMRUN_CONFIG_H_
		if (g_config->DeleteCmd(flags>>4))
	#endif//#ifdef _ALMRUN_CONFIG_H_
			return g_commands->DelCommand(cmd->GetCommandID());
	}
	else
		wxMessageBox("该命令可能是LUA脚本或自动生成的命令,无法编辑/删除");
	return false;
}

const MerryCommand* MerryListBoxPanel::GetSelectionCommand() const
{
	if (m_selectionCommandIndex < 0 || m_selectionCommandIndex > (int)m_commands.size())
		return NULL;
//	assert(0 <= m_selectionCommandIndex && m_selectionCommandIndex < (int)m_commands.size());
	return m_commands[m_selectionCommandIndex];
}

void MerryListBoxPanel::Popup()
{
	if (m_commands.size() == 0)
		return;
//	assert(m_commands.size() > 0);
	m_isPopup = true;
	this->SetHeight(this->CalcHeight());
	this->Refresh();
}

void MerryListBoxPanel::Dismiss()
{
	m_isPopup = false;
	this->SetHeight(0);
}

void  MerryListBoxPanel::onContextMenu(wxContextMenuEvent& e)
{
	e.StopPropagation();
	PopupMenu(menu);
}

void MerryListBoxPanel::onPopMenu(wxCommandEvent& e)
{
	int id = e.GetId();
	switch(id)
	{
		case MENU_CMD_INFO:
			{
				const MerryCommand* cmd = GetSelectionCommand();
				if (cmd)
					wxMessageBox(cmd->GetDetails(),cmd->GetCommandName());
			}
			break;
		case MENU_CMD_DEL:
			if (DelSelectedItem())
			{
				m_commands.erase(m_commands.begin() + m_selectionCommandIndex);
				this->Refresh();
			}
			break;
		case MENU_CMD_EDIT:
		case MENU_CMD_ADD:
			{
				DlgAddNewCmd* dlg;
				if (id == MENU_CMD_EDIT)
				{
					const MerryCommand* cmd = GetSelectionCommand();
					if (!(cmd->GetFlags() & CMDS_FLAG_CMDS)) 
					{
						wxMessageBox("该命令可能是LUA脚本或自动生成的命令,无法编辑/删除");
						return;
					}
					dlg = new DlgAddNewCmd(cmd->GetFlags()>>4);
//					dlg->flags = MENU_CMD_EDIT;
					//dlg->SetCmdID(cmd->GetFlags()>>4);
					//dlg->cmdName->SetValue(cmd->GetCommandName());
					//dlg->cmdDesc->SetValue(cmd->GetCommandDesc());
					//dlg->cmdKey->SetValue(cmd->GetTriggerKey());
					//dlg->cmdLine->SetValue(cmd->GetCmd());
					//dlg->cmdPath->SetValue(cmd->GetWorkDir());
				}
				else
					dlg = new DlgAddNewCmd();
				if (dlg->ShowModal() == wxID_OK)
					PostMessage(this->GetParent()->GetHWND(),WM_COMMAND,MENU_ITEM_RECONFIG,0);
				dlg->Destroy();
			}
			break;
		case MENU_CMD_OPENDIR:
			LocationExec = true;
			GetSelectionCommand()->Execute(wxEmptyString);
			break;

	}
}

void MerryListBoxPanel::OnMouseEvent(wxMouseEvent& e)
{
#ifdef _ALMRUN_CONFIG_H_
	if (e.LeftDClick() || (g_config->get(DoubleClick) == false && e.LeftUp()))
#else
	if (e.LeftUp())
#endif
	{
	#if defined(_ALMRUN_CONFIG_H_) && defined(__WXMSW__)
		SendMessage(HWND_TEXTCTRL,WM_KEYDOWN,VK_RETURN,0);
		SendMessage(HWND_TEXTCTRL,WM_KEYUP,VK_RETURN,0);
	#else
		const MerryCommand* command = m_commands[m_selectionCommandIndex];
		assert(command);
		command->Execute(wxEmptyString);
	#endif
		return;
	}
	#ifdef __WXMSW__
		::SetFocus(HWND_TEXTCTRL);
	#endif
	wxPoint pos = e.GetPosition();
	int itemIndex = pos.y / item_height;
	const wxRect& item = m_items[itemIndex];
	if (item.Contains(pos) == false || itemIndex >= this->GetVisibleItemNum())
	{
		e.StopPropagation();
		return;
	}
	e.Skip();
	itemIndex += m_topCommandIndex;
	if (itemIndex == m_selectionCommandIndex)
		return;

#ifdef _ALMRUN_CONFIG_H_
	bool refresh = e.ButtonDown() || (g_config->get(DoubleClick) == false && e.Moving());
#else
	bool refresh = e.ButtonDown() || e.Moving();
#endif

	if (g_config->get(ShowTip) && (refresh || e.Moving()))
	{
		MerryCommand* cmd = m_commands[itemIndex];
		if (cmd->GetCommandDesc().empty())
			this->SetToolTip(cmd->GetCmd());
		else
			this->SetToolTip(cmd->GetCommandDesc());
	}
	if (refresh)
	{
		m_selectionCommandIndex = itemIndex;
		this->Refresh();
	}
}

void MerryListBoxPanel::OnPaintEvent(wxPaintEvent& e)
{
	MerryPaintDC dc(this);
	dc.Clear();
	this->DrawBackground(dc);
	this->DrawBorder(dc);
	this->DrawItems(dc);
}

void MerryListBoxPanel::DrawBorder(MerryPaintDC& dc) const
{
	wxPoint p[4];
	p[0].x = 0;
	p[0].y = 0;

	p[1].x = this->GetClientSize().x - 2;
	p[1].y = 0;

	p[2].x = 0;
	p[2].y = this->GetClientSize().y - 2;

	p[3].x = p[1].x;
	p[3].y = p[2].y;

	dc.SetPen(wxPen(skin->get(WINDOW_COLOR),2));
	dc.DrawLine(p[0],p[1]);
	dc.DrawLine(p[0],p[2]);
	dc.DrawLine(p[1],p[3]);
	dc.DrawLine(p[2],p[3]);
}

void MerryListBoxPanel::DrawBackground(MerryPaintDC& dc) const
{
	const int h = this->GetVisibleItemNum() * item_height;
	dc.SetPen(*wxTRANSPARENT_PEN);
	if (m_listBackground.IsOk())
		dc.SetBrush(wxBrush(m_listBackground));
	else
		dc.SetBrush(wxBrush(skin->get(TEXT_BACKCOLOR)));
	dc.DrawRectangle(wxPoint(0,0),this->GetSize());  
//	dc.DrawBitmap(m_listBackground,0,0,false);

}

void MerryListBoxPanel::DrawItem(MerryPaintDC& dc,size_t item)
{
	int index = item + m_topCommandIndex;
	if (index >= commands_size)
		return;
	MerryCommand* command = m_commands[index];
	if (!command)
		return;

	int i = (g_config->get(IndexFrom0to9))?item:(item==9?0:item+1);
//	dc.DrawText(wxString::Format(wxT("%5.5s(%1d | %s"),command->GetTriggerKey(),i,command->GetCommandName()),2,item*item_height+2);
	dc.DrawLabel(wxString::Format(wxT("%5.5s(%1d | %s"),command->GetTriggerKey(),i,command->GetCommandName()),m_items[item],wxALIGN_LEFT);
}

void MerryListBoxPanel::DrawItems(MerryPaintDC& dc)
{
	const int ITEM_NUM = this->GetVisibleItemNum();
	if (ITEM_NUM == 0)
		return;

	int active_item = m_selectionCommandIndex - m_topCommandIndex;
	for (int i=0; i<ITEM_NUM; ++i)
	{
		if (active_item != i)
			DrawItem(dc,i);
	}

	if (show_title)
	{
	const MerryCommand* command = m_commands[m_selectionCommandIndex];
	const wxString& cmdLine = command->GetCmd();
	const wxString& cmdDesc = command->GetCommandDesc();
	if (this->flags > 0)
	{
		dc.DrawLabel("  输入命令参数按回车执行或按Esc键返回",m_items[ITEM_NUM], wxALIGN_LEFT);
		this->flags = 0;
	}else {
#ifdef _RELWITHDEBINFO
	dc.DrawLabel(wxString::Format(wxT(" 命令[%d]: %s"),command->GetOrder(),cmdLine.c_str()),item.rect, wxALIGN_LEFT);
#else
	if (!cmdLine.empty())
		dc.DrawLabel(wxString::Format(wxT(" 命令: %s"),cmdLine.c_str()),m_items[ITEM_NUM], wxALIGN_LEFT);
	else if (!cmdDesc.empty())
		dc.DrawLabel(wxString::Format(wxT(" 说明: %s"),cmdDesc.c_str()),m_items[ITEM_NUM], wxALIGN_LEFT);	
#endif
	}
	}
	dc.SetTextForeground(skin->get(LIST_FOCUSTEXT_COLOR));
	dc.SetBrush(wxBrush(skin->get(LIST_FOCUSBG_COLOR)));
	dc.DrawRectangle(m_items[active_item]);
	DrawItem(dc,active_item);
}

void MerryListBoxPanel::SetHeight(int height)
{
	wxSize size = this->GetClientSize();
	size.y = height;
	this->SetClientSize(size);

	wxWindow* parent = this->GetParent();
	assert(parent && parent->IsTopLevel());
	wxSize parentSize = parent->GetClientSize();
	parentSize.y = skin->get(MAIN_HEIGHT) + height;
	parent->SetClientSize(parentSize);
}

int MerryListBoxPanel::CalcHeight() const
{
	const int ITEM_NUM = this->GetVisibleItemNum();
	int height = ITEM_NUM * item_height + 4;
	if (show_title)
		height += skin->get(TITLE_HEIGHT);
	return height;
}

int MerryListBoxPanel::GetVisibleItemNum() const
{
	return std::min((int)m_commands.size(), MERRY_DEFAULT_LIST_BOX_ITEM_MAX_NUM);
}

bool MerryListBoxPanel::IsPopup() const
{
	return m_isPopup;
}
