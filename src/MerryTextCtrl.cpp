#include "MerryTextCtrl.h"

#include "MerryHelper.h"
#include "MerryLua.h"
#include "MerryApp.h"
#include "MerryListBoxPanel.h"
#include "DlgAddNewCmd.h"

#ifdef __WXOSX__
#include "MerryMacHelper.h"
#endif

#ifdef __WXMSW__
	#include <wx/msw/registry.h>
	//#include "Shlwapi.h"
//#pragma comment(lib, "ShLwApi.Lib")
	#pragma comment(lib, "imm32.lib")
#endif

int cmdrun_flags;
bool LocationExec;
#ifdef __WXMSW__
	HWND HWND_TEXTCTRL = NULL;
#endif


BEGIN_EVENT_TABLE(MerryTextCtrl, wxTextCtrl)
	EVT_CONTEXT_MENU(MerryTextCtrl::onContextMenu)
	EVT_MOUSE_EVENTS(OnMouseEvent)
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
		wxPoint(skin->get(EDIT_CTRL_LEFT),skin->get(EDIT_CTRL_TOP)),
		wxSize(skin->get(EDIT_CTRL_WIDTH),skin->get(EDIT_CTRL_HEIGHT)),
		wxWANTS_CHARS | wxBORDER_NONE
	)
{
#ifdef __WXOSX__
	MerryDisableFocusRing(this->GetHandle());
#endif

#ifdef __WXGTK__
	m_needCompletion = false;
#endif

	this->SetForegroundColour(skin->get(TEXT_COLOR));
	this->SetBackgroundColour(skin->get(TEXT_BACKCOLOR));
	//this->SetBackgroundStyle(wxBG_STYLE_TRANSPARENT);
	this->EnterArgs = 0;
	HWND_TEXTCTRL = this->GetHWND();
#ifdef __WXMSW__
	hkl = NULL;
	wxRegKey reg("HKCU\\Keyboard Layout\\Preload");
	if (!reg.Open())
		return;
	size_t max_Keys;
	reg.GetKeyInfo(NULL,NULL,&max_Keys,NULL);
	wxString Value;
	wxString tmp;
	//Windows 8 00000804 是中文输入法
	DWORD winver = ::GetVersion()&0xFFFF;
	winver = ((winver & 0xff)<<8) | winver>>8;
	bool isWin8 = winver > wxWinVersion_7;
	long n = 0;
	while(reg.GetNextValue(Value,n))
	{
		if (reg.QueryValue(Value,tmp))
		{
			if (tmp == "00000409" || tmp == "00000809" || (!isWin8 && tmp == "00000804"))
			{
				hkl = ::LoadKeyboardLayout(tmp,KLF_ACTIVATE);//有找到的英文键盘,就装载它
				break;
			}
		}
	}
	HKL hk1 = hkl?hkl:GetKeyboardLayout(0);
	ActivateKeyboardLayout(hk1, KLF_SUBSTITUTE_OK|KLF_SETFORPROCESS);
#endif
//	::SHAutoComplete(this->GetHWND(),15);
}

void MerryTextCtrl::onContextMenu(wxContextMenuEvent& e)
{
//	e.StopPropagation();
	e.Skip();
}

void MerryTextCtrl::OnMouseEvent(wxMouseEvent& e)
{
	e.Skip();
}

void MerryTextCtrl::SetPluginMode(const MerryCommand* cmd)
{
	wxString name = cmd->GetCommandName(0);
	if (!name.empty() && name.size() > this->GetValue().size())
		this->ChangeValue(name.Lower());
	this->EnterArgs = -1;
	//this->AppendText(" ");
	this->AutoCompletion(WXK_SPACE);
}

#ifdef __WXMSW__

void MerryTextCtrl::SetEnInputMode(void)
{
	if (hkl)
		ActivateKeyboardLayout(hkl,KLF_SETFORPROCESS);
	HIMC hImc;
	DWORD dwConv, dwSent;
	HWND hwnd = this->GetHWND();
	hImc = ImmGetContext(hwnd);
	ImmGetConversionStatus(hImc, &dwConv, &dwSent);
	ImmSetConversionStatus(hImc, 0, dwSent);
	ImmReleaseContext(hwnd, hImc);
}
#endif

