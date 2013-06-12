/////////////////////////////////////////////////////////////////////////////
// Name:        DlgConfig.h
// Purpose:     
// Author:      test
// Modified by: 
// Created:     12/06/2013 09:14:07
// RCS-ID:      
// Copyright:   test
// Licence:     
/////////////////////////////////////////////////////////////////////////////

#ifndef _DLGCONFIG_H_
#define _DLGCONFIG_H_

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma interface "DlgConfig.h"
#endif

/*!
 * Includes
 */

////@begin includes
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
#define ID_DLGCONFIG 10017
#define ID_CHECKLISTBOX 10018
#define ID_TEXTCTRL 10008
#define SYMBOL_DLGCONFIG_STYLE wxCAPTION|wxRESIZE_BORDER|wxSYSTEM_MENU|wxCLOSE_BOX|wxTAB_TRAVERSAL
#define SYMBOL_DLGCONFIG_TITLE wxGetTranslation(wxString(wxT("ALMRun ")) + (wxChar) 0x53C2 + (wxChar) 0x6570 + (wxChar) 0x914D + (wxChar) 0x7F6E)
#define SYMBOL_DLGCONFIG_IDNAME ID_DLGCONFIG
#define SYMBOL_DLGCONFIG_SIZE wxSize(400, 300)
#define SYMBOL_DLGCONFIG_POSITION wxDefaultPosition
////@end control identifiers


/*!
 * DlgConfig class declaration
 */

class DlgConfig: public wxDialog
{    
    DECLARE_DYNAMIC_CLASS( DlgConfig )
    DECLARE_EVENT_TABLE()

public:
    /// Constructors
    DlgConfig();
    DlgConfig( wxWindow* parent, wxWindowID id = SYMBOL_DLGCONFIG_IDNAME, const wxString& caption = SYMBOL_DLGCONFIG_TITLE, const wxPoint& pos = SYMBOL_DLGCONFIG_POSITION, const wxSize& size = SYMBOL_DLGCONFIG_SIZE, long style = SYMBOL_DLGCONFIG_STYLE );

    /// Creation
    bool Create( wxWindow* parent, wxWindowID id = SYMBOL_DLGCONFIG_IDNAME, const wxString& caption = SYMBOL_DLGCONFIG_TITLE, const wxPoint& pos = SYMBOL_DLGCONFIG_POSITION, const wxSize& size = SYMBOL_DLGCONFIG_SIZE, long style = SYMBOL_DLGCONFIG_STYLE );

    /// Destructor
    ~DlgConfig();

    /// Initialises member variables
    void Init();

    /// Creates the controls and sizers
    void CreateControls();

////@begin DlgConfig event handler declarations
	void OnConfigCheck(wxCommandEvent& e);
	void OnCheck(wxCommandEvent& e);
	void OnMouseEvent(wxMouseEvent& e);
////@end DlgConfig event handler declarations

////@begin DlgConfig member function declarations

    /// Retrieves bitmap resources
    wxBitmap GetBitmapResource( const wxString& name );

    /// Retrieves icon resources
    wxIcon GetIconResource( const wxString& name );
////@end DlgConfig member function declarations

    /// Should we show tooltips?
    static bool ShowToolTips();

////@begin DlgConfig member variables
    wxCheckListBox* config;
	wxTextCtrl* config_hotkey;
	wxStaticText* config_tip;
////@end DlgConfig member variables
};

#endif
    // _DLGCONFIG_H_
