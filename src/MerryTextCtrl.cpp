#include "MerryTextCtrl.h"
#include "MerryConfig.h"
#include "MerryCommandManager.h"
#include "MerryHelper.h"
#include "MerryLua.h"
#include "MerryApp.h"
#include "MerryListBoxPanel.h"
#include "DlgAddNewCmd.h"

#ifdef __WXOSX__
#include "MerryMacHelper.h"
#endif

bool LocationExec;
#ifdef __WXMSW__
	HWND HWND_TEXTCTRL = NULL;
#endif

BEGIN_EVENT_TABLE(MerryTextCtrl, wxTextCtrl)
	EVT_CONTEXT_MENU(MerryTextCtrl::onContextMenu)
	EVT_KEY_DOWN(MerryTextCtrl::OnKeyDownEvent)
#ifdef __WXGTK__
	EVT_IDLE(MerryTextCtrl::OnIdleEvent)
#else
	EVT_TEXT(wxID_ANY, MerryTextCtrl::OnTextEvent)
	EVT_CHAR(OnCharEvent)
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
	HWND_TEXTCTRL = this->GetHWND();
}

void MerryTextCtrl::onContextMenu(wxContextMenuEvent& e)
{
	e.StopPropagation();
	e.Skip();
}

void MerryTextCtrl::OnKeyDownEvent(wxKeyEvent& e)
{
	MerryListBoxPanel* listBoxPanel = ::wxGetApp().GetFrame().GetListBoxPanel();
	assert(listBoxPanel);
	int keyCode = e.GetKeyCode();
	m_lastKeyDownCode = keyCode;
#ifdef __WXMSW__
	int menu_id = 0;
	if (e.AltDown())
	{
		int key = keyCode | 0x20;

		switch(key)
		{
			case 'c':// Alt+C 呼出配置设定界面。
				menu_id = MENU_ITEM_GUI_CONFIG;
				break;
			case 's'://Alt+S 呼出快捷项管理界面；
				menu_id = MENU_ITEM_CMDMGR;
				break;
			case 'x'://Alt+X 退出
				menu_id = MENU_ITEM_EXIT;
				break;
		}

	}
	else if (keyCode == WXK_F1)
		menu_id = MENU_ITEM_ABOUT;

	if (menu_id)
	{
		HWND hwnd = this->GetParent()->GetHWND();
		PostMessage(hwnd,WM_COMMAND,menu_id,0);
		return;
	}
#endif
	if (listBoxPanel->IsPopup())
	{
		if ((e.AltDown() || e.ControlDown()) && keyCode >= '0' && keyCode <= '9')
		{
			if (!listBoxPanel->SetSelection(-1,(keyCode & 0xf)))
				return;
			keyCode = WXK_RETURN;
		}
		else if (e.ControlDown() && (keyCode | 0x20) == 'd')
		{
			LocationExec = true;
			keyCode = WXK_RETURN;
		}
		else if (keyCode == WXK_DELETE && this->GetInsertionPoint() == this->GetValue().size())
		{
			if (listBoxPanel->DelSelectedItem())
				this->AppendText("");
			e.Skip();
		}
		else if (keyCode == WXK_INSERT)
		{
			DlgAddNewCmd* dlg = new DlgAddNewCmd(this);
			dlg->ShowModal();
			dlg->Destroy();
			return;
		}
#ifdef __WXMSW__
		else if (keyCode == WXK_F2)
		{
			PostMessage(listBoxPanel->GetHWND(),WM_COMMAND,MENU_CMD_EDIT,0);
			return;
		}
#endif

#ifdef _ALMRUN_CONFIG_H_
		//按空格键执行
		else if (keyCode == WXK_SPACE && g_config->config[SpaceKey] && EnterArgs == 0)
			keyCode = WXK_RETURN;
#endif
	}
	switch (keyCode)
	{
		case WXK_RETURN:
		case WXK_NUMPAD_ENTER:
			this->ExecuteCmd();
			break;
		case WXK_ESCAPE:
			if (this->GetValue().size())
				this->Clear();
			else if (this->EnterArgs == 0)
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
			{
				if (this->EnterArgs == 0 && this->GetValue().size())
				{
					this->AppendText(wxT(">>"));
					this->EnterArgs = this->GetValue().size();
				}
				break;
			}
			else
			{
				this->ChangeValue(listBoxPanel->GetSelectionCommand()->GetCommandName());
				this->AppendText(wxT(">>"));
				this->EnterArgs = this->GetValue().size();
				listBoxPanel->flags = 1;
				MerryCommandArray commands;
				commands.push_back(const_cast<MerryCommand*>(listBoxPanel->GetSelectionCommand()));
				listBoxPanel->SetCommandArray(commands);
				listBoxPanel->Popup();
			}
	#ifdef __WXGTK__
			m_needCompletion = true;
	#endif
			break;
		default:
	#ifdef __WXGTK__
			m_needCompletion = true;
	#endif
			if (this->EnterArgs > 0)
			{
				if (this->GetInsertionPoint()<this->EnterArgs)
					this->SetInsertionPoint(-1);
				else if (this->GetInsertionPoint()==this->EnterArgs)
				{
					if (keyCode == WXK_LEFT || keyCode == WXK_BACK)//设置光标在命令之后
						break;
				}
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

void MerryTextCtrl::OnCharEvent(wxKeyEvent& e)
{
	if (e.GetKeyCode() != WXK_RETURN)
		e.Skip();
}
#endif
void MerryTextCtrl::ExecuteCmd()
{
	MerryListBoxPanel* listBoxPanel = ::wxGetApp().GetFrame().GetListBoxPanel();
	wxString commandArg = (this->EnterArgs>0)?this->GetValue().substr(this->EnterArgs):wxEmptyString;
	if (!listBoxPanel->IsPopup())
		return;
	const MerryCommand* command = listBoxPanel->GetSelectionCommand();
	if (command->GetCmd().empty())
		return;
	#ifdef __WXMSW__
	if (command->GetCmd().Find("{%p+}") != wxNOT_FOUND && this->EnterArgs <= 0)
	{
		SendMessage(HWND_TEXTCTRL,WM_KEYDOWN,VK_TAB,0);
		SendMessage(HWND_TEXTCTRL,WM_KEYUP,VK_TAB,0);
		return;
	}
	#endif
	::wxGetApp().GetFrame().Hide();
	command->Execute(commandArg);
}

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
	else// if (keyCode != WXK_TAB)
	{
		MerryCommandArray commands = g_commands->Collect(name);
#ifdef _ALMRUN_CONFIG_H_
		//数字热键启用

		if (g_config->config[NumberKey] && commands.size() == 0 && (keyCode == ' ' || (keyCode >= '0' && keyCode <= '9')))
		{
			if (keyCode != ' ' && !listBoxPanel->SetSelection(-1,(keyCode & 0xf)))
				return;
			return this->ExecuteCmd();
		}
#endif//#ifdef _ALMRUN_CONFIG_H_
		listBoxPanel->SetCommandArray(commands);
		if (commands.size() == 0)
		{
			listBoxPanel->Dismiss();
			return;
		}
		listBoxPanel->Popup();
#ifdef _ALMRUN_CONFIG_H_
		if (g_config->config[ExecuteIfOnlyOne] && commands.size() == 1)
			return this->ExecuteCmd();
#endif//#ifdef _ALMRUN_CONFIG_H_
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
