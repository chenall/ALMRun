#include "MerryMainPanel.h"
#include "MerryConfig.h"
#include "MerryTaskBarIcon.h"

BEGIN_EVENT_TABLE(MerryMainPanel, wxPanel)
	EVT_CONTEXT_MENU(MerryMainPanel::onContextMenu)
	EVT_MENU(MENU_ITEM_OPEN_CONFIG, MerryTaskBarIcon::onPopMenu)
	EVT_MENU(MENU_ITEM_CONFIG, MerryTaskBarIcon::onPopMenu)
	EVT_MENU(MENU_ITEM_ABOUT, MerryTaskBarIcon::onPopMenu)
	EVT_MENU(MENU_ITEM_EXIT, MerryTaskBarIcon::onPopMenu)
	EVT_MOUSE_EVENTS(MerryMainPanel::OnMouseEvent)
	EVT_PAINT(MerryMainPanel::OnPaintEvent)
END_EVENT_TABLE()

MerryMainPanel::MerryMainPanel(wxWindow* parent):
	wxPanel(parent, wxID_ANY, wxPoint(0, 0), parent->GetClientSize())
{
	m_textCtrl = new MerryTextCtrl(this);

	if (!wxImage::FindHandler(wxBITMAP_TYPE_PNG))
		wxImage::AddHandler(new wxPNGHandler);
	bool isOk = m_background.LoadFile(MERRY_DEFAULT_BACKGROUND_FILE, wxBITMAP_TYPE_PNG);
	assert(isOk);
}

	
void MerryMainPanel::onContextMenu(wxContextMenuEvent& e)
{
	e.StopPropagation();
	wxMenu* menu = new wxMenu;
	menu->Append(MENU_ITEM_OPEN_CONFIG, wxT("显示配置(&S)"));
	menu->Append(MENU_ITEM_CONFIG, wxT("刷新配置(R)"));
	menu->Append(MENU_ITEM_ABOUT, wxT("关于ALMRun(&A)"));
	menu->Append(MENU_ITEM_EXIT, wxT("退出(&X)"));
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
	else if (e.Dragging())
	{
		wxPoint offset = e.GetPosition() - m_mousePosition;
		wxWindow* parent = this->GetParent();
		assert(parent && parent->IsTopLevel());
		parent->SetPosition(parent->GetPosition() + offset);
	}
	else if (e.LeftUp() && this->HasCapture())
		this->ReleaseMouse();

	e.Skip();
}

void MerryMainPanel::OnPaintEvent(wxPaintEvent& e)
{
	wxPaintDC dc(this);
	dc.DrawBitmap(m_background, 0, 0, false);
}
