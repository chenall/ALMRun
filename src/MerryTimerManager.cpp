#include "MerryTimerManager.h"
#include "MerryTimer.h"

MerryTimerManager* g_timers = NULL;

MerryTimerManager::~MerryTimerManager()
{
	__DEBUG_BEGIN("")
	std::set<MerryTimer*>::iterator it = m_timers.begin();
	for (; it != m_timers.end(); ++it)
		delete *it;
	__DEBUG_END("")
}

void* MerryTimerManager::SetTimer(int millisecond, bool oneShot, int funcRef)
{
	MerryTimer* timer = new MerryTimer(funcRef);
	m_timers.insert(timer);
	timer->Start(millisecond, oneShot);
	return timer;
}

void MerryTimerManager::ClearTimer(void* timerHandler)
{
	MerryTimer* timer = (MerryTimer*)timerHandler;
	if (m_timers.find(timer) == m_timers.end())
		return;
	m_timers.erase(timer);
	delete timer;
}
