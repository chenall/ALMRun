/////////////////////////////////////////////////////////////////////////////
// Name:        DlgAddNewDir.h
// Purpose:     
// Author:      test
// Modified by: 
// Created:     09/06/2013 21:11:35
// RCS-ID:      
// Copyright:   test
// Licence:     
/////////////////////////////////////////////////////////////////////////////

#ifndef _DLGADDNEWDIR_H_
#define _DLGADDNEWDIR_H_

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma interface "DlgAddNewDir.h"
#endif

/*!
 * Includes
 */

////@begin includes
#include "wx/spinctrl.h"
////@end includes

/*!
 * Forward declarations
 */

////@begin forward declarations
////@end forward declarations

/*!
 * Control identifiers
 */

////@begin control identifiers
#define ID_DLGADDNEWDIR 10012
#define ID_DLG_DIR_DIR 10013
#define ID_BUTTON 10014
#define ID_DIR_INCLUDE 10015
#define ID_DIR_EXCLUDE 10000
#define ID_DIR_SUB 10016
#define ID_ADD_AS_CMD 10017
#define SYMBOL_DLGADDNEWDIR_STYLE wxCAPTION|wxRESIZE_BORDER|wxSYSTEM_MENU|wxCLOSE_BOX|wxTAB_TRAVERSAL
#define SYMBOL_DLGADDNEWDIR_TITLE _T("添加自动扫描目录")
#define SYMBOL_DLGADDNEWDIR_IDNAME ID_DLGADDNEWDIR
#define SYMBOL_DLGADDNEWDIR_SIZE wxSize(360, 300)
#define SYMBOL_DLGADDNEWDIR_POSITION wxDefaultPosition
#define ADDDIR_FLAG_NORMAL 0
#define ADDDIR_FLAG_EDIT  1
#define ADDDIR_FLAG_CMDS  2
////@end control identifiers


/*!
 * DlgAddNewDir class declaration
 */

class DlgAddNewDir: public wxDialog
{    
    DECLARE_DYNAMIC_CLASS( DlgAddNewDir )
    DECLARE_EVENT_TABLE()

public:
    /// Constructors
    DlgAddNewDir();
    DlgAddNewDir( wxWindow* parent, wxWindowID id = SYMBOL_DLGADDNEWDIR_IDNAME, const wxString& caption = SYMBOL_DLGADDNEWDIR_TITLE, const wxPoint& pos = SYMBOL_DLGADDNEWDIR_POSITION, const wxSize& size = SYMBOL_DLGADDNEWDIR_SIZE, long style = SYMBOL_DLGADDNEWDIR_STYLE );

    /// Creation
    bool Create( wxWindow* parent, wxWindowID id = SYMBOL_DLGADDNEWDIR_IDNAME, const wxString& caption = SYMBOL_DLGADDNEWDIR_TITLE, const wxPoint& pos = SYMBOL_DLGADDNEWDIR_POSITION, const wxSize& size = SYMBOL_DLGADDNEWDIR_SIZE, long style = SYMBOL_DLGADDNEWDIR_STYLE );

    /// Destructor
    ~DlgAddNewDir();

    /// Initialises member variables
    void Init();

    /// Creates the controls and sizers
    void CreateControls();

////@begin DlgAddNewDir event handler declarations
	void onButtonClick(wxCommandEvent& e);
	void onCheckBoxClick(wxCommandEvent& e);
////@end DlgAddNewDir event handler declarations

////@begin DlgAddNewDir member function declarations
    /// Retrieves bitmap resources
    wxBitmap GetBitmapResource( const wxString& name );

    /// Retrieves icon resources
    wxIcon GetIconResource( const wxString& name );
////@end DlgAddNewDir member function declarations

    /// Should we show tooltips?
    static bool ShowToolTips();
	int SetMode(const int mode);
////@begin DlgAddNewDir member variables
	wxTextCtrl* dirName;
	wxTextCtrl* dirInclude;
	wxTextCtrl* dirExclude;
	wxSpinCtrl* dirSub;
	wxCheckBox* AddAsCmd;
	UINT DirId;
////@end DlgAddNewDir member variables
private:
	int flags;
};

#endif
    // _DLGADDNEWDIR_H_
