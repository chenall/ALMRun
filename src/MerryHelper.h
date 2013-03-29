#ifndef _MERRY_HELPER_H_
#define _MERRY_HELPER_H_

#include "MerryWx.h"

void MerryParseCommandStr(const wxString& commandStr, wxString& commandName, wxString& commandArg);
bool MerryParseKeyStr(const wxString& keyStr, int& modifiers, int& keyCode);

#endif
