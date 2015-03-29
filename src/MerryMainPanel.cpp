#include "MerryMainPanel.h"
#include "MerryTaskBarIcon.h"

BEGIN_EVENT_TABLE(MerryMainPanel, wxPanel)
	EVT_CONTEXT_MENU(MerryMainPanel::onContextMenu)
	EVT_MENU_RANGE(MENU_ITEM_FIRST,MENU_ITEM_LAST,MerryTaskBarIcon::onPopMenu)
	EVT_MOUSE_EVENTS(MerryMainPanel::OnMouseEvent)
	EVT_PAINT(MerryMainPanel::OnPaintEvent)
END_EVENT_TABLE()

MerryMainPanel::MerryMainPanel(wxWindow* parent):
	wxPanel(parent, wxID_ANY, wxPoint(0, 0), parent->GetClientSize())
{
	m_textCtrl = new MerryTextCtrl(this);
	menu = new wxMenu;
	menu->Append(MENU_ITEM_GUI_CONFIG, wxT("参数配置(&C)"));
	menu->Append(MENU_ITEM_CMDMGR, wxT("命令管理(&S)"));
	menu->Append(MENU_ITEM_SKIN,wxT("切换主题"));
	menu->Append(MENU_ITEM_RECONFIG, wxT("刷新配置(R)"));
	menu->Append(MENU_ITEM_ABOUT, wxT("关于ALMRun(&A)"));
	menu->Append(MENU_ITEM_EXIT, wxT("退出(&X)"));
	if (skin->get(TITLE_ENABLED))
	{
		title_cfg.x = skin->get(TITLE_LEFT);
		title_cfg.y = skin->get(TITLE_TOP);
		title_cfg.width = skin->get(TITLE_WIDTH);
		title_cfg.height = skin->get(TITLE_HEIGHT);
		title_align = skin->get(TITLE_ALIGN);
	}
	else
		title_align = -1;

	if (!skin->get(SKINPICTURE).empty())
	{
		m_background.LoadFile(skin->get(SKINPICTURE), wxBITMAP_TYPE_ANY);
	}
}

MerryMainPanel::~MerryMainPanel()
{
	__DEBUG_BEGIN("")
	wxDELETE(menu);
	wxDELETE(m_textCtrl);
	__DEBUG_END("")
}
	
void MerryMainPanel::setTitle(const wxString &title)
{
	title_info = title;
}

void MerryMainPanel::onContextMenu(wxContextMenuEvent& e)
{
	e.StopPropagation();

	PopupMenu(menu);
    return;
}

MerryTextCtrl* MerryMainPanel::GetTextCtrl()
{
	return m_textCtrl;
}

void MerryMainPanel::OnMouseEvent(wxMouseEvent& e)
{
	if (e.LeftDown())
	{
		m_mousePosition = e.GetPosition();
		assert(!this->HasCapture());
		this->CaptureMouse();
	}
	else if (e.Dragging() && !e.RightIsDown())
	{
		wxPoint offset = e.GetPosition() - m_mousePosition;
		wxWindow* parent = this->GetParent();
		assert(parent && parent->IsTopLevel());
		parent->SetPosition(parent->GetPosition() + offset);
		if (g_config && g_config->get(SaveWinPos))
		{
			offset = parent->GetPosition();
			g_config->conf->Write("/Window/WinLeft",offset.x);
			g_config->conf->Write("/Window/WinTop",offset.y);
		}
	}
	else if (e.LeftUp() && this->HasCapture())
		this->ReleaseMouse();

	e.Skip();
}

void MerryMainPanel::OnPaintEvent(wxPaintEvent& e)
{
	wxPaintDC dc = wxPaintDC(this);
	dc.Clear();

	if (m_background.IsOk())
		dc.DrawBitmap(m_background, 0, 0, false);

	if (title_align == -1)
		return;
	dc.SetTextForeground(skin->get(TITLE_TEXT_COLOR));
	dc.DrawLabel(title_info,title_cfg,title_align);
}
