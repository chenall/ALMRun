#ifndef _MERRY_LIST_BOX_PANEL_H_
#define _MERRY_LIST_BOX_PANEL_H_

#include "MerryCommandManager.h"

#ifdef __WXMSW__
#include <wx/dcbuffer.h>
typedef wxBufferedPaintDC MerryPaintDC;
#else
typedef wxPaintDC MerryPaintDC;
#endif

typedef enum _list_fmt_
{
	LIST_CMD_ITEM,
	LIST_CMD_ID,
	LIST_CMD_DESC,
	LIST_CMD_CMD,
	LIST_CMD_KEY,
	LIST_CMD_NAME,
};

class MerryListBoxPanel : public wxPanel
{
public:
	MerryListBoxPanel(wxWindow* parent);
	~MerryListBoxPanel();

	void SetCommandArray(const MerryCommandArray& commands);
	void SelectNext();
	void SelectPrev();
	int flags;
	int SetSelection(int index,int top = -1);
	bool DelSelectedItem();
	const MerryCommand* GetSelectionCommand() const;

	bool IsPopup() const;
	void Popup();
	void Dismiss();

private:
	void OnMouseEvent(wxMouseEvent& e);
	void OnPaintEvent(wxPaintEvent& e);
	void onContextMenu(wxContextMenuEvent& e);
	void onPopMenu(wxCommandEvent& e);
	void OnKeyDownEvent(wxKeyEvent& e);

private:
	void DrawBorder(MerryPaintDC& dc) const;
	void DrawBackground(MerryPaintDC& dc) const;
	void DrawItems(MerryPaintDC& dc);
	void DrawItem(MerryPaintDC& dc,size_t item);
	int commands_size;
	wxString list_fmt;

	void SetHeight(int height);
	int CalcHeight() const;
	int GetVisibleItemNum() const;

private:
	MerryCommandArray m_commands;
	int m_topCommandIndex;
	int m_selectionCommandIndex;
	int item_height;
	int item_width;
	int border_width;

	wxRect m_items[MERRY_DEFAULT_LIST_BOX_ITEM_MAX_NUM+1];
	wxBitmap m_listBackground;

	bool m_isPopup;
	MerryPaintDC *dc;
	wxPanel *panel;

private:
	DECLARE_EVENT_TABLE()
};

#endif
