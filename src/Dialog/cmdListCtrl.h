#ifndef __MY_LIST_CTRL_H__
#define __MY_LIST_CTRL_H__

#include "wx/listctrl.h"
#include "ALMRunConfig.h"
#ifdef _ALMRUN_CONFIG_H_
#define DIRLIST_MODE 1
#define CMDLIST_MODE 0
class cmdListCtrl : public wxListCtrl
{
public:
    cmdListCtrl(wxWindow *parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition,
        const wxSize& size = wxDefaultSize, long style =  wxLC_REPORT|wxLC_HRULES|wxLC_VRULES);// | wxLC_LIST | wxLC_VIRTUAL);
	static void RunMenu(const int id,cmdListCtrl* t);
	static bool onDelete(const wxString& item);
	void ReLoadCmds();
	~cmdListCtrl();

protected:
//    void OnColClick(wxListEvent& event);
	void OnSelected(wxListEvent& event);
	void onKeyDown(wxListEvent&  e);
	void onRightClick(wxListEvent& event);
	void onDclick(wxMouseEvent& e);
	void onPopMenu(wxCommandEvent& e);


private:
//    void SetColumnImage(int col, int image);
	int mode;

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
};

enum
{
	DIRLIST_COL_PATH = 0,
	DIRLIST_COL_SUB,
	DIRLIST_COL_INCLUDE,
	DIRLIST_COL_EXCLUDE,
	DIRLIST_COL_ID,
};

#endif
#endif // __MY_LIST_CTRL_H__
