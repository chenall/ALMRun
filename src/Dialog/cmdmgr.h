/////////////////////////////////////////////////////////////////////////////
// Name:        cmdmgr.h
// Purpose:     
// Author:      Chenall
// Modified by: 
// Created:     2013/6/18 11:24:54
// RCS-ID:      
// Copyright:   http://chenall.net
// Licence:     
/////////////////////////////////////////////////////////////////////////////

#ifndef _CMDMGR_H_
#define _CMDMGR_H_

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma interface "cmdmgr.h"
#endif

/*!
 * Includes
 */

////@begin includes
//#include "cmdmgr_symbols.h"
#include "wx/toolbar.h"
#include "cmdListCtrl.h"
////@end includes

/*!
 * Forward declarations
 */

////@begin forward declarations
class wxListCtrl;
////@end forward declarations

/*!
 * Control identifiers
 */

////@begin control identifiers
#define ID_CMDMGR 10001
#define ID_TOOLBAR 10005
//ID_TOOL_ADD = MENU_CMD_ADD
#define ID_LISTCTRL 10007
#define ID_TOOL_ADD 10008
#define ID_TOOL_DELETE 10009
#define ID_TOOL_EDIT 10010
#define ID_TOOL_CHECK 10011
#define ID_TOOL_IMPORT 10012
#define ID_TOOL_COPY 10013
#define ID_TOOL_ADD_FOLDER 10014
#define ID_TOOL_FILTER 10016
#define SYMBOL_CMDMGR_STYLE wxCAPTION|wxRESIZE_BORDER|wxSYSTEM_MENU|wxCLOSE_BOX|wxMAXIMIZE_BOX|wxTAB_TRAVERSAL
#define SYMBOL_CMDMGR_TITLE _T("ALMRun √¸¡Óπ‹¿Ì∆˜")
#define SYMBOL_CMDMGR_IDNAME ID_CMDMGR
#define SYMBOL_CMDMGR_SIZE wxSize(600,-1)
#define SYMBOL_CMDMGR_POSITION wxDefaultPosition
////@end control identifiers


/*!
 * cmdMgr class declaration
 */

class cmdMgr: public wxDialog
{    
    DECLARE_DYNAMIC_CLASS( cmdMgr )
    DECLARE_EVENT_TABLE()

public:
    /// Constructors
    cmdMgr();
    cmdMgr( wxWindow* parent, wxWindowID id = SYMBOL_CMDMGR_IDNAME, const wxString& caption = SYMBOL_CMDMGR_TITLE, const wxPoint& pos = SYMBOL_CMDMGR_POSITION, const wxSize& size = SYMBOL_CMDMGR_SIZE, long style = SYMBOL_CMDMGR_STYLE );

    /// Creation
    bool Create( wxWindow* parent, wxWindowID id = SYMBOL_CMDMGR_IDNAME, const wxString& caption = SYMBOL_CMDMGR_TITLE, const wxPoint& pos = SYMBOL_CMDMGR_POSITION, const wxSize& size = SYMBOL_CMDMGR_SIZE, long style = SYMBOL_CMDMGR_STYLE );

    /// Destructor
    ~cmdMgr();

    /// Initialises member variables
    void Init();

    /// Creates the controls and sizers
    void CreateControls();

////@begin cmdMgr event handler declarations
	void onSelected(wxListEvent& e);
	void onToolClick(wxCommandEvent& e);
////@end cmdMgr event handler declarations

////@begin cmdMgr member function declarations

    /// Retrieves bitmap resources
    wxBitmap GetBitmapResource( const wxString& name );

    /// Retrieves icon resources
    wxIcon GetIconResource( const wxString& name );
////@end cmdMgr member function declarations

    /// Should we show tooltips?
    static bool ShowToolTips();

private:
	void onText(wxCommandEvent& event);
	void onChar(wxKeyEvent& event);

	////@begin cmdMgr member variables
    cmdListCtrl* m_cmd_list;
	wxTextCtrl *filter;
	////@end cmdMgr member variables
};

#endif
    // _CMDMGR_H_
