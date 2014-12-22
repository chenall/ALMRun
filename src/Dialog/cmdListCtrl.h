#ifndef __MY_LIST_CTRL_H__
#define __MY_LIST_CTRL_H__

#include "wx/listctrl.h"
#include "ALMRunConfig.h"
#ifdef _ALMRUN_CONFIG_H_
#define DIRLIST_MODE 1
#define CMDLIST_MODE 0

class ListSortInfo
{
public:
        ListSortInfo()
        {
                SortAscending = false;
                Column = -1;
        }
		wxString filter;
        bool SortAscending;
        int Column;
        class cmdListCtrl *ListCtrl;
};

class cmdListCtrl : public wxListCtrl
{
public:
    cmdListCtrl(wxWindow *parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition,
        const wxSize& size = wxDefaultSize, long style =  wxLC_REPORT|wxLC_HRULES|wxLC_VRULES);// | wxLC_LIST | wxLC_VIRTUAL);
	static void RunMenu(const int id,cmdListCtrl* t);
	static bool onDelete(const wxString& item);
	void SortFilter(const wxString& filter);
	void ReLoadCmds();
	~cmdListCtrl();

protected:
//    void OnColClick(wxListEvent& event);
	void OnSelected(wxListEvent& event);
	void onKeyDown(wxListEvent&  e);
	void onRightClick(wxListEvent& event);
	void onDclick(wxMouseEvent& e);
	void onPopMenu(wxCommandEvent& e);
	wxImageList *m_imageListSmall;

private:
//    void SetColumnImage(int col, int image);
	void OnColClick(wxListEvent& event);
	int mode;
	ListSortInfo SortInfo;

private:
    DECLARE_NO_COPY_CLASS(cmdListCtrl)
    DECLARE_EVENT_TABLE()
};

enum
{
	CMDLIST_COL_NAME = 0,
	CMDLIST_COL_CMD,
	CMDLIST_COL_KEY,
	CMDLIST_COL_DESC,
	CMDLIST_COL_ID,
	CMDLIST_COL_WORKDIR,
	COL_MAX,
};

enum
{
	DIRLIST_COL_NAME = 0,
	DIRLIST_COL_PATH,
	DIRLIST_COL_INCLUDE,
	DIRLIST_COL_EXCLUDE,
	DIRLIST_COL_ID,
	DIRLIST_COL_SUB,
};

#endif
#endif // __MY_LIST_CTRL_H__
