#include "MerryHelper.h"
#include "MerryKey.h"
/*
void MerryParseCommandStr(const wxString& commandStr, wxString& commandName, wxString& commandArg)
{
	bool inQM = false;
	bool inText = false;
	bool inSpace = false;

	// http://alter.org.ua/docs/win/args/
	size_t i;
	for (i=0; i<commandStr.size(); ++i)
	{
		wxChar c = commandStr[i];

		if (inQM)
		{
			if (c == '\"')
				inQM = false;
			else
				commandName += c;
		} 
		else 
		{
			switch (c)
			{
			case '\"':
				inQM = true;
				inText = true;
				if (inSpace)
					goto getParam;
				inSpace = false;
				break;
			case ' ':
			case '\t':
			case '\n':
			case '\r':
				inText = false;
				inSpace = true;
				break;
			default:
				inText = true;
				if (inSpace)
					goto getParam;
				commandName += c;
				inSpace = false;
				break;
			}
		}
	}

getParam:
	if (i < commandStr.size())
		commandArg = commandStr.Mid(i);
}
*/
bool MerryParseKeyStr(const wxString& keyStr, int& modifiers, int& keyCode)
{
	wxArrayString modTokens;
	wxString keyToken;
	wxString token;

	for (size_t i=0; i<keyStr.size(); ++i)
	{
		const wxChar& c = keyStr[i];
		switch (c)
		{
		case wxT('-'):
		case wxT('+'):
			modTokens.push_back(token);
			token.clear();
			break;
		case wxT(' '):
			break;
		default:
			token += c;
			break;
		}
	}
	keyToken = token;

	modifiers = wxMOD_NONE;
	for (size_t i=0; i<modTokens.size(); ++i)
	{
		const wxString& token = modTokens[i];
		if (token.IsSameAs(wxT("A"), false) || token.IsSameAs(wxT("Alt"),false))
			modifiers |= wxMOD_ALT;
		else if (token.IsSameAs(wxT("C"), false) || token.IsSameAs(wxT("Ctrl"),false))
			modifiers |= wxMOD_CONTROL;
		else if (token.IsSameAs(wxT("W"), false) || token.IsSameAs(wxT("Win"),false))
			modifiers |= wxMOD_WIN;
		else if (token.IsSameAs(wxT("S"), false) || token.IsSameAs(wxT("Shift"),false))
			modifiers |= wxMOD_SHIFT;
		else if (token.IsSameAs(wxT("M"), false))
			modifiers |= wxMOD_META;
		else
			return false;
	}

	keyCode = 0;
	keyToken.UpperCase();
	if (keyToken.size() == 1)
	{
		keyCode = *((const char*)keyToken.c_str());
		if (!isalnum(keyCode))
#ifdef __WXMSW__
			keyCode = g_keys.GetWindowsKeyCode(keyToken);
#else
			keyCode = g_keys.GetWxKeyCode(keyToken);
#endif
	}
	else
	{
#ifdef __WXMSW__
		keyCode = g_keys.GetWindowsKeyCode(keyToken);
#else
		keyCode = g_keys.GetWxKeyCode(keyToken);
#endif
	}

	return keyCode != 0;
}
