#include "MerryListBoxPanel.h"

BEGIN_EVENT_TABLE(MerryListBoxPanel, wxPanel)
	EVT_MOUSE_EVENTS(MerryListBoxPanel::OnMouseEvent)
	EVT_PAINT(MerryListBoxPanel::OnPaintEvent)
END_EVENT_TABLE()

MerryListBoxPanel::MerryListBoxPanel(wxWindow* parent):
	wxPanel(parent, wxID_ANY,
		wxPoint(0, parent->GetClientSize().y),
		wxSize(parent->GetClientSize().x, 0))
{
	m_topCommandIndex = -1;
	m_selectionCommandIndex = -1;
	m_isPopup = false;

	if (!wxImage::FindHandler(wxBITMAP_TYPE_PNG))
		wxImage::AddHandler(new wxPNGHandler);
	bool isOk = m_selectionItemBackground.LoadFile(MERRY_DEFAULT_LIST_BOX_ITEM_SELECTION_BACKGROUND_FILE, wxBITMAP_TYPE_PNG);
	assert(isOk);
	
	for (int i=0; i<=MERRY_DEFAULT_LIST_BOX_ITEM_MAX_NUM; ++i)
	{
		ListBoxItem& item = m_items[i];
		item.rect.x = 0;
		item.rect.y = MERRY_DEFAULT_LIST_BOX_ITEM_HEIGHT * i + MERRY_DEFAULT_LIST_BOX_ITEM_MARGIN;
		item.rect.width = this->GetClientSize().x;
		item.rect.height = MERRY_DEFAULT_LIST_BOX_ITEM_HEIGHT - MERRY_DEFAULT_LIST_BOX_ITEM_MARGIN;
		item.mainRect.x = MERRY_DEFAULT_SEPERATOR_OFFSET + MERRY_DEFAULT_SEPERATOR_MARGIN + 1;
		item.mainRect.y = MERRY_DEFAULT_LIST_BOX_ITEM_HEIGHT * i + MERRY_DEFAULT_LIST_BOX_ITEM_MARGIN;
		item.mainRect.width = this->GetClientSize().x - MERRY_DEFAULT_SEPERATOR_OFFSET - MERRY_DEFAULT_SEPERATOR_MARGIN - 1;
		item.mainRect.height = MERRY_DEFAULT_LIST_BOX_ITEM_HEIGHT - MERRY_DEFAULT_LIST_BOX_ITEM_MARGIN;
		item.subRect.x = 1;
		item.subRect.y = MERRY_DEFAULT_LIST_BOX_ITEM_HEIGHT * i + MERRY_DEFAULT_LIST_BOX_ITEM_MARGIN;
		item.subRect.width = MERRY_DEFAULT_SEPERATOR_OFFSET - MERRY_DEFAULT_SEPERATOR_MARGIN - 2;
		item.subRect.height = MERRY_DEFAULT_LIST_BOX_ITEM_HEIGHT - MERRY_DEFAULT_LIST_BOX_ITEM_MARGIN;
	}

	this->SetBackgroundColour(*wxWHITE);
	this->SetBackgroundStyle(wxBG_STYLE_CUSTOM);

	if (MERRY_DEFAULT_LIST_BOX_ITEM_FONT_HEIGHT)
	{
		wxFont font = this->GetFont();
		font.SetPixelSize(wxSize(0, MERRY_DEFAULT_LIST_BOX_ITEM_FONT_HEIGHT));
		this->SetOwnFont(font);
	}
}

void MerryListBoxPanel::SetCommandArray(const MerryCommandArray& commands)
{
	m_commands = commands;

	if (commands.size() == 0)
	{
		m_topCommandIndex = -1;
		m_selectionCommandIndex = -1;
	}
	else
	{
		m_topCommandIndex = 0;
		m_selectionCommandIndex = 0;
	}
}

void MerryListBoxPanel::SelectNext()
{
	this->SetSelection(m_selectionCommandIndex + 1,0);
}

void MerryListBoxPanel::SelectPrev()
{
	this->SetSelection(m_selectionCommandIndex - 1,0);
}

int MerryListBoxPanel::SetSelection(int index,int top = 0)
{
	if (index == -1 && top >= 0)
		index = m_topCommandIndex + top;
	if (index < 0 || index >= (int)m_commands.size())
		return 0;
	if (index == m_selectionCommandIndex)
		return 0;

	m_selectionCommandIndex = index;
	if (m_selectionCommandIndex < m_topCommandIndex)
		m_topCommandIndex = m_selectionCommandIndex;
	else if (m_selectionCommandIndex - m_topCommandIndex >= MERRY_DEFAULT_LIST_BOX_ITEM_MAX_NUM)
		m_topCommandIndex = m_selectionCommandIndex - MERRY_DEFAULT_LIST_BOX_ITEM_MAX_NUM + 1;

	this->Refresh();
	return 1;
}

const MerryCommand* MerryListBoxPanel::GetSelectionCommand() const
{
	assert(0 <= m_selectionCommandIndex && m_selectionCommandIndex < (int)m_commands.size());
	return m_commands[m_selectionCommandIndex];
}

void MerryListBoxPanel::Popup()
{
	assert(m_commands.size() > 0);
	m_isPopup = true;
	this->SetHeight(this->CalcHeight());
	this->Refresh();
}

void MerryListBoxPanel::Dismiss()
{
	m_isPopup = false;
	this->SetHeight(0);
}

