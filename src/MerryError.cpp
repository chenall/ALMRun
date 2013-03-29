#include "MerryError.h"

static wxString s_lastError;

const wxString& MerryGetLastError()
{
	return s_lastError;
}

void MerrySetLastError(const wxString& error)
{
	s_lastError = error;
}
