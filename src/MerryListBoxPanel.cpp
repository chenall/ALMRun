#include "MerryListBoxPanel.h"
#include "DlgAddNewCmd.h"
#include "MerryApp.h"

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
	wxString list_font(skin->get(LIST_FONT));

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

	font.SetFaceName(skin->get(LIST_FONT));
	if (skin->get(LIST_FONT_SIZE))
		font.SetPointSize(skin->get(LIST_FONT_SIZE));

	this->SetOwnFont(font);
	font.FreeResource();

	border_width = skin->get(LIST_BORDER_WIDTH);
	item_height = skin->get(LIST_ITEM_HEIGHT);
	item_width = skin->get(LIST_BOX_WIDTH) - border_width * 2;
	list_fmt = skin->get(LIST_FMT);
	int left = skin->get(LIST_BOX_LEFT);

	for (int i=0; i<=MERRY_DEFAULT_LIST_BOX_ITEM_MAX_NUM; ++i)
	{
		m_items[i].x = border_width;
		m_items[i].y = item_height * i + border_width;
		m_items[i].width = item_width;
		m_items[i].height = item_height;
	}
    return;
}

MerryListBoxPanel::~MerryListBoxPanel()
{
	__DEBUG_BEGIN("")

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
		wxMessageBox("该命令可能是LUA脚本或自动生成的命令,无法删除");
	return false;
}

const MerryCommand* MerryListBoxPanel::GetSelectionCommand() const
{
	if (m_selectionCommandIndex < 0 || m_selectionCommandIndex > (int)m_commands.size())
		return NULL;
	return m_commands[m_selectionCommandIndex];
}

void MerryListBoxPanel::Popup()
{
	if (m_commands.size() == 0)
		return;
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
	const MerryCommand* cmd = GetSelectionCommand();

	wxMenu menu;
	menu.Append(MENU_CMD_ADD, wxT("添加(&I)"));
	menu.Append(MENU_CMD_EDIT, wxT("编辑(&E)"));
	if ((cmd->GetFlags() & CMDS_FLAG_CMDS))
	{
		menu.Append(MENU_CMD_DEL, wxT("删除(&D)"));
	}
	menu.Append(MENU_CMD_OPENDIR, wxT("定位(&L)"));
	menu.Append(MENU_CMD_INFO, wxT("详情(&M)"));
	menu.Append(MENU_CMD_RUNAS, wxT("运行为(&A)"));

	PopupMenu(&menu);
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
						wxMessageBox("该命令可能是LUA脚本或自动生成的命令,继续编辑将会自动转为ALMRun常规命令","提示");
						dlg = new DlgAddNewCmd();
						dlg->cmdName->SetValue(cmd->GetCommandName());
						dlg->cmdDesc->SetValue(cmd->GetCommandDesc());
						dlg->cmdKey->SetValue(cmd->GetTriggerKey());
						dlg->cmdLine->SetValue(cmd->GetCmd());
						dlg->cmdPath->SetValue(cmd->GetWorkDir());
					}
					else
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
		case MENU_CMD_RUNAS:
			cmdrun_flags |= CMDRUN_FLAG_RUNAS;
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
	if (skin->get(LIST_BACKCOLOR) != wxTransparentColour)
		dc.SetBackgroundMode(wxPENSTYLE_SOLID);
	this->DrawBackground(dc);
	this->DrawBorder(dc);
	this->DrawItems(dc);
}

