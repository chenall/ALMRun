#ifndef __MY_LIST_CTRL_H__
#define __MY_LIST_CTRL_H__

#include "wx/listctrl.h"
#include "ALMRunConfig.h"

class cmdListCtrl : public wxListCtrl
{
public:
    cmdListCtrl(wxWindow *parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition,
        const wxSize& size = wxDefaultSize, long style =  wxLC_REPORT|wxLC_HRULES|wxLC_VRULES);// | wxLC_LIST | wxLC_VIRTUAL);
	static void RunMenu(const int id,cmdListCtrl* t);
	static bool onDelete(const wxString& item);
	~cmdListCtrl();

protected:
//    void OnColClick(wxListEvent& event);
	void OnSelected(wxListEvent& event);
	void onRightClick(wxListEvent& event);
	void onPopMenu(wxCommandEvent& e);


private:
//    void SetColumnImage(int col, int image);

private:
    DECLARE_NO_COPY_CLASS(cmdListCtrl)
    DECLARE_EVENT_TABLE()
};

enum
{
	CMDLIST_COL_ID = 0,
	CMDLIST_COL_NAME,
	CMDLIST_COL_CMD,
	CMDLIST_COL_KEY,
	CMDLIST_COL_DESC
};
#endif // __MY_LIST_CTRL_H__
