#include "MerryApp.h"

IMPLEMENT_APP(MerryApp)

bool MerryApp::OnInit()
{
	if (!wxApp::OnInit())
		return false;

	m_frame = NULL;
	this->NewFrame();
	assert(m_frame);
	return true;
}

void MerryApp::NewFrame()
{
	if (m_frame)
	{
		bool ok = m_frame->Close();
		assert(ok);
	}
	
	m_frame = new MerryFrame();
	m_frame->OnInit();
}

MerryFrame& MerryApp::GetFrame()
{
	assert(m_frame);
	return *m_frame;
}
