#include "MerryController.h"

MerryController* g_controller = NULL;

void MerryController::GetMousePosition(int& x, int& y) const
{
	wxPoint mousePoint = ::wxGetMousePosition();
	x = mousePoint.x;
	y = mousePoint.y;
}

void MerryController::SetMousePosition(int x, int y)
{
	m_simulator.MouseMove(x, y);
}

void MerryController::EnterText(const wxString& text)
{
	m_simulator.Text(text);
}