void MerryListBoxPanel::DrawBorder(MerryPaintDC& dc) const
{
	wxPoint p[5];
	if (!border_width)
		return;
	int bw = border_width/2;
	if (!bw)
		++bw;
	p[0].x = bw;
	p[0].y = bw;

	p[1].x = this->GetClientSize().x - bw;
	p[1].y = bw;

	p[2].x = bw;
	p[2].y = this->GetClientSize().y - bw;

	p[3].x = p[1].x;
	p[3].y = p[2].y;
	p[4].y = p[2].y - item_height - border_width;
	dc.SetPen(wxPen(skin->get(WINDOW_COLOR),border_width,skin->get(LIST_BORDER_STYLE)));
	dc.DrawLine(p[0],p[1]);
	dc.DrawLine(p[0],p[2]);
	dc.DrawLine(p[1],p[3]);
	dc.DrawLine(p[2],p[3]);

	if (g_config->get(ShowCommandLine))
		dc.DrawLine(bw,p[4].y,p[1].x,p[4].y);
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

wxString printfmt(long n,const wxString &str)
{
	size_t max = abs(n);
	size_t str_len = str.size();
	size_t len = 0,i = 0;
	for(len = 0;i < max && len<str_len;++len)
	{
		if (str[len].IsAscii())
			++i;
		else
			i += 2;
	}

	if (i>len)
	{
		i -= len;
		n -= (n > 0)?i:-i;
	}
	return wxString::Format("%*.*s",n,len,str);
}

void MerryListBoxPanel::DrawItem(MerryPaintDC& dc,size_t item)
{
	int index = item + m_topCommandIndex;
	if (index >= commands_size)
		return;
	MerryCommand* command = m_commands[index];
	if (!command)
		return;

	wxString fmt = skin->get(LIST_FMT);
	wxString label = "";
	for(size_t i=0;i<fmt.size();++i)
	{
		wxChar c=fmt[i];
		if (c == '$')
		{
			c=wxTolower(fmt[++i]);

			switch(c)
			{
				case '$':
					label += wxString::Format("%*d",skin->get(LISTFMT_NUM_MAX),g_config->get(IndexFrom0to9)?item:(item+1)%10);
					break;
				case 'i':
					label +=wxString::Format("%*d",skin->get(LISTFMT_ID_MAX),command->GetCommandID());
					break;
				case 'n':
					label += printfmt(skin->get(LISTFMT_NAME_MAX),command->GetCommandName());
					break;
				case 'd':
					label += printfmt(skin->get(LISTFMT_DESC_MAX),command->GetCommandDesc());
					break;
				case 'c':
					label += printfmt(skin->get(LISTFMT_CMD_MAX),command->GetCmd());
					break;
				case 'k':
					label += printfmt(skin->get(LISTFMT_KEY_MAX),command->GetTriggerKey());
					break;
			}

		} else {
			label += c;
		}
	}

	dc.DrawLabel(label,m_items[item],wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL);
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

	dc.SetTextForeground(skin->get(LIST_FOCUSTEXT_COLOR));
	dc.SetTextBackground(skin->get(LIST_FOCUSBG_COLOR));
	dc.SetBrush(wxBrush(skin->get(LIST_FOCUSBG_COLOR),wxBRUSHSTYLE_SOLID));
	dc.SetPen(wxPen(wxColor(skin->get(LIST_FOCUSBG_COLOR)),0));
	dc.DrawRectangle(m_items[active_item]);
	DrawItem(dc,active_item);

	const MerryCommand* command = m_commands[m_selectionCommandIndex];

	::wxGetApp().GetFrame().setTitle(command->GetCommandDesc());
	if (g_config->get(ShowCommandLine))
	{
		const wxString& cmdLine = command->GetCmd();
		const wxString& cmdDesc = command->GetCommandDesc();
		wxString title;

		if (this->flags > 0)
		{
			title = "  输入命令参数按回车执行或按Esc键返回";
			this->flags = 0;
		}else {
	#ifdef _RELWITHDEBINFO
			title = wxString::Format(wxT(" 命令[%d]: %s"),command->GetOrder(),cmdLine.c_str());
	#else
		if (!cmdLine.empty())
			title = wxString::Format(wxT("命令: %s"),cmdLine.c_str());
		else if (!cmdDesc.empty())
			title = wxString::Format(wxT("说明: %s"),cmdDesc.c_str());	
	#endif
		}
		dc.SetBrush(wxBrush(skin->get(TEXT_BACKCOLOR)));
		wxRect rect = m_items[ITEM_NUM];
		wxFont font=dc.GetFont();
		font.SetPointSize(skin->get(FONT_SIZE));
		dc.SetFont(font);
		dc.SetBackgroundMode(wxPENSTYLE_TRANSPARENT);
		dc.SetTextForeground(skin->get(TEXT_COLOR));
		rect.y += border_width;
		dc.DrawRectangle(rect);
		dc.DrawLabel(title,rect,wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL);
	}
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
	int height = ITEM_NUM * item_height + border_width * 2;
	if (g_config->get(ShowCommandLine))
		height += item_height + border_width;
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
