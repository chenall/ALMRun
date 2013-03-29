#ifndef _MERRY_TIMER_H_
#define _MERRY_TIMER_H_

#include "MerryWx.h"

class MerryTimer : public wxTimer
{
public:
	MerryTimer(int funcRef);
	~MerryTimer();

private:
	virtual void Notify();

private:
	int m_callback;
};

#endif