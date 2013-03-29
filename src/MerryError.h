#ifndef _MERRY_ERROR_H_
#define _MERRY_ERROR_H_

#include <wx/string.h>

const wxString& MerryGetLastError();
void MerrySetLastError(const wxString& error);

#endif
