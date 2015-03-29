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
#include <wx/tglbtn.h>
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
#define ID_TGLBUTTON 10006
#define SYMBOL_DLGADDNEWCMD_STYLE wxCAPTION|wxRESIZE_BORDER|wxSYSTEM_MENU|wxCLOSE_BOX|wxTAB_TRAVERSAL
#define SYMBOL_DLGADDNEWCMD_TITLE _("Ìí¼ÓÐÂÃüÁî")
#define SYMBOL_DLGADDNEWCMD_IDNAME ID_DLGADDNEWCMD
#define SYMBOL_DLGADDNEWCMD_SIZE wxDefaultSize
#define SYMBOL_DLGADDNEWCMD_POSITION wxDefaultPosition
#define CMD_FLAGS_COPY 0x1
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
	DlgAddNewCmd(const long cmdID);
	DlgAddNewCmd(const wxString& cmdID);
    DlgAddNewCmd( wxWindow* parent, wxWindowID id = SYMBOL_DLGADDNEWCMD_IDNAME, const wxString& caption = SYMBOL_DLGADDNEWCMD_TITLE, const wxPoint& pos = SYMBOL_DLGADDNEWCMD_POSITION, const wxSize& size = SYMBOL_DLGADDNEWCMD_SIZE, long style = SYMBOL_DLGADDNEWCMD_STYLE );

    /// Creation
    bool Create( wxWindow* parent, wxWindowID id = SYMBOL_DLGADDNEWCMD_IDNAME, const wxString& caption = SYMBOL_DLGADDNEWCMD_TITLE, const wxPoint& pos = SYMBOL_DLGADDNEWCMD_POSITION, const wxSize& size = SYMBOL_DLGADDNEWCMD_SIZE, long style = SYMBOL_DLGADDNEWCMD_STYLE );
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
	void OnToggle(wxCommandEvent& e);
	void OnCmdUpdate(wxCommandEvent& e);
	void SetFlags(int e){flags = e;};
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

////@end DlgAddNewCmd member variables
private:

protected:
	int cmdID;
	int flags;
	wxCheckBox* HideRun;
	wxCheckBox* RunAs;
	wxCheckBox* RequiredArg;
	wxCheckBox* isSingleProc;
	wxTextCtrl *luaCmd;
	wxBoxSizer* luaBoxSizer;
	wxBoxSizer* cmdBoxSizer;
	wxBoxSizer* itemBoxSizerToggle;
	wxToggleButton* m_toggleButton;
	wxBoxSizer* itemBoxSizer2;
};

#endif
    // _DLGADDNEWCMD_H_
