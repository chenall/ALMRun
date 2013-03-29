#include "MerryMacHelper.h"

#ifdef __WXOSX__

#import <cocoa/cocoa.h>

void MerryDisableFocusRing(WXWidget widget)
{
	[widget setFocusRingType:NSFocusRingTypeNone];
}

void MerryActivateIgnoringOtherApps()
{
	NSApplication* merry = [NSApplication sharedApplication];
	[merry activateIgnoringOtherApps:YES];
}

#endif