void MerryTextCtrl::OnKeyDownEvent(wxKeyEvent& e)
{
	MerryListBoxPanel* listBoxPanel = ::wxGetApp().GetFrame().GetListBoxPanel();
	const MerryCommand* SelectedCMD;
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
		SelectedCMD = listBoxPanel->GetSelectionCommand();
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
		else if (keyCode == WXK_SPACE && EnterArgs == 0)
		{
			//对于插件命令,按空格之后进入插件命令模式.
			if (SelectedCMD->GetFlags() == CMDS_FLAG_PLUGIN)
				this->SetPluginMode(SelectedCMD);
			else if (g_config->get(SpaceKey))
				keyCode = WXK_RETURN;
		}
#endif
	}
	switch (keyCode)
	{
		case WXK_RETURN:
		case WXK_NUMPAD_ENTER:
			if (e.ShiftDown()) cmdrun_flags |= CMDRUN_FLAG_RUNAS;
			this->ExecuteCmd();
			break;
		case WXK_ESCAPE:
			if (this->EnterArgs>0)//当前是输入参数模式,按Esc取消
			{
				this->ChangeValue(this->GetValue().substr(0,this->EnterArgs-2));
				this->EnterArgs = 0;
				this->SetSelection(0,-1);
			}
			else if (this->GetValue().size())//有输入的内容，按Esc清除内容
			{
				this->EnterArgs = 0;
				this->Clear();
			}
			else//没有输入，按Esc隐藏
				::wxGetApp().GetFrame().Hide();
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
				if (SelectedCMD->GetFlags() == CMDS_FLAG_PLUGIN && this->EnterArgs == 0)//插件命令
				{
					SetPluginMode(SelectedCMD);
					break;
				}
				this->ChangeValue(SelectedCMD->GetCommandName());
				this->AppendText(wxT(">>"));
				this->EnterArgs = this->GetValue().size();
				listBoxPanel->flags = 1;
				MerryCommandArray commands;
				commands.push_back(const_cast<MerryCommand*>(SelectedCMD));
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
	if (this->EnterArgs <= 0 && m_lastKeyDownCode != WXK_TAB)
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
	wxString key = this->GetValue();
	MerryListBoxPanel* listBoxPanel = ::wxGetApp().GetFrame().GetListBoxPanel();
	wxString commandArg = (this->EnterArgs>0)?key.substr(this->EnterArgs):wxEmptyString;
	if (!listBoxPanel->IsPopup())
		return;
	const MerryCommand* command = listBoxPanel->GetSelectionCommand();
	if (command->GetCmd().empty())
	{//命令为空,插件命令
		SetPluginMode(command);
		return;
	}
	#ifdef __WXMSW__
	if (command->GetCmd().Find("{%p+}") != wxNOT_FOUND && this->EnterArgs <= 0)
	{
		SendMessage(HWND_TEXTCTRL,WM_KEYDOWN,VK_TAB,0);
		SendMessage(HWND_TEXTCTRL,WM_KEYUP,VK_TAB,0);
		return;
	}
	#endif
	if (g_config->get(RememberFavouratMatch))
		g_config->SetFavorite((this->EnterArgs > 0)?key.Left(this->EnterArgs - 2):key,command->GetCommandName());

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

	if (name.empty())//输入框内容为空时要恢复当前状态
		this->EnterArgs = 0;

	if (from != to)
		name.Truncate(from);

	if (listBoxPanel->IsPopup() && keyCode == WXK_DOWN)
		listBoxPanel->SelectNext();
	else if (listBoxPanel->IsPopup() && keyCode == WXK_UP)
		listBoxPanel->SelectPrev();
	else// if (keyCode != WXK_TAB)
	{
		MerryCommandArray commands;
		if (this->EnterArgs >= 0)
		{
			commands = g_commands->Collect(name);
#ifdef _ALMRUN_CONFIG_H_
			//数字热键启用
			if (g_config->get(NumberKey) && commands.size() == 0 && (keyCode == ' ' || (keyCode >= '0' && keyCode <= '9')))
			{
				if (keyCode == ' ' || listBoxPanel->SetSelection(-1,(keyCode & 0xf)))
					return this->ExecuteCmd();
			}
		}
#ifdef _ALMRUN_CONFIG_H_
		if (g_config->get(ExecuteIfOnlyOne) && commands.size() == 1)
		{
			listBoxPanel->SetCommandArray(commands);
			listBoxPanel->Popup();
			return this->ExecuteCmd();
		}
#endif//#ifdef _ALMRUN_CONFIG_H_
		if (!g_config->get(ShowTopTen) || commands.size() < 10)
#endif//#ifdef _ALMRUN_CONFIG_H_
			g_commands->GetPluginCmd(name,commands);

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
