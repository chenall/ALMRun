/////////////////////////////////////////////////////////////////////////////
// Name:        dlgaddnewcmd.h
// Purpose:     
// Author:      test
// Modified by: 
// Created:     09/06/2013 11:53:23
// RCS-ID:      
// Copyright:   test
// Licence:     
/////////////////////////////////////////////////////////////////////////////

#ifndef _DLGADDNEWCMD_H_
#define _DLGADDNEWCMD_H_

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma interface "DlgAddNewCmd.h"
#endif

/*!
 * Includes
 */

////@begin includes
#include "HotkeyCtrl.h"
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
#define ID_DLGADDNEWCMD 10000
#define ID_CMD_NAME 10001
#define ID_CMD_KEY 10005
#define ID_CMD_DESC 10002
#define ID_CMD_CMDLINE 10003
#define ID_CMD_WORKDIR 10004
#define ID_CMD_BROWSE 10008
#define ID_CMD_ADD_DIR 10010
#define SYMBOL_DLGADDNEWCMD_STYLE wxCAPTION|wxRESIZE_BORDER|wxSYSTEM_MENU|wxCLOSE_BOX|wxTAB_TRAVERSAL
#define SYMBOL_DLGADDNEWCMD_TITLE _("Ìí¼ÓÐÂÃüÁî")
#define SYMBOL_DLGADDNEWCMD_IDNAME ID_DLGADDNEWCMD
#define SYMBOL_DLGADDNEWCMD_SIZE wxDefaultSize
#define SYMBOL_DLGADDNEWCMD_POSITION wxDefaultPosition
////@end control identifiers


/*!
 * DlgAddNewCmd class declaration
 */

class DlgAddNewCmd: public wxDialog
{    
    DECLARE_DYNAMIC_CLASS( DlgAddNewCmd )
    DECLARE_EVENT_TABLE()

public:
    /// Constructors
    DlgAddNewCmd();
	DlgAddNewCmd(const int flags);
    DlgAddNewCmd( wxWindow* parent, wxWindowID id = SYMBOL_DLGADDNEWCMD_IDNAME, const wxString& caption = SYMBOL_DLGADDNEWCMD_TITLE, const wxPoint& pos = SYMBOL_DLGADDNEWCMD_POSITION, const wxSize& size = SYMBOL_DLGADDNEWCMD_SIZE, long style = SYMBOL_DLGADDNEWCMD_STYLE );

    /// Creation
    bool Create( wxWindow* parent, wxWindowID id = SYMBOL_DLGADDNEWCMD_IDNAME, const wxString& caption = SYMBOL_DLGADDNEWCMD_TITLE, const wxPoint& pos = SYMBOL_DLGADDNEWCMD_POSITION, const wxSize& size = SYMBOL_DLGADDNEWCMD_SIZE, long style = SYMBOL_DLGADDNEWCMD_STYLE );
	void SetCmdID(wxString& id) { long i;id.ToLong(&i);cmdID = i;};
	wxString GetCmdID() {return wxString::Format("%d",cmdID);};
	void SetCmdID(int id) {cmdID = id;};
    /// Destructor
    ~DlgAddNewCmd();

    /// Initialises member variables
    void Init();

    /// Creates the controls and sizers
    void CreateControls();

////@begin DlgAddNewCmd event handler declarations
	void OnOkButtonClick(wxCommandEvent& e);
	void OnBrowseClick(wxCommandEvent& e);
	void OnShow(wxShowEvent& e);
////@end DlgAddNewCmd event handler declarations

////@begin DlgAddNewCmd member function declarations

    /// Retrieves bitmap resources
    wxBitmap GetBitmapResource( const wxString& name );

    /// Retrieves icon resources
    wxIcon GetIconResource( const wxString& name );
////@end DlgAddNewCmd member function declarations

    /// Should we show tooltips?
    static bool ShowToolTips();
////@begin DlgAddNewCmd member variables
	wxTextCtrl* cmdName;
	wxTextCtrl* cmdDesc;
	wxTextCtrl* cmdLine;
	wxTextCtrl* cmdPath;
	HotkeyCtrl* cmdKey;
	wxCheckBox* HideRun;
	wxCheckBox* RunAs;
	wxCheckBox* RequiredArg;
	int flags;
	int cmdID;
////@end DlgAddNewCmd member variables
private:

};

#endif
    // _DLGADDNEWCMD_H_
