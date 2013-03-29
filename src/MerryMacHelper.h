#ifndef _MERRY_MAC_HELPER_H_
#define _MERRY_MAC_HELPER_H_

#include "MerryWx.h"

#ifdef __WXOSX__

void MerryDisableFocusRing(WXWidget widget);
void MerryActivateIgnoringOtherApps();

#endif

#endif
