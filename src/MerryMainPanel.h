#ifndef _MERRY_MAIN_PANEL_H_
#define _MERRY_MAIN_PANEL_H_

#include "MerryTextCtrl.h"

class MerryMainPanel : public wxPanel
{
public:
	MerryMainPanel(wxWindow* parent);

	MerryTextCtrl* GetTextCtrl();

private:
	void OnMouseEvent(wxMouseEvent& e);
	void OnPaintEvent(wxPaintEvent& e);
	void onContextMenu(wxContextMenuEvent& e);
private:
	MerryTextCtrl* m_textCtrl;
	wxPoint m_mousePosition;
	wxBitmap m_background;

private:
	DECLARE_EVENT_TABLE()
};

#endif
