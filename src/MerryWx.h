#ifndef _MERRY_WX_H_
#define _MERRY_WX_H_

#include <wx/wxprec.h>
#ifdef __BORLANDC__
	#pragma hdrstop
#endif
#ifndef WX_PRECOMP
	#include <wx/wx.h>
#endif
#ifdef __WXMSW__
	#if (_DEBUG || _RELWITHDEBINFO)
		#define VLD_FORCE_ENABLE
		#include "vld.h"
	#endif
#endif
#endif