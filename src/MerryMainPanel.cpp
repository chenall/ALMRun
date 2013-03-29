#include "MerryMainPanel.h"
#include "MerryConfig.h"

BEGIN_EVENT_TABLE(MerryMainPanel, wxPanel)
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
