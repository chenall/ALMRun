#ifndef _MERRY_APP_H_
#define _MERRY_APP_H_

#include "MerryFrame.h"

class MerryApp : public wxApp
{
public:
    virtual bool OnInit();
	void NewFrame();
	MerryFrame& GetFrame();

private:
	MerryFrame* m_frame;
};

DECLARE_APP(MerryApp)

#endif