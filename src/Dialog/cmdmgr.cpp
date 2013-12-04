/////////////////////////////////////////////////////////////////////////////
// Name:        cmdmgr.cpp
// Purpose:     
// Author:      Chenall
// Modified by: 
// Created:     2013/6/18 11:24:54
// RCS-ID:      
// Copyright:   http://chenall.net
// Licence:     
/////////////////////////////////////////////////////////////////////////////

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma implementation "cmdmgr.h"
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

////@begin includes
#include "wx/imaglist.h"
////@end includes

#include "cmdmgr.h"

////@begin XPM images
#include "open.xpm"
#include "paste.xpm"
#include "cut.xpm"
#include "find.xpm"
#include "import.xpm"
////@end XPM images


/*!
 * cmdMgr type definition
 */

IMPLEMENT_DYNAMIC_CLASS( cmdMgr, wxDialog )


/*!
 * cmdMgr event table definition
 */

BEGIN_EVENT_TABLE( cmdMgr, wxDialog )

////@begin cmdMgr event table entries
EVT_TOOL_RANGE(ID_TOOL_ADD, ID_TOOL_IMPORT, cmdMgr::onToolClick)
//EVT_LIST_ITEM_SELECTED(ID_LISTCTRL, cmdMgr::onSelected)
//EVT_LIST_COL_CLICK(ID_LISTCTRL, cmdMgr::onSelected)
////@end cmdMgr event table entries

END_EVENT_TABLE()


/*!
 * cmdMgr constructors
 */

cmdMgr::cmdMgr()
{
    Init();
}

cmdMgr::cmdMgr( wxWindow* parent, wxWindowID id, const wxString& caption, const wxPoint& pos, const wxSize& size, long style )
{
    Init();
    Create(parent, id, caption, pos, size, style);
}


/*!
 * cmdMgr creator
 */

bool cmdMgr::Create( wxWindow* parent, wxWindowID id, const wxString& caption, const wxPoint& pos, const wxSize& size, long style )
{
////@begin cmdMgr creation
    wxDialog::Create( parent, id, caption, pos, size, style );

    CreateControls();
    if (GetSizer())
    {
        GetSizer()->SetSizeHints(this);
    }
    Centre();
////@end cmdMgr creation
    return true;
}

void cmdMgr::onSelected(wxListEvent& e)
{
	wxMessageBox(wxString::Format("%d,%d",e.GetId(),e.GetItem().GetId()));
}

/*!
 * cmdMgr destructor
 */

cmdMgr::~cmdMgr()
{
	__DEBUG_BEGIN("")
////@begin cmdMgr destruction
////@end cmdMgr destruction
	__DEBUG_END("")
}


/*!
 * Member initialisation
 */

void cmdMgr::Init()
{
////@begin cmdMgr member initialisation
    m_cmd_list = NULL;
////@end cmdMgr member initialisation
}


/*!
 * Control creation for cmdMgr
 */

void cmdMgr::CreateControls()
{    
////@begin cmdMgr content construction
    cmdMgr* itemDialog1 = this;

    wxBoxSizer* itemBoxSizer2 = new wxBoxSizer(wxVERTICAL);
    itemDialog1->SetSizer(itemBoxSizer2);

    wxToolBar* itemToolBar3 = new wxToolBar( itemDialog1, ID_TOOLBAR, wxDefaultPosition, wxDefaultSize, wxTB_FLAT|wxTB_TEXT|wxTB_NODIVIDER|wxTB_NOALIGN|wxTB_HORZ_LAYOUT);
    itemToolBar3->SetToolBitmapSize(wxSize(32, 32));
    wxBitmap itemtool4Bitmap(itemDialog1->GetBitmapResource(wxT("open.xpm")));
    wxBitmap itemtool4BitmapDisabled;
    itemToolBar3->AddTool(ID_TOOL_ADD, _T("添加"), itemtool4Bitmap, itemtool4BitmapDisabled, wxITEM_NORMAL, wxEmptyString, wxEmptyString);
    wxBitmap itemtool5Bitmap(itemDialog1->GetBitmapResource(wxT("paste.xpm")));
    wxBitmap itemtool5BitmapDisabled;
    itemToolBar3->AddTool(ID_TOOL_EDIT, _T("编辑"), itemtool5Bitmap, itemtool5BitmapDisabled, wxITEM_NORMAL, wxEmptyString, wxEmptyString);
    wxBitmap itemtool6Bitmap(itemDialog1->GetBitmapResource(wxT("cut.xpm")));
    wxBitmap itemtool6BitmapDisabled;
    itemToolBar3->AddTool(ID_TOOL_DELETE, _T("删除"), itemtool6Bitmap, itemtool6BitmapDisabled, wxITEM_NORMAL, wxEmptyString, wxEmptyString);
    itemToolBar3->AddSeparator();
    wxBitmap itemtool8Bitmap(itemDialog1->GetBitmapResource(wxT("find.xpm")));
    wxBitmap itemtool8BitmapDisabled;
    itemToolBar3->AddTool(ID_TOOL_CHECK, _T("校验"), itemtool8Bitmap, itemtool8BitmapDisabled, wxITEM_NORMAL, wxEmptyString, wxEmptyString);
	itemToolBar3->AddTool(ID_TOOL_IMPORT, _T("导入"), wxBitmap(import_xpm),itemtool8BitmapDisabled, wxITEM_NORMAL, wxEmptyString, wxEmptyString);
    itemToolBar3->Realize();
    itemBoxSizer2->Add(itemToolBar3, 0, wxGROW, 5);
	m_cmd_list = new cmdListCtrl(itemDialog1, ID_LISTCTRL, wxDefaultPosition, wxSize(-1,500));
    itemBoxSizer2->Add(m_cmd_list, 2, wxGROW|wxALL, 5);
////@end cmdMgr content construction
}


/*!
 * Should we show tooltips?
 */

bool cmdMgr::ShowToolTips()
{
    return true;
}

/*!
 * Get bitmap resources
 */

wxBitmap cmdMgr::GetBitmapResource( const wxString& name )
{
    // Bitmap retrieval
////@begin cmdMgr bitmap retrieval
    wxUnusedVar(name);
    if (name == _T("open.xpm"))
    {
        wxBitmap bitmap(open_xpm);
        return bitmap;
    }
    else if (name == _T("paste.xpm"))
    {
        wxBitmap bitmap(paste_xpm);
        return bitmap;
    }
    else if (name == _T("cut.xpm"))
    {
        wxBitmap bitmap(cut_xpm);
        return bitmap;
    }
    else if (name == _T("find.xpm"))
    {
        wxBitmap bitmap(find_xpm);
        return bitmap;
    }
	else if (name == _T("import.png"))
	{
		wxBitmap bitmap(import_xpm);
		return bitmap;
	}
    return wxNullBitmap;
////@end cmdMgr bitmap retrieval
}

/*!
 * Get icon resources
 */

wxIcon cmdMgr::GetIconResource( const wxString& name )
{
    // Icon retrieval
////@begin cmdMgr icon retrieval
    wxUnusedVar(name);
    return wxNullIcon;
////@end cmdMgr icon retrieval
}

void cmdMgr::onToolClick(wxCommandEvent& e)
{
	return cmdListCtrl::RunMenu(e.GetId(),m_cmd_list);
}