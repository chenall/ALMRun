#include "MerryTextCtrl.h"
#include "MerryConfig.h"
#include "MerryCommandManager.h"
#include "MerryHelper.h"
#include "MerryLua.h"
#include "MerryApp.h"
#include "MerryListBoxPanel.h"

#ifdef __WXOSX__
#include "MerryMacHelper.h"
#endif

BEGIN_EVENT_TABLE(MerryTextCtrl, wxTextCtrl)
	EVT_KEY_DOWN(MerryTextCtrl::OnKeyDownEvent)
#ifdef __WXGTK__
	EVT_IDLE(MerryTextCtrl::OnIdleEvent)
#else
	EVT_TEXT(wxID_ANY, MerryTextCtrl::OnTextEvent)
#endif
END_EVENT_TABLE()

MerryTextCtrl::MerryTextCtrl(wxWindow* parent):
	wxTextCtrl(parent, wxID_ANY, wxEmptyString,
		wxPoint(MERRY_DEFAULT_TEXT_CTRL_LEFT, MERRY_DEFAULT_TEXT_CTRL_TOP),
		wxSize(MERRY_DEFAULT_TEXT_CTRL_WIDTH, MERRY_DEFAULT_TEXT_CTRL_HEIGHT),
		wxWANTS_CHARS | wxBORDER_NONE
	)
{
#ifdef __WXOSX__
	MerryDisableFocusRing(this->GetHandle());
#endif

#ifdef __WXGTK__
	m_needCompletion = false;
#endif

	if (MERRY_DEFAULT_TEXT_CTRL_FONT_HEIGHT)
	{
		wxFont font = this->GetFont();
		font.SetPixelSize(wxSize(0, MERRY_DEFAULT_TEXT_CTRL_FONT_HEIGHT));
		this->SetOwnFont(font);
	}
	this->EnterArgs = 0;
}

void MerryTextCtrl::OnKeyDownEvent(wxKeyEvent& e)
{
	MerryListBoxPanel* listBoxPanel = ::wxGetApp().GetFrame().GetListBoxPanel();
	assert(listBoxPanel);
	int keyCode = e.GetKeyCode();
	m_lastKeyDownCode = keyCode;
	if (listBoxPanel->IsPopup())
	{
		if (e.AltDown() && keyCode >= '0' && keyCode <= '9')
		{
			if (!listBoxPanel->SetSelection(-1,(keyCode & 0xf)))
				return;
			keyCode = WXK_RETURN;
		}
		else if (e.ControlDown() && (keyCode | 0x20) == 'd')
		{
			keyCode = WXK_RETURN;
		}
	}
	switch (keyCode)
	{
		case WXK_RETURN:
		case WXK_NUMPAD_ENTER:
			{
				if (!listBoxPanel->IsPopup())
					break;
				wxString commandStr = this->GetValue();
				wxString commandArg = (this->EnterArgs>0)?commandStr.substr(this->EnterArgs):wxT('');
				const MerryCommand* command = listBoxPanel->GetSelectionCommand();
				const wxString& commandName = command->GetCommandName();
				::wxGetApp().GetFrame().Hide();
				if (commandName.size())
					command->ExecuteCommand(commandArg);
				else
					g_lua->OnUndefinedCommand(commandName, commandArg);
			}
			break;
		case WXK_ESCAPE:
			if (this->EnterArgs == 0)
				::wxGetApp().GetFrame().Hide();
			else//当前是输入参数模式,按Esc取消
			{
				this->ChangeValue(this->GetValue().substr(0,this->EnterArgs-2));
				this->EnterArgs = 0;
				this->SetSelection(0,-1);
			}
			break;
		case WXK_HOME:
			this->SetInsertionPoint(this->EnterArgs);
			break;
		case WXK_DOWN:
		case WXK_UP:
			this->AutoCompletion(keyCode);
	#ifdef __WXGTK__
			m_lastValue = this->GetValue();
	#endif
			break;
		case WXK_TAB:
			if (!listBoxPanel->IsPopup())
				break;
			this->ChangeValue(listBoxPanel->GetSelectionCommand()->GetCommandName());
			this->AppendText(wxT(">>"));
			this->EnterArgs = this->GetValue().size();
			listBoxPanel->flags = 1;
			listBoxPanel->Refresh();
	#ifdef __WXGTK__
			m_needCompletion = true;
	#endif
			break;
		default:
	#ifdef __WXGTK__
			m_needCompletion = true;
	#endif
			if (this->GetInsertionPoint()<this->EnterArgs)
				this->SetInsertionPoint(-1);
			else if (this->GetInsertionPoint()==this->EnterArgs)
			{
				if (keyCode == WXK_LEFT || keyCode == WXK_BACK)//设置光标在命令之后
					break;
			}
			e.Skip();
			break;
	}
}

#ifdef __WXGTK__
void MerryTextCtrl::OnIdleEvent(wxIdleEvent& e)
{
	if (!m_needCompletion)
		return;
	const wxString& value = this->GetValue();
	if (m_lastValue == value)
		return;

	this->AutoCompletion(m_lastKeyDownCode);

	m_needCompletion = false;
	m_lastValue = value;
}
#else
void MerryTextCtrl::OnTextEvent(wxCommandEvent& e)
{
	if (this->EnterArgs == 0 && m_lastKeyDownCode != WXK_TAB)
		return this->AutoCompletion(m_lastKeyDownCode);
}
#endif

void MerryTextCtrl::AutoCompletion(int keyCode)
{
	MerryListBoxPanel* listBoxPanel = ::wxGetApp().GetFrame().GetListBoxPanel();
	assert(listBoxPanel);

	long from, to;
	this->GetSelection(&from, &to);
	wxString name = this->GetValue();
	if (from != to)
		name.Truncate(from);

	if (listBoxPanel->IsPopup() && keyCode == WXK_DOWN)
		listBoxPanel->SelectNext();
	else if (listBoxPanel->IsPopup() && keyCode == WXK_UP)
		listBoxPanel->SelectPrev();
	else if (keyCode != WXK_TAB)
	{
		MerryCommandArray commands = g_commands->Collect(name);
		listBoxPanel->SetCommandArray(commands);
		if (commands.size() == 0)
		{
			listBoxPanel->Dismiss();
			return;
		}
		listBoxPanel->Popup();
	}

	// Auto completion
	/*
	/if (keyCode != WXK_BACK && keyCode != WXK_DELETE )
	{
		const MerryCommand* command = listBoxPanel->GetSelectionCommand();
		const wxString& commandName = command->GetCommandName();
		this->ChangeValue(commandName);
		this->SetSelection(name.size(), commandName.size());
	}
	*/
}
