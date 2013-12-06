#include "MerryError.h"
#include "ALMRunConfig.h"
#include "MerryInformationDialog.h"

static wxString s_lastError;

const wxString& MerryGetLastError()
{
	return s_lastError;
}

void MerrySetLastError(const wxString& error)
{
	s_lastError = error;
}

void ShowErrinfo(const int error_type)
{
	if (s_lastError.empty() || !g_config)
		return;

	if (error_type< CONFIG_ERROR_TYPE)
	{
		wxMessageBox("提示",s_lastError,wxICON_INFORMATION);
		return;
	}
	if (!g_config->get((CONFIG_ITEM_T)error_type))
		return;
	MerryInformationDialog *dlg = new MerryInformationDialog("提示",s_lastError,wxEmptyString);
	dlg->ShowModal();
	if (dlg->isChecked())
		g_config->set(error_type,false);
}