void MerryListBoxPanel::OnMouseEvent(wxMouseEvent& e)
{
	if (e.Moving())
	{
		wxPoint pos = e.GetPosition();
		int itemIndex = pos.y / MERRY_DEFAULT_LIST_BOX_ITEM_HEIGHT;
		if (itemIndex >= this->GetVisibleItemNum())
			return;
		const ListBoxItem& item = m_items[itemIndex];
		if (item.rect.Contains(pos))
		{
			m_selectionCommandIndex = m_topCommandIndex + itemIndex;
			this->Refresh();
		}
	}
	else if (e.LeftUp())
	{
		const MerryCommand* command = m_commands[m_selectionCommandIndex];
		assert(command);
		command->ExecuteCommand(wxT(''));
	}
	e.Skip();
}

void MerryListBoxPanel::OnPaintEvent(wxPaintEvent& e)
{
	MerryPaintDC dc(this);
	dc.Clear();
	this->DrawBorder(dc);
	this->DrawBackground(dc);
	this->DrawItems(dc);
}

void MerryListBoxPanel::DrawBorder(MerryPaintDC& dc) const
{
	wxPoint p[4];
	p[0].x = 0;
	p[0].y = 0;
	p[1].x = 0;
	p[1].y = this->GetClientSize().y - 1;
	p[2].x = this->GetClientSize().x - 1;
	p[2].y = this->GetClientSize().y - 1;
	p[3].x = this->GetClientSize().x - 1;
	p[3].y = 0;
#ifdef __WXMSW__
	p[3].y = -1;
#endif
    dc.SetPen(wxPen(MERRY_DEFAULT_BORDER_COLOR));
	dc.DrawLine(p[0].x, p[0].y, p[1].x, p[1].y);
	dc.DrawLine(p[1].x, p[1].y, p[2].x, p[2].y);
	dc.DrawLine(p[2].x, p[2].y, p[3].x, p[3].y);
}

void MerryListBoxPanel::DrawBackground(MerryPaintDC& dc) const
{
    // dc.SetPen(wxPen(MERRY_DEFAULT_BORDER_COLOR));
	// dc.DrawLine(MERRY_DEFAULT_SEPERATOR_OFFSET, 0, MERRY_DEFAULT_SEPERATOR_OFFSET, this->GetClientSize().y - 1);

    dc.SetPen(*wxTRANSPARENT_PEN);
	dc.SetBrush(wxBrush(MERRY_DEFAULT_LIST_BOX_MAIN_COLOR));
	dc.DrawRectangle(MERRY_DEFAULT_SEPERATOR_OFFSET + 1, 0, this->GetClientSize().x - MERRY_DEFAULT_SEPERATOR_OFFSET - 2, this->GetClientSize().y - 1);
}

void MerryListBoxPanel::DrawItems(MerryPaintDC& dc)
{
	const int ITEM_NUM = this->GetVisibleItemNum();
	if (ITEM_NUM == 0)
		return;
	for (int i=0; i<ITEM_NUM; ++i)
	{
		const ListBoxItem& item = m_items[i];
		char c_item[5];
		assert(i + m_topCommandIndex < (int)m_commands.size());
		const MerryCommand* command = m_commands[i + m_topCommandIndex];
		assert(command);

		if (m_selectionCommandIndex - m_topCommandIndex == i)
		{
			dc.DrawBitmap(m_selectionItemBackground, item.rect.x, item.rect.y, false);
		}
		sprintf(c_item,"(%1d.",i);
		dc.SetTextForeground(MERRY_DEFAULT_LIST_BOX_MAIN_FONT_COLOR);
		dc.DrawLabel(command->GetCommandName(), item.mainRect, wxALIGN_LEFT);
		dc.SetTextForeground(MERRY_DEFAULT_LIST_BOX_SUB_FONT_COLOR);
		dc.DrawLabel(command->GetTriggerKey()+c_item, item.subRect, wxALIGN_RIGHT);
	}
	const MerryCommand* command = m_commands[m_selectionCommandIndex];
	const ListBoxItem& item = m_items[ITEM_NUM];
	dc.DrawBitmap(m_selectionItemBackground, item.rect.x, item.rect.y, false);
	dc.SetTextForeground(MERRY_DEFAULT_LIST_BOX_MAIN_FONT_COLOR);
	const wxString& cmdLine = command->GetCommandLine();
	const wxString& cmdDesc = command->GetCommandDesc();
	if (this->flags > 0)
	{
		dc.DrawLabel("  输入命令参数按回车执行或按Esc键返回",item.rect, wxALIGN_LEFT);
		this->flags = 0;
		return;
	}
	if (!cmdDesc.empty())
	{
		dc.DrawLabel("说明: "+cmdDesc.c_str(),item.rect, wxALIGN_LEFT);	
	}
	else if (!cmdLine.empty())
	{
		dc.DrawLabel("命令: "+cmdLine.c_str(),item.rect, wxALIGN_LEFT);
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
	parentSize.y = MERRY_DEFAULT_HEIGHT + height;
	parent->SetClientSize(parentSize);
}

int MerryListBoxPanel::CalcHeight() const
{
	const int ITEM_NUM = this->GetVisibleItemNum();
	int height = (ITEM_NUM+1) * MERRY_DEFAULT_LIST_BOX_ITEM_HEIGHT;
	if (ITEM_NUM > 0)
		height += MERRY_DEFAULT_LIST_BOX_ITEM_MARGIN;
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
