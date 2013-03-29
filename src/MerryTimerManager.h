#ifndef _MERRY_TIMER_MANAGER_H_
#define _MERRY_TIMER_MANAGER_H_

#include <set>

class MerryTimer;
class MerryTimerManager
{
public:
	~MerryTimerManager();

	void* SetTimer(int millisecond, bool oneShot, int funcRef);
	void ClearTimer(void* timerHandler);

private:
	std::set<MerryTimer*> m_timers;
};

extern MerryTimerManager* g_timers;

#endif
